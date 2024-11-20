/******************************************************************************
 ;       Program		: M_TempertureProtect.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		:
 ;       Author		:
 ******************************************************************************/
#ifndef M_TEMPERTUREPROTECT_H
#define M_TEMPERTUREPROTECT_H

#include "public.h"

#define TP_OFF					0xFFU
#define TP_INIT_ADCVALUE		0x00U
#define TP_STATE_INIT			0x01U
#define TP_STATE_NORMAL			0X02U
#define TP_STATE_LOWTEMP		0x03U
#define TP_STATE_HITEMP			0x04U
#define TP_ACTION_OVERTEMP		0x01U

typedef struct
{
    int16_t i16GetNowTemp;
    uint8_t u8TempertureProtectMode;
    bool bTempertureProtectEnable;
} ttemperture_protect_manage_def;

typedef struct
{
    uint8_t u8Status;
    uint8_t u8Cnt;
    uint8_t u8Index;

} ttemperture_protect_info_def;

extern ttemperture_protect_info_def gtTPInfo;

void M_TP_Init(void);
uint8_t M_TP_Status_Get(void);
void M_TP_Status_Set(uint8_t u8Action);
void M_TP_Function_Execute(uint8_t u8Action);
uint8_t M_TP_State_Maching(ttemperture_protect_manage_def *tTPM);

#endif
