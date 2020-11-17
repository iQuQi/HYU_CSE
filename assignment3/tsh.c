/*
 * tsh - A tiny shell program with job control
 *
 * <Put your name and ID here>
 */
//답지파일- 프로젝트 연결파일
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

 /* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

 /* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
	pid_t pid;              /* job PID */
	int jid;                /* job ID [1, 2, ...] */
	int state;              /* UNDEF, BG, FG, or ST */
	char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv);
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs);
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid);
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
	char c;
	char cmdline[MAXLINE];
	int emit_prompt = 1; /* emit prompt (default) */

	/* Redirect stderr to stdout (so that driver will get all output
	 * on the pipe connected to stdout) */
	dup2(1, 2);

	/* Parse the command line */
	while ((c = getopt(argc, argv, "hvp")) != EOF) {
		switch (c) {
		case 'h':             /* print help message */
			usage();
			break;
		case 'v':             /* emit additional diagnostic info */
			verbose = 1;
			break;
		case 'p':             /* don't print a prompt */
			emit_prompt = 0;  /* handy for automatic testing */
			break;
		default:
			usage();
		}
	}

	/* Install the signal handlers */

	/* These are the ones you will need to implement */
	Signal(SIGINT, sigint_handler);   /* ctrl-c */
	Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
	Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

	/* This one provides a clean way to kill the shell */
	Signal(SIGQUIT, sigquit_handler);

	/* Initialize the job list */
	initjobs(jobs);

	/* Execute the shell's read/eval loop */
	while (1) {

		/* Read command line */
		if (emit_prompt) {
			printf("%s", prompt);
			fflush(stdout);
		}
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
			app_error("fgets error");
		if (feof(stdin)) { /* End of file (ctrl-d) */
			fflush(stdout);
			exit(0);
		}

		/* Evaluate the command line */
		eval(cmdline);
		fflush(stdout);
		fflush(stdout);
	}

	exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.

 사용자가 내장 명령어(작업, bg 또는 fg)를 요청한 경우 즉시 실행하십시오. 
 그렇지 않은 경우, 하위 프로세스에서 하위 컨텍스트에서 작업을 실행하십시오. 
 작업이 포그라운드에서 실행 중이면 작업이 종료될 때까지 기다렸다가 돌아가십시오. 
 참고: 각 하위 프로세스는 키보드에 ctrl-c(ctrl-z)를 입력할 때 배경 자녀가 
 커널에서 SIGINT(SIGTSTP)를 수신하지 않도록 고유한 프로세스 그룹 ID를 가져야 한다.

 fork = 현재 실행중인 프로세스를 복사해서 다른 프로세스를 생성
 복사해서 생성하기 때문에, 가지고 있던 메모리등의 시스템 자원을 
 모두 원래의 process와 공유하게 된다.
 fork()를 사용하여 생성한 프로세스는 부모 프로세스 Parent process, 새로 생긴 프로세스는 자식 프로세스 Child process 라고 부른다.

모든 프로세스는 (참고: 최상위 프로세스인 init는 pid 1을 가진다) 생성될 때 
프로세스 아이디를 부여받는다. fork() 함수를 이용해서 자식 프로세스를 생성할 수 있는데, 
이 때 부모에게는 자식 프로세스의 pid를 반환하고, 자식에게는 0을 반환한다. 
이를 이용하여 자식 프로세스에게 특정 명령을 시킬 수 있다.
 */
