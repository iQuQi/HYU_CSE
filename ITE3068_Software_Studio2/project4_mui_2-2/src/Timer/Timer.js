import {Typography} from '@mui/material';
//distructuring 을 통해서 아래와 같이 React.Component 대신 앞의 클래스를 생략해서 사용 가능


//이 방식이 functional component
//let Timer=()=>{
//	return <div><h1>25 : 00</h1></div>;}

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
