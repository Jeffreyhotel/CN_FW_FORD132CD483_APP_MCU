/******************************************************************************
 ;       Program		: C_Backlight_Management.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "../Mymain.h"

typedef struct
{
	uint16_t u16PresentDuty;
	uint16_t u16TargetDuty;
	bool bDeratingEnable;
} tbacklight_def;

typedef struct
{
	int16_t i16PCBTemperature;
	int16_t i16BLTemperature;
	bool bTemperatureProtectEnable;
} ttemperture_def;

typedef struct
{
	uint8_t u8VBattState;
	uint8_t u8BatteryProtectStatus;
} tbatt_protect_def;

typedef struct
{
	uint8_t u8ID;
	uint8_t u8Subversion;
} tdisplay_identification_def;

typedef struct
{
	int16_t i16BacklightTemperature;
	int16_t i16PCBATemperature;
	uint16_t u16SyncVol;
	uint16_t u16BatteryVol;
	int16_t i16FPCTxOutVol;
	int16_t i16FPCRxOutVol;

	uint16_t u16TBacklightAnaVol;
	uint16_t u16TPCBAAnaVol;
	uint16_t u16SyncAnaVol;
	uint16_t u16BatteryAnaVol;
	int16_t i16FPCTxOutAnaVol;
	int16_t i16FPCRxOutAnaVol;

	int16_t i16BacklightTemperatureADC;
	int16_t i16PCBATemperatureADC;
	int16_t i16SyncADC;
	int16_t i16BatteryADC;
	int16_t i16FPCTxOutADC;
	int16_t i16FPCRxOutADC;
} tdata_collection_def;

/* FPN Read/Write */
typedef struct
{
	uint8_t WRT_ST    : 1;
	uint8_t INT_WRT   : 1;
	uint8_t CKSUM_ERR : 1;
	uint8_t RESERVED  : 5;
} __attribute__((__packed__)) tfpn_status_info_def;

/* Retrieve Production phase byte Read/Write */
typedef struct
{
	uint8_t WRT_ST   : 1;
	uint8_t INT_WRT  : 1;
	uint8_t RESERVED : 6;
} __attribute__((__packed__)) tProduct_pn_status_info_def;

typedef struct
{
	uint8_t u8Command;
	bool bCommunicationMask;

	uint8_t u8EppromIndex;
	uint8_t mu8FPNSoft[25U];
	uint8_t mu8FPNCore[25U];
	uint8_t mu8FPNMain[25U];
	uint8_t mu8FPNDelivery[25U];
	uint8_t mu8FPNSerial[25U];
	uint8_t mu8FPNID[25U];
	uint8_t u8ProductionPhase;

	tfpn_status_info_def tFPNDeliverystatusInfo;
	tfpn_status_info_def tFPNSerialstatusInfo;
	tProduct_pn_status_info_def tFPNSProductPhaseStatusInfo;
	bool bWriteFPNDeliveryStatusReg;
	bool bWriteFPNDelivery;
	bool bWriteSerNumPNStatusReg;
	bool bWriteSerNumFPN; 
	bool bWriteProductPhasePNStatusReg;
	bool bWriteProductPhaseFPN;
	uint8_t mu8WriteFFPNDelivery[26U];
	uint8_t mu8WriteFPNSerial[26U];
	uint8_t u8WriteProductionPhase;
} tcommunication_def;

typedef struct
{
	uint8_t u8DisplayEnable;
	uint8_t u8DisplayScanning;
	uint8_t u8DisplayShutdown;
	bool bBacklightEnable;

	uint8_t u8DisplayEnableBackup;
	uint8_t u8DisplayEnableSet;

	uint8_t u8DisplayStatus;
	uint8_t u8TouchStatus;
	uint8_t u8ScanStatus;

	uint8_t u8LcdResetStatus;

	uint8_t u8FactoryMode;

	uint32_t u32NT51926_Vcom;
	uint8_t	u8NT51926VGMA[4U];
	bool bDisplayBusySet;
} tdisplay_management_def;

typedef struct
{
	uint8_t u8PowerState;
	uint8_t u8PowerStatus;

	uint8_t u8SyncStatus;
	bool bSoftwareReset;
} tpower_management_def;