void eval(char *cmdline) //TODO
{

	
	char *cmdArr[MAXARGS];
	int bg,b_ch,e_ch;
	pid_t pid;

	//명령을 받아아서 배열안에 하나씩 저장
	bg = parseline(cmdline, cmdArr);

	//명령이 없는 경우
	if (cmdArr[0] == NULL)	return;

	//builtin 명령어면 즉시 실행
	b_ch = builtin_cmd(cmdArr);

	//buitin명령어가 아니었다면
	if (!b_ch) {
		//fork 실패
		if ((pid = fork()) < 0) {
			exit(1);
		}

		// 부모 프로세스인 경우
		else if(pid>0) {
			//background작업이 아닌경우 addjob 후 wait
			if (!bg) {
				addjob(jobs, pid, FG, cmdline);
				waitfg(pid);
			}

			//background일때 addjob
			else {
				addjob(jobs, pid, BG, cmdline);
				printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
			}
		}

		//자식프로세스인 경우
		else if (pid == 0) {
			//자식 pid와 동일한 그룹 id그룹에 넣음
			if (setpgid(0, 0) < 0) {
				exit(1);
			}
			//실행가능한 파일인 filename의 실행코드를 현재 프로세스에 적재하여 
			//기존의 실행코드와 교체하여 새로운 기능으로 실행합니다.  
			//즉, 현재 실행되는 프로그램의 기능은 없어지고 filename 프로그램을 
			//메모리에 loading하여 처음부터 실행합니다
			e_ch = execve(cmdArr[0], cmdArr, environ);

			if (e_ch < 0) {
				printf("%s: Command not found\n", cmdArr[0]);
				exit(1);
			}
		}

		
	}
	return;
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv)
{
	static char array[MAXLINE]; /* holds local copy of command line */
	char *buf = array;          /* ptr that traverses command line */
	char *delim;                /* points to first space delimiter */
	int argc;                   /* number of args */
	int bg;                     /* background job? */

	strcpy(buf, cmdline);
	buf[strlen(buf) - 1] = ' ';  /* replace trailing '\n' with space */
	while (*buf && (*buf == ' ')) /* ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	if (*buf == '\'') {
		buf++;
		delim = strchr(buf, '\'');
	}
	else {
		delim = strchr(buf, ' ');
	}

	while (delim) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* ignore spaces */
			buf++;

		if (*buf == '\'') {
			buf++;
			delim = strchr(buf, '\'');
		}
		else {
			delim = strchr(buf, ' ');
		}
	}
	argv[argc] = NULL;

	if (argc == 0)  /* ignore blank line */
		return 1;

	/* should the job run in the background? */
	if ((bg = (*argv[argc - 1] == '&')) != 0) {
		argv[--argc] = NULL;
	}
	return bg;
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv) //TODO
{
	//명령어에 따라 해당 작업 진행

	//background 작업 리스팅
	if (!strcmp(argv[0], "jobs")) {
		listjobs(jobs);
		return 1;
	}

	//프로그램 종료
	else if (!strcmp(argv[0], "quit")) {
		exit(0);
	}
	

	//&가 명령어에 있다면 background실행
	else if (!strcmp("&", argv[0])) {
		return 1;
	}
	
	//bg이거나  fg 명령어가 들어온경우 
	else if (!strcmp(argv[0], "bg") || !(strcmp(argv[0], "fg"))) {
		do_bgfg(argv);
		return 1;
	}

	
	//
	return 0;
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
	/* $begin handout */
	struct job_t *jobp = NULL;

	/* Ignore command if no argument */
	if (argv[1] == NULL) {
		printf("%s command requires PID or %%jobid argument\n", argv[0]);
		return;
	}

	/* Parse the required PID or %JID arg */
	if (isdigit(argv[1][0])) {
		pid_t pid = atoi(argv[1]);
		if (!(jobp = getjobpid(jobs, pid))) {
			printf("(%d): No such process\n", pid);
			return;
		}
	}
	else if (argv[1][0] == '%') {
		int jid = atoi(&argv[1][1]);
		if (!(jobp = getjobjid(jobs, jid))) {
			printf("%s: No such job\n", argv[1]);
			return;
		}
	}
	else {
		printf("%s: argument must be a PID or %%jobid\n", argv[0]);
		return;
	}

	/* bg command */
	if (!strcmp(argv[0], "bg")) {
		if (kill(-(jobp->pid), SIGCONT) < 0)
			unix_error("kill (bg) error");
		jobp->state = BG;
		printf("[%d] (%d) %s", jobp->jid, jobp->pid, jobp->cmdline);
	}

	/* fg command */
	else if (!strcmp(argv[0], "fg")) {
		if (kill(-(jobp->pid), SIGCONT) < 0)
			unix_error("kill (fg) error");
		jobp->state = FG;
		waitfg(jobp->pid);
	}
	else {
		printf("do_bgfg: Internal error\n");
		exit(0);
	}
	/* $end handout */

	return;
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid) //TODO
{
	//foreground 실행시에는 wait
	while (pid == fgpid(jobs))
	{
		sleep(1);
	}
	return;
}

/*****************
 * Signal handlers
 *****************/

 /*
  * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
  *     a child job terminates (becomes a zombie), or stops because it
  *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
  *     available zombie children, but doesn't wait for any other
  *     currently running children to terminate.
  */
