#pragma once
#ifndef _SW_TIMER_H_
#define _SW_TIMER_H_

#include <stdint.h>

/* configuration */
#define SWTIMER_BLOCK		10U
#define SWTIMER_INVALID		SWTIMER_BLOCK + 1U

#define SWTIMER_IDLE		0x01U
#define SWTIMER_PROGRESS	0x02U
#define SWTIMER_TIMEOUT		0x04U


typedef struct{
	bool bWithPara;
	uint32_t u32Id;
	uint32_t u32Period;
	uint32_t u32Para;
	int32_t (* pCallback)(const uint32_t *);
	int32_t (* pINTCallback)(void);
}tdSWTimerConfig;


typedef struct{
	/* public variables */	
	uint32_t u32Status;
	uint32_t u32Period;
	int32_t (* pCallback)(const uint32_t *);
	int32_t (* pINTCallback)(void);
}tdTimerInterface;

uint32_t SWTimer_Create(void);
uint32_t SWTimer_Counting(uint32_t u32Dummy);
uint32_t SWTimer_Start(tdSWTimerConfig * tdTimerConfig);
uint32_t SWTimer_Stop(uint32_t u32Id);
uint32_t SWTimer_Status(uint32_t u32Id);
#endif
