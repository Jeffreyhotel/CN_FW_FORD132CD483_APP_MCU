/******************************************************************************
 ;       Program		: C_Communication.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef C_COMMUNICATION_H
#define C_COMMUNICATION_H

#include "../Mymain.h"
#include "M_Communication.h"

/* Power Management State Machine Define */
/* The Max Number of State */
#define MAX_COM_STATE_NO					0x03U
#define STATE_COMMUNICATION_INIT			0x00U
#define STATE_COMMUNICATION_PROCESS			0x01U
#define STATE_COMMUNICATION_ERROR			0x02U

#define EVENT_MESSAGE_FLASH_WRITE_CONTROL	EVENT_MESSAGE
#define EVENT_MESSAGE_START_INTB_STRATEGY	EVENT_MESSAGE1
#define EVENT_MESSAGE_CANCEL_INTB_STRATEGY	EVENT_MESSAGE2

#define EVENT_TIMER_I2C_DES_INIT_DELAY		EVENT_TIMER1
#define EVENT_TIMER_INTB_ROUNTINE			EVENT_TIMER2

typedef struct
{
	uint16_t u16Timer1;
	uint16_t u16Timer2;
	uint32_t u32Timeout;
} tcommunication_task_def;

void C_Communication_Timer1(void);
void C_Communication_Timer2(void);
extern void (*const Communication_State_Machine[MAX_COM_STATE_NO])(void);

#endif
