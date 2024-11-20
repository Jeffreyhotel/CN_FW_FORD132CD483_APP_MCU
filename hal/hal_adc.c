/******************************************************************************
;				Program		:	hal_adc.c
;				Function	:	ADC Sensing Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:	 
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_adc.h"

#include "../app/Mymain.h"



/* Scenario: Configure and enable the SAR interrupt. */
cy_stc_sysint_t SAR0_IrqConfig =
{
		/* .intrSrc         = */ pass_0_interrupt_sar_IRQn,   /* Interrupt source is the SAR interrupt */
		/* .intrPriority    = */ 3UL                          /* Interrupt priority is 3 */
};
/*---------------------------- Declare Global Variable ----------------------*/
//static uint16_t u16PcbSenseAdcCh0, u16BlSenseAdcCh1, u16VbattSenseAdcCh2;
adc_result_def AdcResult;


/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	void HAL_ADC_Init(void)
;       Function Description	:	Initialize ADC capture function
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-001
******************************************************************************/
void HAL_ADC_Init(void)
{

	(void)Cy_SAR_Init(SAR0, &ADC_CAP_config);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(SAR0_IrqConfig.intrSrc);
	/* Clear possible interrupt erroneously raised during block enabling */
	Cy_SAR_ClearInterrupt(SAR0, CY_SAR_INTR);
	Cy_SAR_SetInterruptMask(SAR0, CY_SAR_INTR_EOS);
	Cy_SAR_Enable(SAR0);

}


/******************************************************************************
;       Function Name			:	void HAL_ADC_CHs_Convert(void)
;       Function Description	:	Channel convert ADC values.
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-002
******************************************************************************/
void HAL_ADC_Start_Convert(void)
{
	Cy_SAR_StartConvert(SAR0, CY_SAR_START_CONVERT_SINGLE_SHOT);
}
/******************************************************************************
;       Function Name			:	uint16_t HAL_ADC_PCB_Sense_Values_Get(void)
;       Function Description	:	Get PCB sense ADC values.
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-003
******************************************************************************/
int16_t HAL_ADC_PCB_Sense_Values_Get(void)
{
	return (AdcResult.adcResult[0] & 0x7ff);
}

/******************************************************************************
;       Function Name			:	uint16_t Adc_BL_Sense_Values_Get(void)
;       Function Description	:	Get backlight sense ADC values.
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-004
******************************************************************************/
int16_t HAL_ADC_BL_Sense_Values_Get(void)
{
	return (AdcResult.adcResult[1] & 0x7ff);
}

/******************************************************************************
;       Function Name			:	uint16_t HAL_ADC_VBATT_Sense_Values_Get(void)
;       Function Description	:	Get battery voltage sense ADC values.
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-005
******************************************************************************/
int16_t HAL_ADC_VBATT_Sense_Values_Get(void)
{
	return (AdcResult.adcResult[2] & 0x7ff);
}

/******************************************************************************
;       Function Name			:	uint16_t HAL_ADC_VBATT_Sense_Values_Get(void)
;       Function Description	:	Get battery voltage sense ADC values.
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-005
******************************************************************************/
int16_t HAL_ADC_SYNC_Sense_Values_Get(void)
{
	return (AdcResult.adcResult[3] & 0x7ff);
}

int16_t HAL_ADC0_Get_ADCRead(uint32_t u32ch)
{
	int16_t i16ADCValue=0;

	switch(u32ch){
	case HAL_ADC_PCB_TEMP:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	case HAL_ADC_BACKLIGHT_TEMP:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	case HAL_ADC_VBATT_VOLT:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	case HAL_ADC_SYNC_VOLT:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	case HAL_ADC_FPCTXOUT_VOLT:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	case HAL_ADC_FPCRXOUT_VOLT:
		i16ADCValue = Cy_SAR_GetResult16(SAR0, u32ch);
	break;
	default:
	break;
	}
	return i16ADCValue;
}

int16_t HAL_ADC0_Get_Channel_mVolt(uint32_t u32ch,int16_t ADCread)
{
	int16_t i16ADCValue=0;

	switch(u32ch){
	case HAL_ADC_PCB_TEMP:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	case HAL_ADC_BACKLIGHT_TEMP:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	case HAL_ADC_VBATT_VOLT:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	case HAL_ADC_SYNC_VOLT:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	case HAL_ADC_FPCTXOUT_VOLT:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	case HAL_ADC_FPCRXOUT_VOLT:
		i16ADCValue = Cy_SAR_CountsTo_mVolts(SAR0, u32ch, ADCread);
	break;
	default:
	break;
	}
	return i16ADCValue;
}
/******************************************************************************
;       Function Name			:	void HAL_ADC_Disable(void)
;       Function Description	:	Disable ADC function.
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-007
******************************************************************************/
void HAL_ADC_Disable(void)
{
	Cy_SAR_Disable(SAR0);
}

/******************************************************************************
;       Function Name			:	void HAL_ADC_Enable(void)
;       Function Description	:	Enable ADC function.
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-008
******************************************************************************/
void HAL_ADC_Enable(void)
{
	Cy_SAR_Enable(SAR0);
}

/******************************************************************************
;       Function Name			:	void HAL_ADC_Stop(void)
;       Function Description	:	Stop ADC function.
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-009
******************************************************************************/
void HAL_ADC_Stop(void)
{
	Cy_SAR_StopConvert(SAR0);
}
/*---------------------------------------------------------------------------*/ 
/******************************************************************************
;       Function Name			:	sysintStatus HAL_ADC_Register_Event_Callback(cy_israddress SAR0_Isr)
;       Function Description	:	Registe ADC user interrupt callback function
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-005
******************************************************************************/
cy_en_sysint_status_t HAL_ADC_Register_Event_Callback(cy_israddress SAR0_Isr)
{
	/* Configure the interrupt with vector at SAR0_Isr(). */
	cy_en_sysint_status_t sysintStatus = Cy_SysInt_Init(&SAR0_IrqConfig, SAR0_Isr);
	if (CY_SYSINT_SUCCESS != sysintStatus)
	{
		/* insert error handling here */
	}
	return sysintStatus;
}

uint32_t HAL_ADC0_GetIntStatusMask(void){
	return Cy_SAR_GetInterruptStatusMasked(SAR0);
}

void HAL_ADC0_ClearIntFlag(uint32_t intrMask){
	Cy_SAR_ClearInterrupt(SAR0, intrMask);
}