typedef struct
{
	uint8_t u8IntStatus;
	bool bDiagnosisEnable;
	uint32_t u32DisplayStatus;
	uint32_t u32DisplayStatusPreHostCommand;
	uint32_t u32ActualDisplayStatus;
	uint32_t u32DisplayStatusBp;
	uint32_t u32ReadAfterDisplayStatus;
	bool bLockLoss;

	uint16_t u16GeneralDiagnosis;
	uint64_t u64LEDDriverDiagnosis;
	uint64_t u64NT51926Diagnosis;
	uint8_t u8NT51926FWDiagnosis;
	uint16_t u16IcCommunicationDiagnosis;
} tdiagnosis_def;

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
typedef struct
{
	uint8_t u8DispFaultPinLevel;
	uint8_t u8DispFaultStatus[8];
	
	uint8_t u8LedINTPinLevel;
	uint8_t u8LedFaultStatus[6];
	
	uint8_t u8PG1V2PinLevel;
	uint8_t u8PG3V3PinLevel;
	uint8_t u8LockPinLevel;

	uint16_t u16BatteryVol;
	uint16_t u16FPCTXVol;
	uint16_t u16FPCRXVol;
	uint16_t u16FPCBLVol;
	
	int16_t i16PCBATemperature;

	uint16_t u16IcCommunicationDiagnosis;
} tdiagnosis_simulate_def;
#endif


