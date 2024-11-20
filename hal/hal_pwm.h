/******************************************************************************
;       Program		: HAL_PWM.h
;       Function	: Declare tcpwm hal & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once
#include <stdint.h>
/* -- Includes -- */
#ifndef HAL_PWM_H
#define HAL_PWM_H

/* -- Marco Define -- */
#define		RSLT_FAIL			0x01U

/* -- Data Type Define -- */

/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern uint32_t HAL_PWM_CH4_Init(void);
extern void HAL_PWM_CH4_DeInit(void);
extern void HAL_PWM_Duty_Output_Adjust(uint32_t u32Channel, uint32_t u32DutyAdjust);
#endif /* HAL_PWM_H_ */