void sigchld_handler(int sig)
{
	/* $begin handout */
	pid_t child_pid;
	int child_jid;
	int status;

	if (verbose)
		printf("sigchld_handler: entering\n");

	/* Detect any terminated or stopped jobs, but don't wait on the others. */
	while ((child_pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {

		/* Was the job stopped by the receipt of a signal? */
		if (WIFSTOPPED(status)) {
			struct job_t *j = getjobpid(jobs, child_pid);
			if (!j) {
				printf("Lost track of (%d)\n", child_pid);
				return;
			}
			j->state = ST;

			fprintf(stdout, "Job [%d] (%d) stopped by signal %d\n",
				pid2jid(child_pid), child_pid, WSTOPSIG(status));
		}

		/* Was the job terminated by the receipt of an uncaught signal? */
		else if (WIFSIGNALED(status)) {
			child_jid = pid2jid(child_pid);
			if (deletejob(jobs, child_pid))
				if (verbose)
					printf("sigchld_handler: Job [%d] (%d) deleted\n",
						child_jid, child_pid);

			fprintf(stdout, "Job [%d] (%d) terminated by signal %d\n",
				child_jid, child_pid, WTERMSIG(status));

		}

		/* Did the job terminate normally? */
		else if (WIFEXITED(status)) {
			child_jid = pid2jid(child_pid);
			if (deletejob(jobs, child_pid))
				if (verbose)
					printf("sigchld_handler: Job [%d] (%d) deleted\n",
						child_jid, child_pid);
			if (verbose) {
				printf("sigchld_handler: Job [%d] (%d) terminates OK (status %d)\n",
					child_jid, child_pid, WEXITSTATUS(status));
			}
		}
		else
			unix_error("waitpid error");
	}

	/*
	 * Check for normal termination of the waitpid loop: Either
	 * there were children, but no zombies (child_pid == 0), or there
	 * were no children at all (child_pid == -1 and errno == ECHILD).
	 */
	if (!((child_pid == 0) || (child_pid == -1 && errno == ECHILD)))
		unix_error("sigchld_handler wait error");

	if (verbose)
		printf("sigchld_handler: exiting\n");

	/* $end handout */

	return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
{
	/* $begin handout */
	pid_t pid;

	if (verbose)
		printf("sigint_handler: entering\n");
	if ((pid = fgpid(jobs)) > 0) {
		if (kill(-pid, SIGINT) < 0)
			unix_error("kill (sigint) error");
		if (verbose)
			printf("sigint_handler: Job (%d) killed\n", pid);
	}
	if (verbose)
		printf("sigint_handler: exiting\n");
	/* $end handout */

	return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
	/* $begin handout */
	pid_t pid;

	if (verbose)
		printf("sigtstp_handler: entering\n");
	if ((pid = fgpid(jobs)) > 0) {
		if (kill(-pid, SIGTSTP) < 0)
			unix_error("kill (tstp) error");
		if (verbose)
			printf("sigtstp_handler: Job [%d] (%d) stopped\n",
				pid2jid(pid), pid);
	}
	if (verbose)
		printf("sigtstp_handler: exiting\n");
	/* $end handout */

	return;
}

/*********************
 * End signal handlers
 *********************/

 /***********************************************
  * Helper routines that manipulate the job list
  **********************************************/

  /* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
	job->pid = 0;
	job->jid = 0;
	job->state = UNDEF;
	job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
	int i;

	for (i = 0; i < MAXJOBS; i++)
		clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs)
{
	int i, max = 0;

	for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].jid > max)
			max = jobs[i].jid;
	return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
	int i;

	if (pid < 1)
		return 0;

	for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == 0) {
			jobs[i].pid = pid;
			jobs[i].state = state;
			jobs[i].jid = nextjid++;
			if (nextjid > MAXJOBS)
				nextjid = 1;
			strcpy(jobs[i].cmdline, cmdline);
			if (verbose) {
				printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
			}
			return 1;
		}
	}
	printf("Tried to create too many jobs\n");
	return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid)
{
	int i;

	if (pid < 1)
		return 0;

	for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == pid) {
			clearjob(&jobs[i]);
			nextjid = maxjid(jobs) + 1;
			return 1;
		}
	}
	return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
	int i;

	for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].state == FG)
			return jobs[i].pid;
	return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
	int i;

	if (pid < 1)
		return NULL;
	for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].pid == pid)
			return &jobs[i];
	return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid)
{
	int i;

	if (jid < 1)
		return NULL;
	for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].jid == jid)
			return &jobs[i];
	return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid)
{
	int i;

	if (pid < 1)
		return 0;
	for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].pid == pid) {
			return jobs[i].jid;
		}
	return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs)
{
	int i;

	for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid != 0) {
			printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
			switch (jobs[i].state) {
			case BG:
				printf("Running ");
				break;
			case FG:
				printf("Foreground ");
				break;
			case ST:
				printf("Stopped ");
				break;
			default:
				printf("listjobs: Internal error: job[%d].state=%d ",
					i, jobs[i].state);
			}
			printf("%s", jobs[i].cmdline);
		}
	}
}
/******************************
 * end job list helper routines
 ******************************/


 /***********************
  * Other helper routines
  ***********************/

  /*
   * usage - print a help message
   */
void usage(void)
{
	printf("Usage: shell [-hvp]\n");
	printf("   -h   print this message\n");
	printf("   -v   print additional diagnostic information\n");
	printf("   -p   do not emit a command prompt\n");
	exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
	fprintf(stdout, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
	fprintf(stdout, "%s\n", msg);
	exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask); /* block sigs of type being handled */
	action.sa_flags = SA_RESTART; /* restart syscalls if possible */

	if (sigaction(signum, &action, &old_action) < 0)
		unix_error("Signal error");
	return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig)
{
	printf("Terminating after receipt of SIGQUIT signal\n");
	exit(1);
}