/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSoft_Set(const uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNSoft_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_FPNCore_Set(const uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNCore_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_FPNMain_Set(const uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNMain_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_FPNDelivery_Set(uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNDelivery_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_FPNID_Set(uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNID_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_FPNSerial_Set(uint8_t *pSetValue, uint8_t u8Length);
void Memory_Pool_FPNSerial_Get(uint8_t *pSetValue, uint8_t u8Length);

void Memory_Pool_EppromIndex_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_EppromIndex_Get(void);

bool Memory_Pool_CommunicationMask_Get(void);
void Memory_Pool_CommunicationMask_Set(bool bSetValue);

void Memory_Pool_BacklightDuty_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_BacklightDuty_Get(void);

void Memory_Pool_NowDuty_Set(uint16_t u16SetValue);

void Memory_Pool_BLTemp_Set(int16_t i16SetValue);
int16_t Memory_Pool_BLTemp_Get(void);

void Memory_Pool_BLTempAnaVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_BLTempAnaVol_Get(void);

void Memory_Pool_PCBATemp_Set(int16_t i16SetValue);
int16_t Memory_Pool_PCBATemp_Get(void);

void Memory_Pool_PCBATempAnaVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_PCBATempAnaVol_Get(void);

void Memory_Pool_SyncVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_SyncVol_Get(void);

void Memory_Pool_SyncAnaVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_SyncAnaVol_Get(void);

void Memory_Pool_BatteryVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_BatteryVol_Get(void);

void Memory_Pool_BatteryAnaVol_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_BatteryAnaVol_Get(void);

void Memory_Pool_FPCTxOutVol_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCTxOutVol_Get(void);

void Memory_Pool_FPCTxOutAnaVol_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCTxOutAnaVol_Get(void);

void Memory_Pool_FPCRxOutVol_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCRxOutVol_Get(void);

void Memory_Pool_FPCRxOutAnaVol_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCRxOutAnaVol_Get(void);

void Memory_Pool_BLTempADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_BLTempADC_Get(void);

void Memory_Pool_PCBATempADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_PCBATempADC_Get(void);

void Memory_Pool_SyncADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_SyncADC_Get(void);

void Memory_Pool_BatteryADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_BatteryADC_Get(void);

void Memory_Pool_FPCTxOutADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCTxOutADC_Get(void);

void Memory_Pool_FPCRxOutADC_Set(int16_t i16SetValue);
int16_t Memory_Pool_FPCRxOutADC_Get(void);

void Memory_Pool_LEDDiagnosis_Set(uint64_t u64SetValue);
uint64_t Memory_Pool_LEDDiagnosis_Get(void);

void Memory_Pool_GeneralDiagnosis_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_GeneralDiagnosis_Get(void);

void Memory_Pool_LEDDiagnosis_Set(uint64_t u64SetValue);
uint64_t Memory_Pool_LEDDiagnosis_Get(void);

void Memory_Pool_NT51926Diagnosis_Set(uint64_t u64SetValue);
uint64_t Memory_Pool_NT51926Diagnosis_Get(void);

void Memory_Pool_NT51926FWDiagnosis_Set(uint8_t u64SetValue);
uint8_t Memory_Pool_NT51926FWDiagnosis_Get(void);

void Memory_Pool_IcCommDiagnosis_Set(uint16_t u16SetValue);
uint16_t Memory_Pool_IcCommDiagnosis_Get(void);

void Memory_Pool_DisplayStatusBp_Set(uint32_t u32SetValue);
uint32_t Memory_Pool_DisplayStatusBp_Get(void);

void Memory_Pool_DisplayStatus_Set(uint32_t u32SetValue);
uint32_t Memory_Pool_DisplayStatus_Get(void);

void Memory_Pool_ActualDisplayStatus_Set(uint32_t u32SetValue);
uint32_t Memory_Pool_ActualDisplayStatus_Get(void);

void Memory_Pool_ReadAfterDisplayStatus_Set(uint32_t u32SetValue);
uint32_t Memory_Pool_ReadAfterDisplayStatus_Get(void);

void Memory_Pool_DiagnosisEnable_Set(bool bSetValue);
bool Memory_Pool_DiagnosisEnable_Get(void);

void Memory_Pool_TempProtectEn_Set(bool bSetValue);
bool Memory_Pool_TempProtectEn_Get(void);

void Memory_Pool_LockLoss_Set(bool bSetValue);
bool Memory_Pool_LockLoss_Get(void);

void Memory_Pool_IntStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_IntStatus_Get(void);

void Memory_Pool_SoftwareReset_Set(bool bSetValue);
bool Memory_Pool_SoftwareReset_Get(void);

void Memory_Pool_SyncStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_SyncStatus_Get(void);

void Memory_Pool_FactoryMode_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_FactoryMode_Get(void);

void Memory_Pool_ScanStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_ScanStatus_Get(void);

void Memory_Pool_TouchStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_TouchStatus_Get(void);

void Memory_Pool_LcdStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_LcdStatus_Get(void);

bool Memory_Pool_DeratingEnable_Get(void);
void Memory_Pool_DeratingEnable_Set(bool bSetValue);

void Memory_Pool_BacklightEnable_Set(bool bSetValue);
bool Memory_Pool_BacklightEnable_Get(void);

void Memory_Pool_Shutdown_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_Shutdown_Get(void);

void Memory_Pool_DisplayScan_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_DisplayScan_Get(void);

void Memory_Pool_DisplayEnableBp_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_DisplayEnableBp_Get(void);

void Memory_Pool_DisplayEnable_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_DisplayEnable_Get(void);

void Memory_Pool_DisplayEnable_Reg_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_DisplayEnable_Reg_Get(void);

void Memory_Pool_DisplayBusy_Set(bool bSetValue);
bool Memory_Pool_DisplayBusy_Get(void);

void Memory_Pool_PowerStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_PowerStatus_Get(void);

void Memory_Pool_PowerState_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_PowerState_Get(void);

#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
	void Memory_Pool_NT51926_Vcom_Set(uint32_t u32SetValue);
	uint32_t Memory_Pool_NT51926_Vcom_Get(void);
	int8_t Memory_Pool_NT51926_VGMA_Set(const uint8_t *);	/* Jacky@220928, for VGMA*/
	int8_t Memory_Pool_NT51926_VGMA_Get(uint8_t *);
#endif

void Memory_Pool_Command_Info_Assign(uint8_t *pCmdBuffer);
void Memory_Pool_Command_Info_Fetch(uint8_t *pDataBuffer, uint8_t *pLength);

void Memory_Pool_LcdResetStatus_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_LcdResetStatus_Get(void);

void Memory_Pool_VBattProtectState_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_VBattProtectState_Get(void);

bool Memory_Pool_WriteFPNDeliveryStatusReg_Get(void);
void Memory_Pool_WriteFPNDeliveryStatusReg_Set(bool bSetValue);

bool Memory_Pool_WriteFPNDelivery_Get(void);
void Memory_Pool_WriteFPNDelivery_Set(bool bSetValue);

bool Memory_Pool_WriteSerNumPNStatusReg_Get(void);
void Memory_Pool_WriteSerNumPNStatusReg_Set(bool bSetValue);

bool Memory_Pool_WriteSerNumFPN_Get(void);
void Memory_Pool_WriteSerNumFPN_Set(bool bSetValue);

bool Memory_Pool_WriteProductPhasePNStatusReg_Get(void);
void Memory_Pool_WriteProductPhasePNStatusReg_Set(bool bSetValue);

bool Memory_Pool_WriteProductPhaseFPN_Get(void);
void Memory_Pool_WriteProductPhaseFPN_Set(bool bSetValue);

uint8_t *Memory_Pool_pdelivery_assembly_Get(void);
uint8_t *Memory_Pool_pSerNum_assembly_Get(void);
uint8_t *Memory_Pool_pProductionPhase_Get(void);

void Memory_Pool_FPNDeliverystatusInfo_Get(tfpn_status_info_def * pData);
void Memory_Pool_FPNDeliverystatusInfo_Set(tfpn_status_info_def * pData);

void Memory_Pool_FPNSerialstatusInfo_Get(tfpn_status_info_def * pData);
void Memory_Pool_FPNSerialstatusInfo_Set(tfpn_status_info_def * pData);

void Memory_Pool_FPNSProductPhaseStatusInfo_Get(tProduct_pn_status_info_def * pData);
void Memory_Pool_FPNSProductPhaseStatusInfo_Set(tProduct_pn_status_info_def * pData);

void Memory_Pool_FPNProductionPhase_Set(uint8_t u8SetValue);
uint8_t Memory_Pool_FPNProductionPhase_Get(void);

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
tdiagnosis_simulate_def Memory_Pool_DiagnosisSimulateInfo_Get(void);
#endif

//#if (BX726_TDDI_NT51926)
extern int8_t Get_MCUVersion(uint8_t *);
//#endif

#endif

