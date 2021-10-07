import Slider from '@mui/material/Slider';
import Container from '@mui/material/Container';
import Typography from '@mui/material/Typography';
import TextField from '@mui/material/TextField';

let marks=[
{value: 5,label: '5 min'},{value: 30, label: '30 max'}
];

let TimerForm = ({errorTextField,helperTextField,timerTitle,timerValue,
activated,handleChangeSlider,handleSubmit,
handleChangeTextField,helperText}) => !activated&&<Container>
		<div style = {{width : '280px'}}>
			<form onSubmit={handleSubmit}>
			<TextField
				error={errorTextField}
				helperText={helperTextField}
			//my는 margin y 부분을 말함
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
				min = {0}
				max = {30}
				onChange = {handleChangeSlider}/>
			</form>
			</div>
		</Container>

			//slider랑 typography 까지 묶는게 컨벤션- 인풋은 다 넣어준다
export default TimerForm;
