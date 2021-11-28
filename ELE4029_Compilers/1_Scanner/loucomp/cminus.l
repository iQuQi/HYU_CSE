/****************************************************/
/* File: cminus.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"while"          {return WHILE;}
"return"          {return RETURN;}
"int"          {return INT;}
"void"          {return VOID;}

"="            {
                  char c=input();
                  if(c=='=') return EQ;
                  else {
                    unput(c);
                    return ASSIGN;
                  }
                }
"!"             {
                  char c=input();
                  if(c=='=') return NE;
                  else {
                    unput(c);
                    return ERROR;
                  }
                }

"<"             {
                  char c=input();
                  if(c=='=') return LE;
                  else {
                    unput(c);
                    return LT;
                  };
                }
">"             {
                  char c=input();
                  if(c=='=') return GE;
                  else {
                    unput(c);
                    return GT;
                  };
                }

"["             {return LBRACE;}
"]"             {return RBRACE;}
"{"             {return LCURLY;}
"}"             {return RCURLY;}
"("             {return LPAREN;}
")"             {return RPAREN;}

"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {
                  char c ;
                  int done = 0,star;
                  
                  c = input();
                  if (c!='*') {
                    unput(c);
                    return OVER;
                  }
                  
                  while(!done){
              
                      c= input();
                      if(star){
                        if(c=='/') done =1 ;
                        else star =0;
                      } 

                      if (c == EOF) break;
                      if (c == '\n') lineno++;
                      if (c == '*') {
                        star = 1;
                      }
                  } 

                }

";"             {return SEMI;}
","             {return COMMA;}

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}

.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

