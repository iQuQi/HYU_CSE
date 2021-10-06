import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';
import ButtonGroup from '@mui/material/ButtonGroup';

let TimerControl=(prop)=>{
	let {activated, paused,handleStopTimer, handleStartTimer,handlePauseTimer,
	handleResumeTimer }=prop;
	console.log(prop); //받아온 프로퍼티 확인

	let fxs = [handleStopTimer, handleStartTimer, handlePauseTimer, handleResumeTimer];
	let arr_disabled = [false,activated,paused || !activated, !paused || !activated];
/*
	return <Stack spacing = {1} direction = "column">
			스탑 버튼- 시작 버튼/재시작 버튼/일시정지 버튼(조건에 따라 변경)
			<Button variant = "contained" onClick = {handleStopTimer}>Stop</Button>
			{	
				활성화가 되지 않은 상황이면 시작 버튼이 표시됨
				!activated ? 
					<Button variant = "contained" onClick={handleStartTimer}>Start</Button> :
					활성화가 된 상태
					!paused ?
						일시 정지가 되지 않은 상태라면 일시정지 버튼이 노출
						<Button variant = "contained" onClick={handlePauseTimer}>Pasue</Button> :
						일시 정지가 되어 있는 상태라면 재시작 버튼이 노출
						<Button variant = "contained" onClick={handleResumeTimer}>Resume</Button> 
			}			
			</Stack>;
*/
//태그 내에 주석은 아래와 같이 처리함을 유의
	return <ButtonGroup variant = "contained"
		aria-label ="outline primary button group">
			{['Stop', 'Start','Pause', 'Resume' ].map((item,index)=>
			
			<Button  
			key = {index}
			onClick = {fxs[index]} 
			disabled={arr_disabled[index]}
			variant = "contained"> 
			{item}
			</Button>
			
			)}
			</ButtonGroup>
}

export default TimerControl;
