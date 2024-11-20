/******************************************************************************
 ;       Program		: M_BatteryProtect.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		:
 ;       Author		:
 ******************************************************************************/
#ifndef M_BATTERYPROTECT_H
#define M_BATTERYPROTECT_H

#include "public.h"

#define BP_VMIN_CFG				66 /*Define as 6.6V, unit: 0.1V*/
#define BP_VMINRCV_CFG			70 /*Define as 7.0V, unit: 0.1V*/
#define BP_VMAX_CFG				220 /*Define as 22V, unit: 0.1V*/
#define BP_VMAXRCV_CFG			180 /*Define as 18V, unit: 0.1V*/

#define BP_LOTIME_CFG			15U /*Unit: ms*/
#define BP_HITIME_CFG			15U /*Unit: ms*/
#define BP_LOOVRTIME_CFG		1000U /*Unit: ms*/
#define BP_HIOVRTIME_CFG		1000U /*Unit: ms*/
#define BP_LORECOVTIME_CFG		200U /*Unit: ms*/
#define BP_HIRECOVTIME_CFG		200U /*Unit: ms*/

typedef enum
{
	BP_STATE_INIT=0x00U,
	BP_STATE_NORM_V,
	BP_STATE_LO_V,
	BP_STATE_HI_V,
	BP_STATE_UNDER_V,
	BP_STATE_OVER_V,
}VBATT_STATE;

typedef enum
{
	BP_ACTION_NONE=0x00U,
	BP_ACTION_UNDERVOL,
	BP_ACTION_OVERVOL,
	BP_ACTION_RECOVER,
}BATTPROTECT_ACTION;

typedef struct
{
	VBATT_STATE u8VbattState;
	BATTPROTECT_ACTION u8ProtectAction;

	uint16_t u16LoEnterNormalTime;
	uint16_t u16HiEnterNormalTime;
	uint16_t u16LoEnterUnderTime;
	uint16_t u16HiEnterOverTime;
	uint16_t u16UnderEnterNormalTime;
	uint16_t u16OverEnterNormalTime;
} tbattery_protect_manage_def;

typedef void (*CALLBACK_BATT_ACTION_PROTECT)(BATTPROTECT_ACTION);
typedef void (*CALLBACK_BATT_ACTION_RECOVER)(void);

void M_BP_State_Maching(uint16_t u16RoundTime);
void M_BP_Callback_Register(CALLBACK_BATT_ACTION_PROTECT CBBattActionProtectReg, CALLBACK_BATT_ACTION_RECOVER CBBattActionRecoverReg);

#endif
