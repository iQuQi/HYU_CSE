import {Component} from 'react';
import Timer from './Timer';
import TimerControl from './TimerControl';

class TimerPannel extends Component{
	constructor(){
		super();
		this.state = {
		remained: 1500,
		activated: false,
		paused: false
		};
	}

	handleStartTimer = () => {
		this.interval= setInterval(
		()=>{
			this.setState((prev) => {
				return { 
					activated: true,
					remained: prev.remained-1
				};
			});//여기까지 setState()
		},1000);//여기까지 setInterval()
		//1초(1000)마다 이 콜백함수를 호출하겠다는 뜻
	}

	handleResumeTimer = () => {
		this.interval= setInterval(
		()=>{
			this.setState((prev) => {
				return { 
					//일시 정지 상태를 풀어준다
					paused: false,
					remained: prev.remained-1
				};
			});
		},1000);
 	}

	handleStopTimer = () => {
		//Interval 활성화를 끈다
		clearInterval(this.interval);
		this.setState(()=>{
			//남은 시간을 초기화 한다
			return	{
			activated: false,
			remained :1500
			};
		});
	}

	handlePauseTimer = () => {
		clearInterval(this.interval);
		this.setState(()=>{
			//일시 정지 상태로 만든다
			return{ paused:true};
		});
	}

	render() {
		let {remained, activated, paused} =this.state;
		return (<div>
			<Timer remained={remained} />
			<TimerControl
				activated = {activated}
				paused = {paused}
				handleStopTimer = {this.handleStopTimer}
				handleStartTimer = {this.handleStartTimer}
				handlePauseTimer = {this.handlePauseTimer}
				handleResumeTimer = {this.handleResumeTimer}
				/>
			</div>);
	}
}

export default TimerPannel;
