import {Component} from 'react';
import Timer from './Timer';
import TimerUp from './TimerUp'
import TimerControl from './TimerControlV4';
import TimerAnimation from './TimerAnimation';
import Card from '@mui/material/Card';
import CardContent from '@mui/material/CardContent';
import CardMedia from '@mui/material/CardMedia';
import { CardActionArea } from '@mui/material';
let timerVal = 5;
class TimerPannel extends Component{
	constructor(){
		super();
		this.state = {
		initialized : false,
		remained: timerVal,
		activated: false,
		paused: false
		};
	}

	handleStartTimer = () => {
		this.interval= setInterval(
		()=>{
			this.setState((prev) => {
				if(prev.remained <= 0){
					clearInterval(this.interval);
					return{activated: false,initialized : false};
				}
				else{
				return { 
					initialized : true,
					activated: true,
					remained: prev.remained-1
				};
				}
			});//여기까지 setState()
		},1000);//여기까지 setInterval()
		//1초(1000)마다 이 콜백함수를 호출하겠다는 뜻
	}

	handleResumeTimer = () => {
		this.interval= setInterval(
		()=>{
			this.setState((prev) => {
				if(prev.remained <= 0){
					clearInterval(this.interval);
					return{activated: false, initialized : false};
				}else{
				return { 
					//일시 정지 상태를 풀어준다
					paused: false,
					remained: prev.remained-1
				};
				}
			});
		},1000);
 	}

	handleStopTimer = () => {
		//Interval 활성화를 끈다
		clearInterval(this.interval);
		this.setState(()=>{
			//남은 시간을 초기화 한다
			return	{
			initialized: false,
			activated: false,
			remained :timerVal
			};
		});
	}

	handlePauseTimer = () => {
		clearInterval(this.interval);
		this.setState(()=>{
			//일시 정지 상태로 만든다
			return{ 
			paused:true};
		});
	}

	render() {
		let {initialized,remained, activated, paused} =this.state;
		return (<Card sx={{maxWidth : 360}}>
			<CardActionArea>
			<CardMedia component="img" height = "140" 
			image="https://mui.com/static/images/cards/contemplative-reptile.jpg"
          	alt="green iguana" />
			<CardContent>
			<TimerAnimation 
				initialized={initialized}
				activated = {activated}
				paused ={paused} />
			<Timer remained={remained} />
			<TimerControl
				activated = {activated}
				paused = {paused}
				handleStopTimer = {this.handleStopTimer}
				handleStartTimer = {this.handleStartTimer}
				handlePauseTimer = {this.handlePauseTimer}
				handleResumeTimer = {this.handleResumeTimer}
				/>
			<TimerUp 
				handleClose = {this.handleStopTimer}
				open = {remained <= 0}
			/>
			</CardContent>
			</CardActionArea>
			</Card>);
	}
}

export default TimerPannel;
