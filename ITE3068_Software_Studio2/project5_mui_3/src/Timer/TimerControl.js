import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';

let TimerControl=(prop)=>{
	let {activated, paused,handleStopTimer, handleStartTimer,handlePauseTimer,
	handleResumeTimer }=prop;
	console.log(prop); //받아온 프로퍼티 확인

	let fxs = [handleStopTimer, handleStartTimer, handlePauseTimer, handleResumeTimer];
	let arr_disabled = [false,activated,paused || !activated, !paused || !activated];

//태그 내에 주석은 아래와 같이 처리함을 유의
	return <Stack spacing = {1} direction = "row">
			{['Stop', 'Start','Pause', 'Resume' ].map((item,index)=>
			
			<Button  
			key = {index}
			onClick = {fxs[index]} 
			disabled={arr_disabled[index]}
			variant = "contained"> 
			{item}
			</Button>
			
			)}
			</Stack>
}

export default TimerControl;
