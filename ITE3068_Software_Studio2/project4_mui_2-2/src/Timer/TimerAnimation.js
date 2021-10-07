import HourglassEmptyIcon from '@mui/icons-material/HourglassEmpty';

let TimerAnimation = ({initialized,activated,paused}) =>
	{
	return <HourglassEmptyIcon
		className = {initialized? "App-logo" : ""}
		htmlColor="blue"
		sx={{fontSize: 100,
		animationPlayState : (!paused && activated)? "running" : "paused"}}
		/>
	}

export default TimerAnimation;
