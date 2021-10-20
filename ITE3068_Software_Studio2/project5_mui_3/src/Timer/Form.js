import Slider from '@mui/material/Slider';
import Container from '@mui/material/Container';
import Typography from '@mui/material/Typography';
import TextField from '@mui/material/TextField';

let marks=[
{value: 5,label: '5 min'},{value: 30, label: '30 max'}
];

//컨테이너로 감싸면 중앙정렬
//my는 margin y 부분을 말함
				//marks 라고만 적으면 step에 맞게 점 찍어줌
				//step이 널이면 mark된 곳만 이동 가능
let TimerForm = ({errorTextField,helperTextField,timerTitle,timerValue,
activated,handleChangeSlider,handleSubmit,
handleChangeTextField,helperText}) => !activated&&<Container>
		<div style = {{width : '280px'}}>
			<form onSubmit={handleSubmit}>
			<TextField
				error={errorTextField}
				helperText={helperTextField}
				sx={{width:'100%', my : '10px'}}
				value = {timerTitle}
				id="timer-title"
				label={errorTextField? "Error" : "Required"}
				onChange = {handleChangeTextField}
			/>
			<Typography color="error">{helperText}</Typography>
			<Slider
				disabled = {activated}
				id = "slider"
				value = {timerValue/60}
				valuelabelDisplay="auto"
				marks={marks}
				step = {5}
				min ={0}

				max = {40}
				onChange = {handleChangeSlider}/>
			</form>
			</div>
		</Container>

			//slider랑 typography 까지 묶는게 컨벤션- 인풋은 다 넣어준다
export default TimerForm;
