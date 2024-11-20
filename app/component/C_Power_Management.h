/******************************************************************************
 ;       Program		: C_Power_Management.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef C_POWER_MANAGEMENT_H
#define C_POWER_MANAGEMENT_H

#include "../Mymain.h"
#include "M_PowerManagement.h"

// Power Management State Machine Define
// The Max Number of State
#define MAX_PM_STATE_NO             0x03U
#define STATE_POWER_MANAGE_INIT     0x00U
#define STATE_POWER_MANAGE_CTRL     0x01U
#define STATE_POWER_MANAGE_ERROR    0x02U

typedef struct
{
	uint16_t u16Timer1;
	uint32_t u32Timeout;
} tpower_manage_task_def;

#define OFF_POWER_STATE				0x00U
#define SHUTDOWN1OR2_STATE			0x01U
#define START_UP_STATE				0x02U
#define NORMAL_RUN_STATE			0x03U
#define NORMAL_DISABLE_STATE		0x04U

#define EVENT_SYNCS_CHECK_PERIOD	EVENT_TIMER1

#define SYNC_HIGH_VOL				70U /*Analog value as 7V*/
#define SYNC_LOW_VOL				22U /*Analog value as 2.2V*/
#define SYNC_DISABLE				0x00U
#define SYNC_ENABLE					0x01U
#define SYNC_UNKNOW					0x02U
#define SYNC_HIGH_DEBOUNCE_MAX		3U
#define SYNC_LOW_DEBOUNCE_MAX		3U

void C_Power_Manage_Timer1(void);
void C_Power_Manage_Timer2(void);
extern void (*const Power_Manage_State_Machine[MAX_PM_STATE_NO])(void);

#endif
