/******************************************************************************
 ;       Program		: C_Battery_Protect.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#ifndef C_BATTERY_PROTECT_H
#define C_BATTERY_PROTECT_H

#include "../Mymain.h"
#include "M_BatteryProtect.h"

#define MAX_BP_STATE_NO             	0x03U
#define STATE_BATTERY_PROTECT_INIT     	0x00U
#define STATE_BATTERY_PROTECT_CTRL     	0x01U
#define STATE_BATTERY_PROTECT_ERROR    	0x02U

typedef struct
{
	uint16_t u16Timer1;
	uint32_t u32Timeout;
} tbattery_protect_task_def;

void C_Battery_Protect_Timer1(void);
extern void (*const Battery_Protect_State_Machine[MAX_BP_STATE_NO])(void);

#endif
