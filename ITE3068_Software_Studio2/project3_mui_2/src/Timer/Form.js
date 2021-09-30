import Slider from '@mui/material/Slider';
import Container from '@mui/material/Container';


let TimerForm = ({activated,handleChangeSlider}) => <Container>
		<div style = {{width : '280px'}}>
			<Slider
				disabled = {activated}
				id = "slider"
				defaultValue = {25}
				step = {5}
				min = {5}
				max = {30}
				onChange = {handleChangeSlider}/>
			</div>
		</Container>

export default TimerForm;
