//distructuring 을 통해서 아래와 같이 React.Component 대신 앞의 클래스를 생략해서 사용 가능


//이 방식이 functional component
//let Timer=()=>{
//	return <div><h1>25 : 00</h1></div>;}

let Timer = ({remained}) => {
	console.log(remained); 
	return <div>
				<h1>{Math.floor(remained/60)} : {remained %60 < 10
				? '0' + remained % 60 : remained % 60}
				</h1>
			</div>;
}

export default Timer;
