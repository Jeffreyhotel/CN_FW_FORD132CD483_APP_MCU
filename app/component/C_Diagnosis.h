
/******************************************************************************
;       Program		: C_Power_Management.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		: 2021 / 10 / 07
;       Author		:
******************************************************************************/
#pragma once

#ifndef C_DIAGNOSIS_H
#define C_DIAGNOSIS_H

#include "../Mymain.h"
#include "M_GPIOSense.h"
#include "public.h"

// Power Management State Machine Define
// The Max Number of State
#define MAX_DN_STATE_NO             	0x03U
#define STATE_DIAGNOSIS_INIT     		0x00U
#define STATE_DIAGNOSIS_CTRL     		0x01U
#define STATE_DIAGNOSIS_ERROR    		0x02U

#define INT_ERROR						0x01U
#define INT_TOUCH						0x04U

#define DIAG_FPC_TX_DISCON_VOL			2640 /*Unit: 1 mv, FPC normal voltage */ // ADC Check
#define DIAG_FPC_RX_DISCON_VOL			2640 /*Unit: 1 mv, FPC normal voltage */
#define DIAG_FPC_BL_DISCON_VOL			3180U /*Unit: 1 mv, FPC disconnect voltage */

#define DEBOUNCE_3_TIMES			  		3U
#define DEBOUNCE_4_TIMES			  		4U

#define C_DIAG_LP8864_I2CTIME				250U /*Unit: 1ms*/
#define C_DIAG_NT51926_I2CTIME				250U /*Unit: 1ms*/
#define C_DIAG_LP8864_REG_DEBUNCE			4U 
#define C_DIAG_NT51926_REG_DEBUNCE			4U
#define C_DIAG_NT51926_Comm_DEBUNCE			4U
#define C_DIAG_NT51926_STATE_DEBUNCE		4U


#define EVENT_MESSAGE_DISANOSIS_ENABLE		EVENT_MESSAGE

#if(BACKDOOR_ICDIAG_OPEN)
#define EVENT_MESSAGE_ICDIAG				EVENT_TIMER1
#endif
#define EVENT_TIME_INITIAL_DELAY			EVENT_TIMER1
#define EVENT_TIME_DIAGNOSIS_POLLING		EVENT_TIMER2

typedef void (*CALLBACK_DIAG_ACTION_PROTECT)(void);
typedef void (*CALLBACK_DIAG_ACTION_RECOVER)(void);

typedef struct
{
	uint16_t u16Timer1;
	uint16_t u16Timer2;
	uint32_t u32Timeout;
}tdiagnosis_task_def;

typedef enum
{
	DIAG_ACTION_NONE=0x00U,
	DIAG_ACTION_SHUTDOWN,
	DIAG_ACTION_DISPBL_OFF_RSTRQ,
	DIAG_ACTION_DISPBL_OFF_RSTRQ_RECOV,
	DIAG_ACTION_DISPBL_OFF_NORSTRQ_NORECOV,
	DIAG_ACTION_DISPBL_OFF_NORSTRQ_RECOV,
 }DIAGPROTECT_ACTION;

typedef struct
{
	DIAGPROTECT_ACTION DiagProtectAction;
	uint16_t u16LEDDriverCommTime;
	uint8_t  u8LEDDriverRegDebunce;
	uint8_t  u8LEDDriverRegDebunce_RECOV;	
	uint16_t u16NT51926CommTime;
	uint8_t  u8NT51926DpRegDebunce;
	uint8_t  u8NT51926DpRegDebunce_RECOV;	
	uint8_t  u8NT51926TypeCRegDebunce;
	uint8_t  u8NT51926TypeCRegDebunce_RECOV;	
	uint8_t  u8NT51926TpRegDebunce;	
	uint8_t  u8NT51926TpRegDebunce_RECOV;

	uint16_t u16NT51926I2cCommTime;
	uint8_t  u8NT51926I2cDebunce;
	uint8_t  u8NT51926DpStatusOnDebunce;
	uint8_t  u8NT51926DpStatusOnDebunce_RECOV;
	uint8_t  u8NT51926DpStatusOfDebunce;
	uint8_t  u8NT51926DpStatusOfDebunce_RECOV;

	uint8_t  u8NT51926BLDDebunce;	
	uint8_t  u8NT51926BLDDebunce_RECOV;

	bool bFirstLedIntDebounceFinsh;	
}tdiagnosis_ctrl_def;

extern void C_Diagnosis_Timer1(void);
extern void C_Diagnosis_Timer2(void);
extern void (*const Diagnosis_State_Machine[MAX_DN_STATE_NO])(void);
extern void C_Diagnosis_Callback_Register(CALLBACK_DIAG_ACTION_PROTECT CBDiagActionProtectReg, CALLBACK_DIAG_ACTION_RECOVER CBDiagActionRecoverReg);


#endif
