#include "C_Display_Management.h"
#include <stdlib.h>
#include "M_BatteryProtect.h"
#include "Memory_Pool.h"

static tbattery_protect_manage_def tBPManagement;
CALLBACK_BATT_ACTION_PROTECT CallbackBattActionProtect=NULL;
CALLBACK_BATT_ACTION_RECOVER CallbackBattActionRecover=NULL;

/******************************************************************************
 ;       Function Name			:	
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void M_BP_State_Maching(uint16_t u16RoundTime)
{
	uint16_t u16BatteryVol=Memory_Pool_BatteryVol_Get();
	uint16_t u16Temp = 0U;

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	u16BatteryVol = Memory_Pool_DiagnosisSimulateInfo_Get().u16BatteryVol;
#endif
	switch(tBPManagement.u8VbattState)
	{
		case BP_STATE_INIT:
			tBPManagement.u8VbattState=BP_STATE_NORM_V;
			tBPManagement.u8ProtectAction=BP_ACTION_NONE;
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;
		break;

		case BP_STATE_NORM_V:
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;

			if(u16BatteryVol < BP_VMIN_CFG)
			{
				tBPManagement.u8VbattState=BP_STATE_LO_V;
			}
			else if(u16BatteryVol > BP_VMAX_CFG)
			{
				tBPManagement.u8VbattState=BP_STATE_HI_V;
			}
			else
			{/*Nothing*/}

			if(tBPManagement.u8ProtectAction == BP_ACTION_RECOVER)
			{
				if(CallbackBattActionRecover != NULL)
				{
					CallbackBattActionRecover();
				}
				else
				{/*Nothing*/}
				tBPManagement.u8ProtectAction=BP_ACTION_NONE;
			}
			else
			{/*Nothing*/}
		break;

		case BP_STATE_LO_V:
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;

			/*Time count calculate base on battery voltage*/
			if(u16BatteryVol >= BP_VMIN_CFG)
			{
				tBPManagement.u16LoEnterNormalTime+=u16RoundTime;
				tBPManagement.u16LoEnterUnderTime=0U;
			}
			else
			{
				tBPManagement.u16LoEnterNormalTime=0U;
				tBPManagement.u16LoEnterUnderTime+=u16RoundTime;
			}

			/*Check to enter Normal*/
			if(tBPManagement.u16LoEnterNormalTime >= BP_LOTIME_CFG)
			{
				tBPManagement.u16LoEnterNormalTime=BP_LOTIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_NORM_V;
			}
			else
			{/*Nothing*/}

			/*Check to enter Under*/
			if(tBPManagement.u16LoEnterUnderTime >= BP_LOOVRTIME_CFG)
			{
				tBPManagement.u16LoEnterUnderTime=BP_LOOVRTIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_UNDER_V;
				tBPManagement.u8ProtectAction=BP_ACTION_UNDERVOL;
			}
			else
			{/*Nothing*/}
		break;

		case BP_STATE_HI_V:
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;

			/*Time count calculate base on battery voltage*/
			if(u16BatteryVol <= BP_VMAX_CFG)
			{
				tBPManagement.u16HiEnterNormalTime+=u16RoundTime;
				tBPManagement.u16HiEnterOverTime=0U;
			}
			else
			{
				tBPManagement.u16HiEnterNormalTime=0U;
				tBPManagement.u16HiEnterOverTime+=u16RoundTime;
			}

			/*Check to enter Normal*/
			if(tBPManagement.u16HiEnterNormalTime >= BP_HITIME_CFG)
			{
				tBPManagement.u16HiEnterNormalTime=BP_HITIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_NORM_V;
			}
			else
			{/*Nothing*/}

			/*Check to enter Over*/
			if(tBPManagement.u16HiEnterOverTime >= BP_HIOVRTIME_CFG)
			{
				tBPManagement.u16HiEnterOverTime=BP_HIOVRTIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_OVER_V;
				tBPManagement.u8ProtectAction=BP_ACTION_OVERVOL;
			}
			else
			{/*Nothing*/}
		break;

		case BP_STATE_UNDER_V:
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;

			/*Time count calculate base on battery voltage*/
			if(u16BatteryVol >= BP_VMINRCV_CFG)
			{
				tBPManagement.u16UnderEnterNormalTime+=u16RoundTime;
			}
			else
			{
				tBPManagement.u16UnderEnterNormalTime=0U;
			}

			/*Check to enter Normal*/
			if(tBPManagement.u16UnderEnterNormalTime >= BP_LORECOVTIME_CFG)
			{
				tBPManagement.u16UnderEnterNormalTime=BP_LORECOVTIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_NORM_V;
				tBPManagement.u8ProtectAction=BP_ACTION_RECOVER;
			}
			else
			{/*Nothing*/}

			if(tBPManagement.u8ProtectAction == BP_ACTION_UNDERVOL)
			{
				if(CallbackBattActionProtect != NULL)
				{
					CallbackBattActionProtect(BP_ACTION_UNDERVOL);
				}
				else
				{/*Nothing*/}
				tBPManagement.u8ProtectAction=BP_ACTION_NONE;
				
				/* Record 0xA3 Status */
				u16Temp = Memory_Pool_GeneralDiagnosis_Get();
				Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_LOW_VOL_ERROR_POS);
			}
			else
			{/*Nothing*/}
		break;

		case BP_STATE_OVER_V:
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;

			/*Time count calculate base on battery voltage*/
			if(u16BatteryVol <= BP_VMAXRCV_CFG)
			{
				tBPManagement.u16OverEnterNormalTime+=u16RoundTime;
			}
			else
			{
				tBPManagement.u16OverEnterNormalTime=0U;
			}

			/*Check to enter Normal*/
			if(tBPManagement.u16OverEnterNormalTime >= BP_HIRECOVTIME_CFG)
			{
				tBPManagement.u16OverEnterNormalTime=BP_HIRECOVTIME_CFG;
				tBPManagement.u8VbattState=BP_STATE_NORM_V;
				tBPManagement.u8ProtectAction=BP_ACTION_RECOVER;
			}
			else
			{/*Nothing*/}

			if(tBPManagement.u8ProtectAction == BP_ACTION_OVERVOL)
			{
				if(CallbackBattActionProtect != NULL)
				{
					CallbackBattActionProtect(BP_ACTION_OVERVOL);
				}
				else
				{/*Nothing*/}
				tBPManagement.u8ProtectAction=BP_ACTION_NONE;
				
				/* Record 0xA3 Status */
				u16Temp = Memory_Pool_GeneralDiagnosis_Get();
				Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_HIGH_VOL_ERROR_POS);
			}
			else
			{/*Nothing*/}
		break;

		default:
			tBPManagement.u8VbattState=BP_STATE_NORM_V;
			tBPManagement.u16LoEnterNormalTime=0U;
			tBPManagement.u16HiEnterNormalTime=0U;
			tBPManagement.u16LoEnterUnderTime=0U;
			tBPManagement.u16HiEnterOverTime=0U;
			tBPManagement.u16UnderEnterNormalTime=0U;
			tBPManagement.u16OverEnterNormalTime=0U;
		break;
	}
}

/******************************************************************************
 ;       Function Name			:	
 ;       Function Description	:	
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void M_BP_Callback_Register(CALLBACK_BATT_ACTION_PROTECT CBBattActionProtectReg, CALLBACK_BATT_ACTION_RECOVER CBBattActionRecoverReg)
{
	CallbackBattActionProtect=CBBattActionProtectReg;
	CallbackBattActionRecover=CBBattActionRecoverReg;
}

