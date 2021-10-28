import HourglassEmptyIcon from '@mui/icons-material/HourglassEmpty';

let TimerAnimation = ({initialized,activated,paused}) =>
	{
	return <HourglassEmptyIcon
		className = {initialized? "App-logo" : ""}
		//htmlColor = "red" -> #00FF00 처럼 rgb가능 
		sx={{fontSize: '100px',
		animationPlayState : (!paused && activated)? "running" : "paused"}}
		/>
	}

export default TimerAnimation;
