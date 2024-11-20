#include "hal_uart.h"
#include "SW_Timer.h"


static tdTimerInterface gtdSWTimer[SWTIMER_BLOCK];

static int32_t SWTimer_INTCallback(void);
uint32_t SWTimer_Create(void)
{
	uint8_t u8Loop;

	for( u8Loop = 0U; u8Loop < SWTIMER_BLOCK; u8Loop++)
	{
		gtdSWTimer[u8Loop].u32Status = SWTIMER_IDLE;
		gtdSWTimer[u8Loop].u32Period = 0U;
		gtdSWTimer[u8Loop].pCallback = NULL;
		gtdSWTimer[u8Loop].pINTCallback = NULL;
	}	

	return true;
}

uint32_t SWTimer_Counting(uint32_t u32Dummy)
{
	uint8_t u8Loop;

	for( u8Loop = 0U; u8Loop < SWTIMER_BLOCK; u8Loop++)
	{
		if( SWTIMER_IDLE & gtdSWTimer[u8Loop].u32Status )
		{
			;/* Timer IDLE do nothing */
		}
		else
		{
			if( SWTIMER_PROGRESS & gtdSWTimer[u8Loop].u32Status )
			{
				gtdSWTimer[u8Loop].u32Period--;
				if( 0U == gtdSWTimer[u8Loop].u32Period )
				{
					gtdSWTimer[u8Loop].u32Status &= ~SWTIMER_PROGRESS;
					gtdSWTimer[u8Loop].u32Status |= SWTIMER_TIMEOUT;
				}
			}
			else
			{
				;/* Nothing */
			}
	
			if( SWTIMER_TIMEOUT & gtdSWTimer[u8Loop].u32Status )
			{
				gtdSWTimer[u8Loop].pCallback(NULL);
				gtdSWTimer[u8Loop].u32Status&= ~SWTIMER_TIMEOUT;
				gtdSWTimer[u8Loop].u32Status |= SWTIMER_IDLE;
			}
			else
			{
				;/* Nothing */
			}
		}
	}	
	return true;
}
uint32_t SWTimer_Start(tdSWTimerConfig * tdTimerConfig)
{
	uint8_t u8Loop = 0U;
	uint32_t u32ReturnId = 0U;

	for( u8Loop = 0U; u8Loop < SWTIMER_BLOCK; u8Loop++)
	{
		if( SWTIMER_IDLE & gtdSWTimer[u8Loop].u32Status )
		{
			/* IDLE for free */
			/* Check input period */
			if( tdTimerConfig->u32Period % 10U )
			{
				/* not 10ms based, input invalid */
				u32ReturnId = SWTIMER_INVALID;
				HAL_UART_Printf("not 10ms based:%d\r\n",gtdSWTimer[u8Loop].u32Period);
			}
			else
			{
				u32ReturnId = (uint32_t)u8Loop;
				gtdSWTimer[u8Loop].u32Period = tdTimerConfig->u32Period;
				gtdSWTimer[u8Loop].pCallback = tdTimerConfig->pCallback;
				if( NULL == tdTimerConfig->pINTCallback )
					gtdSWTimer[u8Loop].pINTCallback = &SWTimer_INTCallback;
				else
					gtdSWTimer[u8Loop].pINTCallback = tdTimerConfig->pINTCallback;
				gtdSWTimer[u8Loop].u32Status&= ~SWTIMER_IDLE;
				gtdSWTimer[u8Loop].u32Status |= SWTIMER_PROGRESS;
			}
			break;
		}
		else
		{
			/* Timer busy, go next */
			u32ReturnId = SWTIMER_INVALID;
		}
	}
	return u32ReturnId;
}

uint32_t SWTimer_Stop(uint32_t u32Id)
{
	uint32_t u32RtCode = true;
	
	if( SWTIMER_INVALID <= u32Id)
	{
	    HAL_UART_Printf("Invalid sw timer:%d\r\n", u32Id);
		u32RtCode = false;
	}
	else
	{
		if( SWTIMER_PROGRESS & gtdSWTimer[u32Id].u32Status)
		{
			/* timer progressing */
			gtdSWTimer[u32Id].u32Status&= ~SWTIMER_PROGRESS;
			gtdSWTimer[u32Id].u32Status |= SWTIMER_IDLE;
			
			//if( NULL gtdSWTimer[u32Id].pINTCallback )
				gtdSWTimer[u32Id].pINTCallback();
			//else
			//	printf("No sw timer INT callback\r\n");
		}
		else
		{
			;/* Do nothing */	
		}
	}
	return u32RtCode;
}

uint32_t SWTimer_Status(uint32_t u32Id)
{
	return gtdSWTimer[u32Id].u32Status;
}

static int32_t SWTimer_INTCallback(void)
{
    HAL_UART_Printf("SW timer INT stop callback\r\n");
	return true;
}
