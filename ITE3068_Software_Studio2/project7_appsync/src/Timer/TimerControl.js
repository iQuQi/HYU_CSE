import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';

let TimerControl=(prop)=>{
	let {activated, paused,handleStopTimer, handleStartTimer,handlePauseTimer,
	handleResumeTimer }=prop;
	console.log(prop); //받아온 프로퍼티 확인

	let fxs = [handleStopTimer, handleStartTimer, handlePauseTimer, handleResumeTimer];
	let arr_disabled = [false,activated,paused || !activated, !paused || !activated];
	//스택이 수직 혹은 수평축을 기죽으로 직계 자식을 정렬하고
	//spacing과 divider도 옵션으로 추가 가능
	//Grid 라는 걸 사용할수도 있음 -> 반응형 웹을 쉽게 구현 가능. 12 column 그리드 레이아웃
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
