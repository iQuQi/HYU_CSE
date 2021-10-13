import * as React from 'react';
import Timeline from '@mui/lab/Timeline';
import TimelineItem from '@mui/lab/TimelineItem';
import TimelineSeparator from '@mui/lab/TimelineSeparator';
import TimelineConnector from '@mui/lab/TimelineConnector';
import TimelineContent from '@mui/lab/TimelineContent';
import TimelineOppositeContent from '@mui/lab/TimelineOppositeContent';
import TimelineDot from '@mui/lab/TimelineDot';
import FastfoodIcon from '@mui/icons-material/Fastfood';
import Typography from '@mui/material/Typography';
import TimerAnimation from './TimerAnimation'
//map 으로 한경우 key  프로퍼티가 필요하데
let TimerHistory = ({currentTask,activated,history,initialized,paused}) => 
	<Timeline position = "right" sx={{mt: 0 , mb: 0}}>
		{history && history.map((item,index) =>
	currentTask && currentTask.id == item.id ?
      <TimelineItem key={'task_'+item.id}>
	  	<TimelineOppositeContent/>
        <TimelineSeparator>
          <TimelineDot color="error">
          	<TimerAnimation 
				initialized = {initialized}
				activated = {activated}
				paused = {paused}
				/>
		  </TimelineDot>
        </TimelineSeparator>
		<TimelineContent/>
      </TimelineItem>
	  :
	<TimelineItem key={'task_'+item.id}>
        <TimelineOppositeContent sx={{py: '24px'}} >
          {item.duration/60 < 10 ? "0":""}{Math.floor(item.duration/60)} 
		  : {item.duration%60 < 10? '0' : ""}{item.duration%60}
        </TimelineOppositeContent>
        <TimelineSeparator>
          <TimelineDot>
            <FastfoodIcon />
          </TimelineDot>
          <TimelineConnector />
          <TimelineConnector />
        </TimelineSeparator>
        <TimelineContent sx={{ py: '20px', px: 2 }}>
          <Typography 
		  	variant="h6" 
			component="span"
			color="text.secondary"
			>
            {item.title}
          </Typography>
        </TimelineContent>
      </TimelineItem>
	 )}
    </Timeline>


export default TimerHistory
