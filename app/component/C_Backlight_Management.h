
/******************************************************************************
;       Program		: C_Backlight_Management.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		: 2021 / 10 / 07
;       Author		:
******************************************************************************/
#ifndef C_BACKLIGHT_MANAGEMENT_H
#define C_BACKLIGHT_MANAGEMENT_H

#include "../Mymain.h"

// Backlight Management State Machine Define
// The Max Number of State
#define MAX_BL_STATE_NO             	0x03U
#define STATE_BACKLIGHT_MANAGE_INIT     0x00U
#define STATE_BACKLIGHT_MANAGE_CTRL     0x01U
#define STATE_BACKLIGHT_MANAGE_ERROR    0x02U

typedef struct
{
	uint16_t u16BLDeratingRate;
	uint16_t u16Timer1;
	uint16_t u16Timer2;
	uint32_t u32Timeout;
}tbacklight_manage_task_def;

void C_Backlight_Manage_Timer1(void);
void C_Backlight_Manage_Timer2(void);

extern void (*const Backlight_Manage_State_Machine[MAX_BL_STATE_NO])(void);


#endif
