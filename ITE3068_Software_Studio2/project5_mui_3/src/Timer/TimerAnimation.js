import HourglassEmptyIcon from '@mui/icons-material/HourglassEmpty';

let TimerAnimation = ({initialized,activated,paused}) =>
	{
	return <HourglassEmptyIcon
		className = {initialized? "App-logo" : ""}
		sx={{fontSize: '100px', 
		animationPlayState : (!paused && activated)? "running" : "paused"}}
		/>
	}

export default TimerAnimation;
