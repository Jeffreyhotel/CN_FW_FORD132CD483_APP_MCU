#include "M_TempertureProtect.h"
#include <stdlib.h>
#include "hal_adc.h"
#include "hal_pwm.h"
#include "hal_gpio.h"
//#include "C_memory_pool.h"


#define TP_TEMP_OVERTEMP       (uint16_t)78UL//(uint16_t)85UL
#define TP_TEMP_0         (uint16_t)0UL
#define TP_TEMP_NORMALTEMP      (uint16_t)77UL //(uint16_t)78UL





ttemperture_protect_info_def gtTPInfo;


/******************************************************************************
;       Function Name			:	uint8_t M_TP_Status_Get(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	uint8_t
;		Source ID				:
******************************************************************************/
uint8_t M_TP_Status_Get(void)
{
	return gtTPInfo.u8Status;
}
/******************************************************************************
;       Function Name			:	void M_TP_Status_Set(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void M_TP_Status_Set(uint8_t u8Action)
{
	gtTPInfo.u8Status = u8Action;
}

void M_TP_Init(void)
{
//	HAL_ADC_Init();

}

/******************************************************************************
;       Function Name			:	void M_TP_Function_Execute(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void M_TP_Function_Execute(uint8_t u8Action)
{
	switch(u8Action)
	{
		case TP_INIT_ADCVALUE:

			break;

		default:
			break;
	}
	M_TP_Status_Set(u8Action);
}
/******************************************************************************
;       Function Name			:	void M_TP_State_Maching(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t M_TP_State_Maching(ttemperture_protect_manage_def *tTPM)
{
    uint8_t u8Action;
    u8Action = TP_OFF;
    switch (tTPM->u8TempertureProtectMode)
    {
        case TP_STATE_INIT:
            tTPM->u8TempertureProtectMode = TP_STATE_NORMAL;
            break;
        case TP_STATE_NORMAL:
            if (tTPM->i16GetNowTemp >= TP_TEMP_OVERTEMP)
            {
                u8Action = TP_ACTION_OVERTEMP;
                tTPM->u8TempertureProtectMode = TP_STATE_HITEMP;
            }
            else
            {
                tTPM->u8TempertureProtectMode = TP_STATE_NORMAL;
            }
            break;
        case TP_STATE_HITEMP:
            if (tTPM->i16GetNowTemp <= TP_TEMP_NORMALTEMP)
            {

                tTPM->u8TempertureProtectMode = TP_STATE_NORMAL;
            }
            else
            {
                tTPM->u8TempertureProtectMode = TP_STATE_HITEMP;
            }
            break;
//		case TP_STATE_LOWTEMP:
//			break;
		default:
			//un-expecting case
			break;
	}
	return u8Action;
}
