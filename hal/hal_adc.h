
/******************************************************************************
;       Program		: hal_adc.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "public.h"

/*---------------------------- Include File ---------------------------------*/
/*---------------------------- Define Constant ------------------------------*/
#define HAL_ADC_EOS_MASK 	CY_SAR_INTR_EOS

//enum HAL_ADCChannel{
//	ADC_CH0 = 0U,
//	ADC_CH1 = 1U,
//	ADC_CH2 = 2U,
//	ADC_CH3 = 3U,
//};

#define HAL_ADC_PCB_TEMP			(uint32_t)0
#define HAL_ADC_BACKLIGHT_TEMP		(uint32_t)1
#define	HAL_ADC_VBATT_VOLT			(uint32_t)2
#define	HAL_ADC_SYNC_VOLT			(uint32_t)3
#define	HAL_ADC_FPCTXOUT_VOLT		(uint32_t)4
#define	HAL_ADC_FPCRXOUT_VOLT		(uint32_t)5

typedef struct{
	int16_t adcResult[4];
}adc_result_def;

//#define CHAN0SWITCHMASK = (uint32_t)(CY_SAR_MUX_FW_P0_VPLUS | CY_SAR_MUX_FW_P0_VMINUS);
//#define CHAN1SWITCHMASK = (uint32_t)(CY_SAR_MUX_FW_P1_VPLUS | CY_SAR_MUX_FW_P1_VMINUS);
//#define CHAN2SWITCHMASK = (uint32_t)(CY_SAR_MUX_FW_P2_VPLUS | CY_SAR_MUX_FW_P2_VMINUS);
//#define CHAN3SWITCHMASK = (uint32_t)(CY_SAR_MUX_FW_P3_VPLUS | CY_SAR_MUX_FW_P3_VMINUS);
/*---------------------------- Declare Global Variables -----------------------------*/
extern adc_result_def AdcResult;
/*---------------------------- Declare Function -----------------------------*/
extern void HAL_ADC_Init(void);
void HAL_ADC_Start_Convert(void);
//extern void HAL_HAL_ADC_CHs_Convert(void);
//int16_t HAL_ADC_PCB_Sense_Values_Get(void);
//int16_t HAL_ADC_BL_Sense_Values_Get(void);
//int16_t HAL_ADC_VBATT_Sense_Values_Get(void);
//int16_t HAL_ADC_SYNC_Sense_Values_Get(void);
extern uint16_t HAL_ADC_Convert_To_mVolt(uint8_t u8Ch);
extern void HAL_ADC_Disable(void);
extern void HAL_ADC_Enable(void);
extern void HAL_ADC_Stop(void);
int16_t HAL_ADC0_Get_Channel_mVolt(uint32_t u32ch,int16_t ADCread);
int16_t HAL_ADC0_Get_ADCRead(uint32_t u32ch);
cy_en_sysint_status_t HAL_ADC_Register_Event_Callback(cy_israddress SAR0_Isr);
uint32_t HAL_ADC0_GetIntStatusMask(void);
void HAL_ADC0_ClearIntFlag(uint32_t intrMask);

void HAL_ADC0_Callback(void);
#endif

