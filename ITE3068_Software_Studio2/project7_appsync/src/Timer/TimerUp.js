import Button from '@mui/material/Button';
import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogTitle from '@mui/material/DialogTitle';
//open이 true가 되면 알림창이 뜸
//onClose 뒤 - 바깥 영역을 클릭했을 때 불리는 함수
//onClick 뒤 - close버튼을 눌렀을때 불리는 함수
		 
let TimerUp = ({open,handleClose}) => <div>
      <Dialog
        open={open}
        onClose={handleClose}
        aria-labelledby="alert-dialog-title"
        aria-describedby="alert-dialog-description"
      >
        <DialogTitle id="alert-dialog-title">
        	Timer is up!!
		</DialogTitle>
        <DialogActions sx={{justifyContent : 'center'}}>
          <Button onClick={handleClose}>Close</Button> 
        </DialogActions>
      </Dialog>
    </div>



export default TimerUp;
