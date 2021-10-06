import {Component} from 'react';
import TimerForm from './Form'
import Timer from './Timer';
import TimerUp from './TimerUp'
import TimerControl from './TimerControl';
import TimerAnimation from './TimerAnimation';

import Card from '@mui/material/Card';
import CardContent from '@mui/material/CardContent';
import CardMedia from '@mui/material/CardMedia';
import { CardActionArea } from '@mui/material';


let time = 1500;
class TimerPannel extends Component{
	constructor(){
		super();
		this.state = {
		initialized : false,
		remained: time,
		activated: false,
		paused: false,
		
		errorTextField:false,
		helperTextField: "",

		timerValue : time,
		timerTitle: "",
		helperText: ""
		};
	}

	handleStartTimer = () => {
		let {timerValue} = this.state;
		this.setState({
			remained: timerValue
		})
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
		let {timerValue} = this.state;
		//Interval 활성화를 끈다
		clearInterval(this.interval);
		this.setState(()=>{
			//남은 시간을 초기화 한다
			return	{
			initialized: false,
			activated: false,
			remained  : timerValue
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

	handleChangeSlider = e => {
		if (Number(e.target.value) < 5){
			this.setState({
			helperText: "error",
			timerValue: 5 * 60})
		}
		else{
			this.setState({
				helperText:"",
				timerValue: Number(e.target.value)*60
			})
		}
	}

	handleChangeTextField = e => {
		if(e.target.value && e.target.value.length>25){
			this.setState({errorTextField: true,
			helperTextField: "Title cannat exceeds 25 char",
			timerTitle: e.target.value})	;
		}else{
			this.setState({errorTextField: false,
			helperTextField: "" ,
			timerTitle: e.target.value});
		}
	}

	handleSubmit = e =>{
		e.preventDefault();
		console.log(e.target);

		let {timerTitle,timerValue} =this.state;

		if(timerTitle ==""){
			this.setState(
			{
				errorTextField: true,
				helperTextField: "Empty Title"
			}
			);
		}else{
		//리프레시를 막았기때문에 폼 초기화를 해줘야 함
			this.setState({
				errorTextField: false,
				helperTextField: "",

				timerTitle: "",
				timerValue,
				remained: timerValue

			});
			this.handleStartTimer();
		}

	}

	render() {
		let {errorTextField, helperTextField,timerTitle,timerValue,initialized,remained, activated, paused,helperText} =this.state;
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
			<TimerForm
				errorTextField = {errorTextField}
				helperTextField= {helperTextField}
				timerTitle={timerTitle}
				timerValue = {timerValue}
				helperText = {helperText}
				activated ={activated}
				handleChangeSlider={this.handleChangeSlider}
				handleChangeTextField ={this.handleChangeTextField}
				handleSubmit={this.handleSubmit}
				/>
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
			/>i
			</CardContent>
			</CardActionArea>
			</Card>);
	}
}

export default TimerPannel;
