
/******************************************************************************
;       Program		: C_Data_Collecting.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		: 2021 / 10 / 07
;       Author		:
******************************************************************************/
#ifndef C_DATA_COLLECTING_H
#define C_DATA_COLLECTING_H

#include "../Mymain.h"
#include "M_DataCollecting.h"

/* DATA COLLECTING State Machine Define */
/* The Max Number of State */
#define MAX_DC_STATE_NO             	0x03U
#define STATE_DATA_COLLECTING_INIT     	0x00U
#define STATE_DATA_COLLECTING_CTRL     	0x01U
#define STATE_DATA_COLLECTING_ERROR    	0x02U

typedef struct
{
	uint16_t u16Timer1;
	uint16_t u16Timer2;
	uint32_t u32Timeout;
}tdata_collecting_task_def;

void C_Data_Collecting_Timer1(void);
extern void (*const Data_Collecting_State_Machine[MAX_DC_STATE_NO])(void);

#endif
