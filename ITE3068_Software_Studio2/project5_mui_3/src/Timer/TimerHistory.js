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
//map 으로 한경우 key  프로퍼티가 필요하데
let TimerHistory = ({history}) => 
	<Timeline>
		{history && history.map(item => 
      <TimelineItem key={'task_'+item.id}>
        <TimelineOppositeContent sx={{py: '24px'}} >
          {item.duration}
        </TimelineOppositeContent>
        <TimelineSeparator>
          <TimelineConnector />
          <TimelineDot>
            <FastfoodIcon />
          </TimelineDot>
          <TimelineConnector />
        </TimelineSeparator>
        <TimelineContent sx={{ py: '20px', px: 2 }}>
          <Typography variant="h6" component="span">
            {item.title}
          </Typography>
        </TimelineContent>
      </TimelineItem>
	 )}
    </Timeline>


export default TimerHistory
