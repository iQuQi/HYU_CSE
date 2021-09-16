let TimerControl=(prop)=>{
	let {activated, paused,handleStopTimer, handleStartTimer,handlePauseTimer,
	handleResumeTimer }=prop;
	console.log(prop); //받아온 프로퍼티 확인

//태그 내에 주석은 아래와 같이 처리함을 유의
	return <div>
			{/*스탑 버튼- 시작 버튼/재시작 버튼/일시정지 버튼(조건에 따라 변경)*/}
			<button onClick = {handleStopTimer}>Stop</button>
			{	
				/*활성화가 되지 않은 상황이면 시작 버튼이 표시됨*/
				!activated ? 
					<button onClick={handleStartTimer}>Start</button> :
					/*활성화가 된 상태*/
					!paused ?
						/*일시 정지가 되지 않은 상태라면 일시정지 버튼이 노출*/
						<button onClick={handlePauseTimer}>Pasue</button> :
						/*일시 정지가 되어 있는 상태라면 재시작 버튼이 노출*/
						<button onClick={handleResumeTimer}>Resume</button> 
			}			
			</div>;
}

export default TimerControl;
