import {Typography} from '@mui/material';
let Timer = ({remained}) => {
	return <div>
				<Typography
				variant='h2'
				sx={{fontWeight : 700 }}
				>
				{Math.floor(remained/60) < 10 ? 
				'0' + Math.floor(remained/60) : Math.floor(remained/60)} : {remained %60 < 10 ?
				'0' + remained % 60 : remained % 60}
				</Typography>
			</div>;
}

export default Timer;
