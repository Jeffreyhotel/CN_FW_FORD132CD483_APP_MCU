
/******************************************************************************
;       Program		: M_DataCollecting.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#ifndef M_DATACOLLECTING_H
#define M_DATACOLLECTING_H

#include "public.h"

#define SAMPLE_ELEMENT_NUM		8U
#define THERMAL_SIGNALSTABLETIME	20U /*Temperature stable counter.*/
#define THERMAL_DEFAULT 			25  /*Default Temperature.*/

typedef struct
{
	uint8_t u8Status;
	uint8_t u8Cnt;
	uint8_t u8Index;
	uint8_t u8FIRInit;
	uint8_t u8IIRInit;
	int16_t mi16PCBTempADCData[SAMPLE_ELEMENT_NUM];
	int16_t mi16BacklightTempADCData[SAMPLE_ELEMENT_NUM];
	int16_t mi16VbattADCData[SAMPLE_ELEMENT_NUM];
	int16_t mi16VSyncADCData[SAMPLE_ELEMENT_NUM];
	int16_t mi16FPCTxOutADCData[SAMPLE_ELEMENT_NUM];
	int16_t mi16FPCRxOutADCData[SAMPLE_ELEMENT_NUM];
	/*ADC Value*/
	float fPresentPCBTempADC;
	int16_t i16PresentPCBTempADC;
	float fPresentBacklightTempADC;
	int16_t i16PresentBacklightTempADC;
	int16_t i16PresentVbattADC;
	int16_t i16PresentVSyncADC;
	int16_t i16PresentFPCTxOutADC;
	int16_t i16PresentFPCRxOutADC;
	/*Analog Voltage*/
	int16_t i16PresentTPCBAnaVol;
	int16_t i16PresentTBacklightAnaVol;
	int16_t i16PresentVbattAnaVol;
	int16_t i16PresentVSyncAnaVol;
	int16_t i16PresentFPCTxAnaVol;
	int16_t i16PresentFPCRxAnaVol;
	/*Actual Value*/
	int16_t i16PresentTPCB;
	int16_t i16PresentTBacklight;
	int16_t i16PresentVbatt;
	int16_t i16PresentVSync;
	int16_t i16PresentFPCTxOut;
	int16_t i16PresentFPCRxOut;
}data_collecting_info_def;

typedef struct
{	 
	int16_t i16ActulValue;
	uint16_t u16ADCVol;
}tmtx_tbl_element_def;

#define DC_OFF						0xFFU
#define DC_INIT_ADCVALUE			0x00U
#define DC_TURNON_VBATT_MEASURE		0x01U
#define DC_GETVALUE			    	0x02U
#define DC_START_MEASURE			0x03U
#define DC_QUERY_VBATT_TABLE		0x04U
#define DC_QUERY_VSYNC_TABLE		0x05U
#define DC_QUERY_PCBT_TABLE			0x06U
#define DC_QUERY_BLT_TABLE			0x07U
#define DC_QUERY_FPCTXOUT_TABLE		0x08U
#define DC_QUERY_FPCRXOUT_TABLE		0x09U

extern data_collecting_info_def gtDCInfo;

void M_DC_Init(void);
uint8_t M_DC_Status_Get(void);
void M_DC_Status_Set(uint8_t u8Action);
void M_DC_Function_Execute(uint8_t u8Action);
void M_DC_Callback(void);

#endif
