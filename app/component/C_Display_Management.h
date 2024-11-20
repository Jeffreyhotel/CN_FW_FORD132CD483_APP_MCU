/******************************************************************************
 ;       Program		: C_Power_Management.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef C_DISPLAY_MANAGEMENT_H
#define C_DISPLAY_MANAGEMENT_H

#include "../Mymain.h"
#include "M_DisplayManage.h"

// Power Management State Machine Define
// The Max Number of State
#define MAX_DM_STATE_NO               	0x03U
#define STATE_DISPLAY_MANAGE_INIT     	0x00U
#define STATE_DISPLAY_MANAGE_CTRL     	0x01U
#define STATE_DISPLAY_MANAGE_ERROR    	0x02U

#define EVENT_MESSAGE_DISPLAY_ENABLE    EVENT_MESSAGE
#define EVENT_MESSAGE_SCANNING          EVENT_MESSAGE1
#define EVENT_MESSAGE_TDDI_VCOM   		EVENT_MESSAGE2
#define EVENT_I2C_MCU_INITIAL_DELAY     EVENT_TIMER1
#define EVENT_DISPLAY_CONTORL_DELAY     EVENT_TIMER1
#define EVENT_DISPLAY_INITIAL_DELAY     EVENT_TIMER2
#define EVENT_TIME_ATTN_POLLING			EVENT_TIMER2
#define EVENT_TIME_INIT_POLLING			EVENT_TIMER3

typedef struct
{
	uint16_t u16Timer1;
	uint16_t u16Timer2;
	uint16_t u16Timer3;
	uint32_t u32Timeout;
} tdisplay_manage_task_def;

typedef enum
{
	DS_ACTION_NONE = 0x00U,
	DS_ACTION_DISPLAY_CTRL,
	DS_ACTION_DISPLAY_STATUS,
	DS_ACTION_DISPLAY_RESET,
	DS_ACTION_TOUCH_CTRL,
	DS_ACTION_TOUCH_STATUS,
	DS_ACTION_BACKLIGHT,
	DS_ACTION_BACKLIGHT_PWM,
	DS_ACTION_CTRL_PROTECT,	
}DISPLAY_SEQ_ACTION;

typedef struct
{
	uint8_t u8DisplayEnLock;
	bool bPowerStartupEvent;
	uint8_t u8LastDisplayStatus;
	uint8_t u8DispSeqStatus;
	uint8_t u8CurrentDisplaySet;
	bool bBacklightSet;
	bool bBacklightSetBackup;
	bool bDiagnosisProtect;
	bool bDiagnosisProtectLeve;
}tdisplay_ctrl_def;



void C_Display_Manage_Timer1(void);
void C_Display_Manage_Timer2(void);
void C_Display_Manage_Timer3(void);

extern void (*const Display_Manage_State_Machine[MAX_DM_STATE_NO])(void);

#endif
