#include "Memory_Pool.h"
#include "public.h"
#include "C_Diagnosis.h"
#include "C_Communication.h"
#include "C_Display_Management.h"
#include "M_FPNCtrl.h"
#include "ICDiagApp.h"
#include "M_TemperatureDerating.h"
#include "M_DisplayManage.h"
#include "M_PowerManagement.h"
#include "FIDM_Config.h"

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
#include "M_BatteryProtect.h"
#endif
//#ifdef BX726_TDDI_NT51926
#include "M_BacklightControl.h"
//#endif

tcommunication_def gtCommunicationInfo;
tdata_collection_def gtDataCollectInfo;
tbacklight_def gtBacklightInfo;
ttemperture_def gtTemperatureInfo;
tbatt_protect_def gtBattProtectInfo;
tdiagnosis_def gtDiagnosisInfo = 
{
	.u32DisplayStatusPreHostCommand = 0x00U, 
	.u32DisplayStatus = 0x00U,
	.u32ActualDisplayStatus = 0x00U,
};

tpower_management_def gtPowerManageInfo;

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
tdiagnosis_simulate_def gtDiagnosisSimulateInfo = 
{
	.u8DispFaultPinLevel = GPIO_HIGH,
	.u8DispFaultStatus = {0U,0U,0U,0U,0U,0U,1U,0U},
	.u8LedINTPinLevel = GPIO_HIGH,
	.u8LedFaultStatus = {0U,0U,0U,0U,0U,0U},
	.u8PG1V2PinLevel = GPIO_HIGH,
	.u8PG3V3PinLevel = GPIO_HIGH,
	.u8LockPinLevel = GPIO_HIGH,
	.u16BatteryVol = (BP_VMIN_CFG + 1U),
	.u16FPCTXVol = (DIAG_FPC_TX_DISCON_VOL + 1U),
	.u16FPCRXVol = (DIAG_FPC_RX_DISCON_VOL + 1U),
	.i16PCBATemperature = (TEMP_DERATING_DEFAULT_DERA_TEMP - (1*TEMP_DERATING_TEMP_RESOLUTION)),
	.u16IcCommunicationDiagnosis = 0U,
	.u16FPCBLVol = (DIAG_FPC_BL_DISCON_VOL),
};
#endif

tdisplay_management_def gtDisplayManageInfo = { .u8DisplayStatus = DISPLAY_UNKNOW, .u8DisplayEnableBackup = DISPLAY_UNKNOW, .u8DisplayEnableSet = DISPLAY_UNKNOW, .u8TouchStatus = TOUCH_UNKNOW, .u8ScanStatus = SCAN_UNKNOW, .bBacklightEnable = false, .u8LcdResetStatus = LCD_RESET_UNKNOW, .u32NT51926_Vcom=0x00U, .bDisplayBusySet = false };

#if(U717_TDDI_NT51926)
    const tdisplay_identification_def ctDisplayID = { .u8ID = 0x3AU, .u8Subversion = 0x01U };    
#elif(CX430_TDDI_NT51926)
    const tdisplay_identification_def ctDisplayID = { .u8ID = 0x3FU, .u8Subversion = 0x01U };
#elif(BX726_TDDI_NT51926)
    const tdisplay_identification_def ctDisplayID = { .u8ID = 0x3BU, .u8Subversion = 0x03U };
#else
#endif

/************************************************************************************
 ;  [Phase].[大版號].[小版號].[流水碼]
 ;  Phase   : F = MP後, T = MP前 
 ;  大版號  : 對應各個軟體milestone或對外發行而進位，範圍: 0 ~ 99
 ;  小版號  : 對應軟體function ready或整合而進版，當大版號進位後，此碼歸零，範圍: 1 ~ 99
 ;  流水號  : Build number，RD自行記錄用，當小版號進位後，此碼歸零，範圍: 1 ~ 99
 ************************************************************************************/
#if(CX430_TDDI_NT51926)
    const uint8_t cmu8McuVersion[] = { "F-04.01.00" };
#elif(BX726_TDDI_NT51926)
    const uint8_t cmu8McuVersion[] = { "F-04.02.00" };
#elif(U717_TDDI_NT51926)
    const uint8_t cmu8McuVersion[] = { "F-04.01.00" };
#else
#endif


//#if (BX726_TDDI_NT51926)
/**
 * @brief Calls MPWMDimming_Routine6ms() to run independent state machine.
 *
 * @details 1.The different routine base time is immutable.\n
 * 2.The different state machine checking time is immutable..
 *
 * @note 1.The state machine checking time is 6ms.\n
 *
 */
int8_t Get_MCUVersion(uint8_t *pu8Out)
{
	uint8_t u8Len = 0U, u8Loop = 0U;
	int8_t i8Rt = -1;


	u8Len = strlen((const char *)cmu8McuVersion);
	if( 0U < u8Len)
	{
		for( u8Loop = 0U ; u8Loop < u8Len ; u8Loop++ )
		{
			*pu8Out = cmu8McuVersion[u8Loop];
			pu8Out++;
		}
		i8Rt = 1U;
	}
	else
	{
		;/* no elements in cmu8McuVersion[] */
	}
	return i8Rt;
}
//#endif
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNDelivery_Set(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNDelivery, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNDelivery_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNDelivery, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSoft_Set(const uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNSoft, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSoft_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNSoft, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNCore_Set(const  uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNCore, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNCore_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNCore, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNMain_Set(const uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNMain, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNMain_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNMain, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNID_Set(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNID, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNID_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNID, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSerial_Set(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(gtCommunicationInfo.mu8FPNSerial, pSetValue, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSerial_Get(uint8_t *pSetValue, uint8_t u8Length)
{
    memcpy(pSetValue, gtCommunicationInfo.mu8FPNSerial, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNProductionPhase_Set(uint8_t u8SetValue)
{
    gtCommunicationInfo.u8ProductionPhase = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_FPNProductionPhase_Get(void)
{
    return gtCommunicationInfo.u8ProductionPhase;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_EppromIndex_Set(uint8_t u8SetValue)
{
    gtCommunicationInfo.u8EppromIndex = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_EppromIndex_Get(void)
{
    return gtCommunicationInfo.u8EppromIndex;
}
/******************************************************************************
 ;       Function Name			:	bool Memory_Pool_bWriteFPNDeliveryStatusReg_Get(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteFPNDeliveryStatusReg_Get(void)
{
    return gtCommunicationInfo.bWriteFPNDeliveryStatusReg;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_bWriteFPNDeliveryStatusReg_Set(bool bSetValue)
 ;       Function Description	:
 ;       Parameters				:	bool bSetValue
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteFPNDeliveryStatusReg_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteFPNDeliveryStatusReg = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteFPNDelivery_Get(void)
{
    return gtCommunicationInfo.bWriteFPNDelivery;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteFPNDelivery_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteFPNDelivery = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteSerNumPNStatusReg_Get(void)
{
    return gtCommunicationInfo.bWriteSerNumPNStatusReg;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteSerNumPNStatusReg_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteSerNumPNStatusReg = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteSerNumFPN_Get(void)
{
    return gtCommunicationInfo.bWriteSerNumFPN;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteSerNumFPN_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteSerNumFPN = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteProductPhasePNStatusReg_Get(void)
{
    return gtCommunicationInfo.bWriteProductPhasePNStatusReg;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteProductPhasePNStatusReg_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteProductPhasePNStatusReg = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_WriteProductPhaseFPN_Get(void)
{
    return gtCommunicationInfo.bWriteProductPhaseFPN;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_WriteProductPhaseFPN_Set(bool bSetValue)
{
    gtCommunicationInfo.bWriteProductPhaseFPN = bSetValue;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNDeliverystatusInfo_Get(tfpn_status_info_def * pData)
{
    memcpy(pData,  &gtCommunicationInfo.tFPNDeliverystatusInfo, sizeof(tfpn_status_info_def));
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNDeliverystatusInfo_Set(tfpn_status_info_def * pData)
{
    memcpy(&gtCommunicationInfo.tFPNDeliverystatusInfo, pData , sizeof(tfpn_status_info_def));
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSerialstatusInfo_Get(tfpn_status_info_def * pData)
{
    memcpy(pData,  &gtCommunicationInfo.tFPNSerialstatusInfo, sizeof(tfpn_status_info_def));
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSerialstatusInfo_Set(tfpn_status_info_def * pData)
{
    memcpy(&gtCommunicationInfo.tFPNSerialstatusInfo, pData , sizeof(tfpn_status_info_def));
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSProductPhaseStatusInfo_Get(tProduct_pn_status_info_def * pData)
{
    memcpy(pData,  &gtCommunicationInfo.tFPNSProductPhaseStatusInfo, sizeof(tProduct_pn_status_info_def));
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPNSProductPhaseStatusInfo_Set(tProduct_pn_status_info_def * pData)
{
    memcpy(&gtCommunicationInfo.tFPNSProductPhaseStatusInfo, pData , sizeof(tfpn_status_info_def));
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t *Memory_Pool_pdelivery_assembly_Get(void)
{
    return gtCommunicationInfo.mu8WriteFFPNDelivery;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t *Memory_Pool_pSerNum_assembly_Get(void)
{
    return gtCommunicationInfo.mu8WriteFPNSerial;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t *Memory_Pool_pProductionPhase_Get(void)
{
    return &gtCommunicationInfo.u8WriteProductionPhase;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_CommunicationMask_Get(void)
{
    return gtCommunicationInfo.bCommunicationMask;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_CommunicationMask_Set(bool bSetValue)
{
    gtCommunicationInfo.bCommunicationMask = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BacklightDuty_Set(uint16_t u16SetValue)
{
    gtBacklightInfo.u16TargetDuty = u16SetValue & 0x03FFU;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_DeratingEnable_Get(void)
{
    return gtBacklightInfo.bDeratingEnable;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DeratingEnable_Set(bool bSetValue)
{
    gtBacklightInfo.bDeratingEnable = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_BacklightDuty_Get(void)
{
    return gtBacklightInfo.u16TargetDuty & 0x03FFU;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_NowDuty_Set()
 ;       Function Description	:
 ;       Parameters				:	uint16_t u16SetValue
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_NowDuty_Set(uint16_t u16SetValue)
{
    gtBacklightInfo.u16PresentDuty = (u16SetValue & 0x03FFU);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BLTemp_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16BacklightTemperature = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_BLTemp_Get(void)
{
    return gtDataCollectInfo.i16BacklightTemperature;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BLTempAnaVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16TBacklightAnaVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_BLTempAnaVol_Get(void)
{
    return gtDataCollectInfo.u16TBacklightAnaVol;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_PCBATemp_Set(int16_t i16SetValue)
{

#if (HIJACK_ADC)
	if(0x01U == MBacklightControl_GetADCHiJack())
	{
		gtDataCollectInfo.i16PCBATemperature = MBacklightControl_GetHADC();
	}
	else
	{
		gtDataCollectInfo.i16PCBATemperature = i16SetValue;
	}
#else
    gtDataCollectInfo.i16PCBATemperature = i16SetValue;
#endif
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_PCBATemp_Get(void)
{
#if (BACKDOOR_DIAGNOSIS_SIMULATE)
    return gtDiagnosisSimulateInfo.i16PCBATemperature;
#else
    return gtDataCollectInfo.i16PCBATemperature;
#endif
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_PCBATempAnaVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16TPCBAAnaVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_PCBATempAnaVol_Get(void)
{
    return gtDataCollectInfo.u16TPCBAAnaVol;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_SyncVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16SyncVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_SyncVol_Get(void)
{
    return gtDataCollectInfo.u16SyncVol;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_SyncAnaVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16SyncAnaVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_SyncAnaVol_Get(void)
{
    return gtDataCollectInfo.u16SyncAnaVol;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BatteryVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16BatteryVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_BatteryVol_Get(void)
{
    return gtDataCollectInfo.u16BatteryVol;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BatteryAnaVol_Set(uint16_t u16SetValue)
{
    gtDataCollectInfo.u16BatteryAnaVol = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_BatteryAnaVol_Get(void)
{
    return gtDataCollectInfo.u16BatteryAnaVol;
}

/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCTxOutVol_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCTxOutVol = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCTxOutVol_Get(void)
{
    return gtDataCollectInfo.i16FPCTxOutVol;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCTxOutAnaVol_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCTxOutAnaVol = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCTxOutAnaVol_Get(void)
{
    return gtDataCollectInfo.i16FPCTxOutAnaVol;
}

/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCRxOutVol_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCRxOutVol = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCRxOutVol_Get(void)
{
    return gtDataCollectInfo.i16FPCRxOutVol;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCRxOutAnaVol_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCRxOutAnaVol = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCRxOutAnaVol_Get(void)
{
    return gtDataCollectInfo.i16FPCRxOutAnaVol;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BLTempADC_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16BacklightTemperatureADC = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_BLTempADC_Get(void)
{
    return gtDataCollectInfo.i16BacklightTemperatureADC;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_PCBATempADC_Set(int16_t u16SetValue)
{
    gtDataCollectInfo.i16PCBATemperatureADC = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_PCBATempADC_Get(void)
{
    return gtDataCollectInfo.i16PCBATemperatureADC;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_SyncADC_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16SyncADC = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_SyncADC_Get(void)
{
    return gtDataCollectInfo.i16SyncADC;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BatteryADC_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16BatteryADC = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_BatteryADC_Get(void)
{
    return gtDataCollectInfo.i16BatteryADC;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_FPCTxOutADC_Set(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCTxOutADC_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCTxOutADC = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_FPCTxOutADC_Get(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCTxOutADC_Get(void)
{
    return gtDataCollectInfo.i16FPCTxOutADC;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_FPCRxOutADC_Set(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FPCRxOutADC_Set(int16_t i16SetValue)
{
    gtDataCollectInfo.i16FPCRxOutADC = i16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_FPCRxOutADC_Get(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
int16_t Memory_Pool_FPCRxOutADC_Get(void)
{
    return gtDataCollectInfo.i16FPCRxOutADC;
}

/******************************************************************************
 ;       Function Name			:	void Memory_Pool_VBattProtectState_Set(uint8_t u8SetValue)
 ;       Function Description	:	Set Battery voltage protection status
 ;       Parameters				:	uint8_t u8SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_VBattProtectState_Set(uint8_t u8SetValue)
{
    gtBattProtectInfo.u8BatteryProtectStatus = u8SetValue;
}

/******************************************************************************
 ;       Function Name			:	uint8_t Memory_Pool_VBattProtectState_Get(void)
 ;       Function Description	:	Get Battery voltage protection status
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t gtBattProtectInfo.u8BatteryProtectStatus
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_VBattProtectState_Get(void)
{
    return gtBattProtectInfo.u8BatteryProtectStatus;
}


/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_LEDDiagnosis_Set(uint64_t u64SetValue)
{
    gtDiagnosisInfo.u64LEDDriverDiagnosis = u64SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint64_t Memory_Pool_LEDDiagnosis_Get(void)
{
    return gtDiagnosisInfo.u64LEDDriverDiagnosis;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_GeneralDiagnosis_Set(uint16_t u16SetValue)
{
    gtDiagnosisInfo.u16GeneralDiagnosis = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_GeneralDiagnosis_Get(void)
{
    return gtDiagnosisInfo.u16GeneralDiagnosis;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_NT51926Diagnosis_Set(uint64_t u64SetValue)
{
    gtDiagnosisInfo.u64NT51926Diagnosis = u64SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint64_t Memory_Pool_NT51926Diagnosis_Get(void)
{
    return gtDiagnosisInfo.u64NT51926Diagnosis;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_NT51926FWDiagnosis_Set(uint8_t u8SetValue)
{
    gtDiagnosisInfo.u8NT51926FWDiagnosis = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_NT51926FWDiagnosis_Get(void)
{
    return gtDiagnosisInfo.u8NT51926FWDiagnosis;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_IcCommDiagnosis_Set(uint16_t u16SetValue)
{
	if(u16SetValue > 0U ) 
	{
		if(u16SetValue >= 65535U)
		{
			u16SetValue = 65535U;
		}
		else
		{/* Nothing */}
		M_DM_I2cMasterInit();
	}
	else
	{/* Nothing */}
	
    gtDiagnosisInfo.u16IcCommunicationDiagnosis = u16SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint16_t Memory_Pool_IcCommDiagnosis_Get(void)
{
#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	return gtDiagnosisSimulateInfo.u16IcCommunicationDiagnosis;
#else
    return gtDiagnosisInfo.u16IcCommunicationDiagnosis;
#endif
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayStatusBp_Set(uint32_t u32SetValue)
{
    gtDiagnosisInfo.u32DisplayStatusBp = u32SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint32_t Memory_Pool_DisplayStatusBp_Get(void)
{
    return gtDiagnosisInfo.u32DisplayStatusBp;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayStatus_Set(uint32_t u32SetValue)
{
    uint32_t u32TempRegVal = gtDiagnosisInfo.u32DisplayStatus;
    uint8_t u8TempRegVal = gtDiagnosisInfo.u8IntStatus;
    tdINTBIF *ptrINTB = NULL;
	ptrINTB = GetINTB_Instance();

    gtDiagnosisInfo.u32DisplayStatus = u32SetValue;    
    /* Compares display status if setting INT_ERR bit. */
    if(u32TempRegVal != gtDiagnosisInfo.u32DisplayStatus)
    {                    
       /* Set INT_ERR */
        gtDiagnosisInfo.u8IntStatus = gtDiagnosisInfo.u8IntStatus | (BIT_INT_ERR_POS);        
        
        /* Compares interrupt status if sending INTB. */
        if((u8TempRegVal & BIT_INT_ERR_POS) == 0U)
        {
			if(Memory_Pool_PowerStatus_Get() != POWER_OFF_READY)
			{
				/* Send INTB Strategy Control Msg. */
				ptrINTB->Trigger(aTRUE);
				//(void)Task_ChangeEvent(TYPE_COMMUNICATION, LEVEL4, EVENT_MESSAGE_START_INTB_STRATEGY);
			}
			else
			{/*Nothing*/}    
        }
		else
		{/*Nothing*/}
    }
	else
	{/*Nothing*/}	
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint32_t Memory_Pool_DisplayStatus_Get(void)
{
    return gtDiagnosisInfo.u32DisplayStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_ActualDisplayStatus_Set(uint32_t u32SetValue)
{
    gtDiagnosisInfo.u32ActualDisplayStatus = u32SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint32_t Memory_Pool_ActualDisplayStatus_Get(void)
{
    return gtDiagnosisInfo.u32ActualDisplayStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint32_t Memory_Pool_ReadAfterDisplayStatus_Get(void)
{
    return gtDiagnosisInfo.u32ReadAfterDisplayStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_ReadAfterDisplayStatus_Set(uint32_t u32SetValue)
{
    gtDiagnosisInfo.u32ReadAfterDisplayStatus = u32SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/

bool Memory_Pool_LockLoss_Get(void)
{
    return gtDiagnosisInfo.bLockLoss;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_LockLoss_Set(bool bSetValue)
{
    gtDiagnosisInfo.bLockLoss = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_DiagnosisEnable_Get(void)
{
    return gtDiagnosisInfo.bDiagnosisEnable;
}
/******************************************************************************
 ;       Function Name          :   void Main_I2cSlaveInit(void)
 ;       Function Description   :
 ;       Parameters             :   void
 ;       Return Values          :
 ;       Source ID              :
 ******************************************************************************/
void Memory_Pool_DiagnosisEnable_Set(bool bSetValue)
{
    gtDiagnosisInfo.bDiagnosisEnable = bSetValue;
}
/******************************************************************************
 ;       Function Name          :   void Main_I2cSlaveInit(void)
 ;       Function Description   :
 ;       Parameters             :   void
 ;       Return Values          :
 ;       Source ID              :
 ******************************************************************************/
bool Memory_Pool_TempProtectEn_Get(void)
{
    return gtTemperatureInfo.bTemperatureProtectEnable;
}
/******************************************************************************
 ;       Function Name          :   void Main_I2cSlaveInit(void)
 ;       Function Description   :
 ;       Parameters             :   void
 ;       Return Values          :
 ;       Source ID              :
 ******************************************************************************/
void Memory_Pool_TempProtectEn_Set(bool bSetValue)
{
    gtTemperatureInfo.bTemperatureProtectEnable = bSetValue;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_IntStatus_Set(uint8_t u8SetValue)
{
    gtDiagnosisInfo.u8IntStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_IntStatus_Get(void)
{
    return gtDiagnosisInfo.u8IntStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_SoftwareReset_Set(bool bSetValue)
{
    gtPowerManageInfo.bSoftwareReset = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_SoftwareReset_Get(void)
{
    return gtPowerManageInfo.bSoftwareReset;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_SyncStatus_Set(uint8_t u8SetValue)
{
    gtPowerManageInfo.u8SyncStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_SyncStatus_Get(void)
{
    return gtPowerManageInfo.u8SyncStatus;
}

#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_NT51926_Vcom_Set(uint16_t u16SetValue)
 ;       Function Description	:
 ;       Parameters				:	uint32_t u32SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_NT51926_Vcom_Set(uint32_t u32SetValue)
{
	gtDisplayManageInfo.u32NT51926_Vcom = u32SetValue;
}
/******************************************************************************
 ;       Function Name			:	uint32_t Memory_Pool_NT51926_Vcom_Get(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:	uint32_t gtDisplayManageInfo.u32NT51926_Vcom
 ;       Source ID				:
 ******************************************************************************/
uint32_t Memory_Pool_NT51926_Vcom_Get(void)
{
	return gtDisplayManageInfo.u32NT51926_Vcom; 
}		

int8_t Memory_Pool_NT51926_VGMA_Set(const uint8_t *pu8Input)	/* Jacky@220928, for VGMA*/
{
	int8_t i8Rt = 1;
	uint8_t u8Loop = 0U;

	if( NULL != pu8Input )
	{
		for( u8Loop = 0U ; u8Loop < 4U ; u8Loop++ )
		{
			gtDisplayManageInfo.u8NT51926VGMA[u8Loop] = *pu8Input;
			pu8Input++;
		}
	}
	else
	{
		i8Rt = -1; /* TODO: input buff fail */
	}

	return i8Rt;
}

int8_t Memory_Pool_NT51926_VGMA_Get(uint8_t *pu8Out)
{
	int8_t i8Rt = 0;
	uint8_t u8Loop = 0U;

	if( NULL != pu8Out)
	{
		for( u8Loop = 0U ; u8Loop < 4U ; u8Loop++ )
		{
			*pu8Out = gtDisplayManageInfo.u8NT51926VGMA[u8Loop];
			pu8Out++;
		}
		i8Rt = 1;
	}
	else
	{
		i8Rt = -1;
	}
	return i8Rt;
}
#endif
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_FactoryMode_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8FactoryMode = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_FactoryMode_Get(void)
{
    return gtDisplayManageInfo.u8FactoryMode;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_ScanStatus_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8ScanStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_ScanStatus_Get(void)
{
    return gtDisplayManageInfo.u8ScanStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_TouchStatus_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8TouchStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_TouchStatus_Get(void)
{
    return gtDisplayManageInfo.u8TouchStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_LcdStatus_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_LcdStatus_Get(void)
{
    return gtDisplayManageInfo.u8DisplayStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_BacklightEnable_Set(bool bSetValue)
{
    gtDisplayManageInfo.bBacklightEnable = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_BacklightEnable_Get(void)
{
    return gtDisplayManageInfo.bBacklightEnable;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_Shutdown_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayShutdown = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_Shutdown_Get(void)
{
    return gtDisplayManageInfo.u8DisplayShutdown;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayScan_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayScanning = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_DisplayScan_Get(void)
{
    return gtDisplayManageInfo.u8DisplayScanning;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_DisplayEnableBp_Set(uint8_t u8SetValue)
 ;       Function Description	:	Backup DisplayEnable_Set value
 ;       Parameters				:	uint8_t u8SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayEnableBp_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayEnableBackup = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	uint8_t Memory_Pool_DisplayEnableBp_Get(void)
 ;       Function Description	:	Get  DisplayEnable_Set Backup value
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t gtDisplayManageInfo.u8DisplayEnableBackup
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_DisplayEnableBp_Get(void)
{
    return gtDisplayManageInfo.u8DisplayEnableBackup;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_DisplayEnable_Reg_Set(uint8_t u8SetValue)
 ;       Function Description	:	Set DisplayEnable register value
 ;       Parameters				:	uint8_t u8SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayEnable_Reg_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayEnable = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_DisplayEnable_Reg_Get(uint8_t u8SetValue)
 ;       Function Description	:	Get DisplayEnable register value
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t gtDisplayManageInfo.u8DisplayEnable
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_DisplayEnable_Reg_Get(void)
{
    return gtDisplayManageInfo.u8DisplayEnable;
}
/******************************************************************************
 ;       Function Name			:	void Memory_Pool_DisplayEnable_Set(uint8_t u8SetValue)
 ;       Function Description	:	Set DisplayEnable value
 ;       Parameters				:	uint8_t u8SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayEnable_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8DisplayEnableSet = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	uint8_t Memory_Pool_DisplayEnable_Set(uint8_t u8SetValue)
 ;       Function Description	:	Get DisplayEnable value
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t gtDisplayManageInfo.u8DisplayEnableSet
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_DisplayEnable_Get(void)
{
    return gtDisplayManageInfo.u8DisplayEnableSet;
}

/******************************************************************************
 ;       Function Name			:	void Memory_Pool_DisplayBusy_Set(bool bSetValue)
 ;       Function Description	:	Set DisplayBusy value
 ;       Parameters				:	bool bSetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_DisplayBusy_Set(bool bSetValue)
{
    gtDisplayManageInfo.bDisplayBusySet = bSetValue;
}
/******************************************************************************
 ;       Function Name			:	bool Memory_Pool_DisplayBusy_Get(void)
 ;       Function Description	:	Get DisplayBusy value
 ;       Parameters				:	void
 ;       Return Values			:	bool gtDisplayManageInfo.bDisplayBusySet
 ;       Source ID				:
 ******************************************************************************/
bool Memory_Pool_DisplayBusy_Get(void)
{
    return gtDisplayManageInfo.bDisplayBusySet;
}


/******************************************************************************
 ;       Function Name			:	void Memory_Pool_LcdResetStatus_Set(uint8_t u8SetValue)
 ;       Function Description	:	Set LcdResetStatus value
 ;       Parameters				:	uint8_t u8SetValue
 ;       Return Values			:	void
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_LcdResetStatus_Set(uint8_t u8SetValue)
{
    gtDisplayManageInfo.u8LcdResetStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	uint8_t Memory_Pool_LcdResetStatus_Get(uint8_t u8SetValue)
 ;       Function Description	:	Get LcdResetStatus value
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t gtDisplayManageInfo.u8DisplayEnableSet
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_LcdResetStatus_Get(void)
{
    return gtDisplayManageInfo.u8LcdResetStatus;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_PowerStatus_Set(uint8_t u8SetValue)
{
    gtPowerManageInfo.u8PowerStatus = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_PowerStatus_Get(void)
{
    return gtPowerManageInfo.u8PowerStatus;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_PowerState_Set(uint8_t u8SetValue)
{
    gtPowerManageInfo.u8PowerState = u8SetValue;
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t Memory_Pool_PowerState_Get(void)
{
    return gtPowerManageInfo.u8PowerState;
}

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
/******************************************************************************
 ;       Function Name			:	tdiagnosis_simulate_def Memory_Pool_DiagnosisSimulateInfo_Get(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
tdiagnosis_simulate_def Memory_Pool_DiagnosisSimulateInfo_Get(void)
{
    return gtDiagnosisSimulateInfo;
}
#endif

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_Command_Info_Fetch(uint8_t *pDataBuffer, uint8_t *pLength)
{
	uint8_t u8Counter = 0U, u8loop = 0u, u8FetchLen;
	uint8_t u8Temp = 0U;


#if(BACKDOOR_ICDIAG_OPEN)
	uint8_t *u8I2CICDiagBuffer;
#endif

	switch (*pDataBuffer)
	{
		case CMD_DISPLAY_STATUS:
			*(pDataBuffer + 1U) = gtDiagnosisInfo.u32DisplayStatus & 0xFFU;
#if (FORD_I2CCOMV1P9)
			*(pDataBuffer + 2U) = (gtDiagnosisInfo.u32DisplayStatus >> 8U) & 0x07U;
#elif (FORD_SPSSV1P0 || FORD_SPSSV1P1)
			*(pDataBuffer + 2U) = (gtDiagnosisInfo.u32DisplayStatus >> 8U) & 0xC7U;
			*(pDataBuffer + 3U) = (gtDiagnosisInfo.u32DisplayStatus >> 16U) & 0x00;
#endif
			/* Updates previous display status. */
			gtDiagnosisInfo.u32DisplayStatusPreHostCommand = gtDiagnosisInfo.u32DisplayStatus;

			/* Clear INT_ERROR  */
			gtDiagnosisInfo.u8IntStatus = gtDiagnosisInfo.u8IntStatus & (~BIT_INT_ERR_POS);

			//Memory_Pool_DisplayStatus_Set((gtDiagnosisInfo.u32DisplayStatus&(~BIT_ALL_ERROR_POS))|Memory_Pool_ActualDisplayStatus_Get());

			*pLength = LEN_DISPLAY_STATUS + LEN_SUBADDRESS;
		break;

		case CMD_DISPLAY_IDENTIFICATION:
			*(pDataBuffer + 1U) = ctDisplayID.u8ID;
			*(pDataBuffer + 2U) = ctDisplayID.u8Subversion;
			*pLength = LEN_DISPLAY_IDENTIFICATION + LEN_SUBADDRESS;
		break;

		case CMD_BACKLIGHT_PWM:
			*(pDataBuffer + 1U) = gtBacklightInfo.u16TargetDuty & 0xFFU;
			*(pDataBuffer + 2U) = (gtBacklightInfo.u16TargetDuty >> 8U) & 0x03U;
			*pLength = LEN_BACKLIGHT_PWM + LEN_SUBADDRESS;
		break;

		case CMD_DISPLAY_SCANNING:
			*(pDataBuffer + 1U) = gtDisplayManageInfo.u8DisplayScanning & 0x03U;
			*pLength = LEN_DISPLAY_SCANNING + LEN_SUBADDRESS;
		break;

		case CMD_DISPLAY_ENABLE:
			*(pDataBuffer + 1U) = gtDisplayManageInfo.u8DisplayEnable & 0x03U;
			*pLength = LEN_DISPLAY_ENABLE + LEN_SUBADDRESS;
		break;

		case CMD_DISPLAY_SHUTDOWN:
			*(pDataBuffer + 1U) = gtDisplayManageInfo.u8DisplayShutdown & 0x01U;
			*pLength = LEN_DISPLAY_SHUTDOWN + LEN_SUBADDRESS;
		break;

		case CMD_INTERRUPT_STATUS:
			/* Cancels INTB strategy if it is at set-up time. */
			(void)Task_ChangeEvent(TYPE_COMMUNICATION, LEVEL4, EVENT_MESSAGE_CANCEL_INTB_STRATEGY);
			*(pDataBuffer + 1U) = gtDiagnosisInfo.u8IntStatus & 0x0FU;
#if (INT_TCH_LATCH)
			/* Clear INT_TCH */
			gtDiagnosisInfo.u8IntStatus &= (~BIT_INT_TCH_POS);
#endif
			*pLength = LEN_INTERRUPT_STATUS + LEN_SUBADDRESS;
		break;

		case CMD_LOCK_DELIVERY_ASSEMBLY:
			u8Temp = 0U;
			(void)memcpy(&u8Temp, &gtCommunicationInfo.tFPNDeliverystatusInfo, sizeof(gtCommunicationInfo.tFPNDeliverystatusInfo));
			*(pDataBuffer + 1U) = u8Temp & 0x07U;
			*pLength = LEN_LOCK_FPN + LEN_SUBADDRESS;

			gtCommunicationInfo.tFPNDeliverystatusInfo.INT_WRT = FPN_DISABLE;
			gtCommunicationInfo.tFPNDeliverystatusInfo.CKSUM_ERR = FPN_DISABLE;           
		break; 

		case CMD_DELIVERY_ASSEMBLY_DATA:
			for ( u8Counter = 0; u8Counter < 26U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8WriteFFPNDelivery[u8Counter];
			}
			*pLength = LEN_WRITE_DEL_SER_PART_NUMBER + LEN_SUBADDRESS;
		break;                

		case CMD_LOCK_SERIAL_NUMBER:
			u8Temp = 0U;
			(void)memcpy(&u8Temp, &gtCommunicationInfo.tFPNSerialstatusInfo, sizeof(gtCommunicationInfo.tFPNSerialstatusInfo));
			*(pDataBuffer + 1U) = u8Temp & 0x07U;
			*pLength = LEN_LOCK_FPN + LEN_SUBADDRESS;

			gtCommunicationInfo.tFPNSerialstatusInfo.INT_WRT = FPN_DISABLE;
			gtCommunicationInfo.tFPNSerialstatusInfo.CKSUM_ERR = FPN_DISABLE;
		break;

		case CMD_SERIAL_NUMBER_DATA:
			for ( u8Counter = 0; u8Counter < 26U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8WriteFPNSerial[u8Counter];
			}
			*pLength = LEN_WRITE_DEL_SER_PART_NUMBER + LEN_SUBADDRESS;
		break; 

		case CMD_LOCK_PRODUCTION_PHASE_BYTE:
			u8Temp = 0U;
			(void)memcpy(&u8Temp, &gtCommunicationInfo.tFPNSProductPhaseStatusInfo, sizeof(gtCommunicationInfo.tFPNSProductPhaseStatusInfo));
			*(pDataBuffer + 1U) = u8Temp & 0x03U;
			*pLength = LEN_LOCK_FPN + LEN_SUBADDRESS;

			gtCommunicationInfo.tFPNSProductPhaseStatusInfo.INT_WRT = FPN_DISABLE;
		break; 

		case CMD_PRODUCTION_PHASE_BYTE_DATA:
			u8Temp = gtCommunicationInfo.u8WriteProductionPhase;
			*(pDataBuffer + 1U) = u8Temp;
			*pLength = LEN_PRODUCTION_PHASE_BYTE + LEN_SUBADDRESS;
		break;          

		case CMD_CORE_ASSEMBLY:
			for ( u8Counter = 0; u8Counter < 25U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8FPNCore[u8Counter];
			}
			*pLength = LEN_CORE_ASSEMBLY + LEN_SUBADDRESS;
		break;

		case CMD_DELIVERY_ASSEMBLY:
			for ( u8Counter = 0; u8Counter < 25U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8FPNDelivery[u8Counter];
			}
			*pLength = LEN_DELIVERY_ASSEMBLY + LEN_SUBADDRESS;
		break;

		case CMD_SOFT_PART_NUMBER:
			for ( u8Counter = 0; u8Counter < 25U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8FPNSoft[u8Counter];
			}
			*pLength = LEN_SOFT_PART_NUMBER + LEN_SUBADDRESS;
		break;

		case CMD_SERIAL_NUMBER:
			for ( u8Counter = 0; u8Counter < 25U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8FPNSerial[u8Counter];
			}
			*pLength = LEN_SERIAL_NUMBER + LEN_SUBADDRESS;
		break;

		case CMD_MAIN_CALI_PART_NUMBER:
			for ( u8Counter = 0; u8Counter < 25U; u8Counter++ )
			{
				*(pDataBuffer + u8Counter + 1U) = gtCommunicationInfo.mu8FPNMain[u8Counter];
			}
			*pLength = LEN_MAIN_CALI_PART_NUMBER + LEN_SUBADDRESS;
		break;

		case CMD_DETIAL_DIAGNOSIS_GET:
			*(pDataBuffer + 1U) = gtDiagnosisInfo.u16GeneralDiagnosis & 0xFFU;
			*(pDataBuffer + 2U) = (gtDiagnosisInfo.u16GeneralDiagnosis >> 8U) & 0xFFU;

			*(pDataBuffer + 3U) = gtDiagnosisInfo.u64LEDDriverDiagnosis & 0xFFU;
			*(pDataBuffer + 4U) = (gtDiagnosisInfo.u64LEDDriverDiagnosis >> 8U) & 0xFFU;
			*(pDataBuffer + 5U) = (gtDiagnosisInfo.u64LEDDriverDiagnosis >> 16U) & 0xFFU;
			*(pDataBuffer + 6U) = (gtDiagnosisInfo.u64LEDDriverDiagnosis >> 24U) & 0xFFU;
			*(pDataBuffer + 7U) = (gtDiagnosisInfo.u64LEDDriverDiagnosis >> 32U) & 0xFFU;
			*(pDataBuffer + 8U) = (gtDiagnosisInfo.u64LEDDriverDiagnosis >> 40U) & 0xFFU;
			*(pDataBuffer + 9U) = gtDiagnosisInfo.u64NT51926Diagnosis & 0xFFU;
			*(pDataBuffer + 10U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 8U) & 0xFFU;
			*(pDataBuffer + 11U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 16U) & 0xFFU;
			*(pDataBuffer + 12U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 24U) & 0xFFU;
			*(pDataBuffer + 13U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 32U) & 0xFFU;
			*(pDataBuffer + 14U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 40U) & 0xFFU;
			*(pDataBuffer + 15U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 48U) & 0xFFU;
			*(pDataBuffer + 16U) = (gtDiagnosisInfo.u64NT51926Diagnosis >> 56U) & 0xFFU;			
			*(pDataBuffer + 17U) = gtDiagnosisInfo.u8NT51926FWDiagnosis & 0xFFU;
			*pLength = LEN_DETIAL_DIAGNOSIS + LEN_SUBADDRESS;
		break;

		case CMD_TEMPERATURE_GET:
			*(pDataBuffer + 1U) = gtDataCollectInfo.i16PCBATemperature;
			*(pDataBuffer + 2U) = gtDataCollectInfo.i16PCBATemperature >> 8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.i16BacklightTemperature;
			*(pDataBuffer + 4U) = gtDataCollectInfo.i16BacklightTemperature >> 8U;
//#if (BX726_TDDI_NT51926)
			*(pDataBuffer + 5U) = (uint8_t)MBacklightControl_GetBacklightState();
//#endif
			*pLength = LEN_TEMPERATURE_GET + LEN_SUBADDRESS;
		break;

		case CMD_VOLTAGE_GET:
			*(pDataBuffer + 1U) = gtDataCollectInfo.u16BatteryVol;
			*(pDataBuffer + 2U) = gtDataCollectInfo.u16BatteryVol >> 8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.i16BatteryADC;
			*(pDataBuffer + 4U) = gtDataCollectInfo.i16BatteryADC >> 8U;
			*pLength = LEN_VOLTAGE_GET + LEN_SUBADDRESS;
		break;

		case CMD_PRESENT_BACKLIGHT_PWM_GET:
			*(pDataBuffer + 1U) = gtBacklightInfo.u16PresentDuty & 0xFFU;
			*(pDataBuffer + 2U) = (gtBacklightInfo.u16PresentDuty >> 8U) & 0x03U;
			*pLength = LEN_PRESENT_BACKLIGHT_PWM_GET + LEN_SUBADDRESS;
		break;

		case CMD_MCU_VERSION_GET:
			/**/
#if (0)
			u8loop = 0U;
			u8FetchLen = strlen((char *)cmu8McuVersion);
			do{
				*pDataBuffer = cmu8McuVersion[u8loop];
				pDataBuffer++;
				u8loop++;
				u8FetchLen--;
			}while(u8FetchLen > 0U);
			*pLength = LEN_MCU_VERSION_GET + LEN_SUBADDRESS;
#else
			for(u8Counter = 0U; u8Counter < LEN_MCU_VERSION_GET; u8Counter++)
			{
				*(pDataBuffer + u8Counter + 1U) = cmu8McuVersion[u8Counter];
			}
			*pLength = LEN_MCU_VERSION_GET + LEN_SUBADDRESS;
#endif
		break;

		case CMD_JUMP_TO_BOOTLOADER:
			*(pDataBuffer + 0U) = 0xF4U;
			*(pDataBuffer + 1U) = 0x03U;
			*(pDataBuffer + 2U) = 0x43U;
			*pLength = LEN_LEN + LEN_UPDATE_REQUEST + LEN_SUBADDRESS;
		break;

		case CMD_FACTORY_MODE:
			*(pDataBuffer + 1U) = gtDisplayManageInfo.u8FactoryMode;
			*pLength = LEN_FACTORY_MODE + LEN_SUBADDRESS;
		break;

		case CMD_DERATING_ENABLE:
			u8Temp = 0U;
			if (gtBacklightInfo.bDeratingEnable == true)
			{
				u8Temp = u8Temp | BIT_DERATING_EN_POS;
			}
			else
			{
				u8Temp = u8Temp & ~BIT_DERATING_EN_POS;
			}
			*(pDataBuffer + 1U) = u8Temp;
			*pLength = LEN_DERATING_ENABLE + LEN_SUBADDRESS;
		break;

		case CMD_PRODUCTION_PHASE_BYTE_GET:
			*(pDataBuffer + 1U) = gtCommunicationInfo.u8ProductionPhase;
			*pLength = LEN_PRODUCTION_PHASE_BYTE + LEN_SUBADDRESS;
		break;

		case CMD_VCOM_VALUE_GET:
			*(pDataBuffer + 1U) = (uint8_t)(gtDisplayManageInfo.u32NT51926_Vcom & 0xFFU);
			*(pDataBuffer + 2U) = (uint8_t)((gtDisplayManageInfo.u32NT51926_Vcom >> 8U) & 0xFFU);
			*(pDataBuffer + 3U) = (uint8_t)((gtDisplayManageInfo.u32NT51926_Vcom >> 16U) & 0xFFU);
//#if (BX726_TDDI_NT51926)
			*(pDataBuffer + 4U) = (uint8_t)(gtDisplayManageInfo.u8NT51926VGMA[0U]);
			*(pDataBuffer + 5U) = (uint8_t)(gtDisplayManageInfo.u8NT51926VGMA[1U]);
			*(pDataBuffer + 6U) = (uint8_t)(gtDisplayManageInfo.u8NT51926VGMA[2U]);
			*(pDataBuffer + 7U) = (uint8_t)(gtDisplayManageInfo.u8NT51926VGMA[3U]);
//#endif
			*pLength = LEN_VCOM_VALUE_GET + LEN_SUBADDRESS;
			(void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL4, EVENT_MESSAGE_TDDI_VCOM);
		break;

		case CMD_PCBATEMPINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.i16PCBATemperature+40U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.i16PCBATemperature+40U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.u16TPCBAAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.u16TPCBAAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16PCBATemperatureADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16PCBATemperatureADC>>8U;
			*pLength = LEN_PCBATEMPERATURE_INFO + LEN_SUBADDRESS;
		break;

		case CMD_BLTEMPINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.i16BacklightTemperature+40U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.i16BacklightTemperature+40U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.u16TBacklightAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.u16TBacklightAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16BacklightTemperatureADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16BacklightTemperatureADC>>8U;
			*pLength = LEN_BACKLIGHTTEMPERATURE_INFO + LEN_SUBADDRESS;
		break;

		case CMD_BATTVOLINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.u16BatteryVol*1U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.u16BatteryVol*1U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.u16BatteryAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.u16BatteryAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16BatteryADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16BatteryADC>>8U;
			*pLength = LEN_BATTERYVOLTAGE_INFO + LEN_SUBADDRESS;
		break;

		case CMD_SYNCVOLINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.u16SyncVol*1U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.u16SyncVol*1U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.u16SyncAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.u16SyncAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16SyncADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16SyncADC>>8U;
			*pLength = LEN_SYNCVOLTAGE_INFO + LEN_SUBADDRESS;
		break;

		case CMD_FPCTXOUTVOLINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.i16FPCTxOutVol*1U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.i16FPCTxOutVol*1U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.i16FPCTxOutAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.i16FPCTxOutAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16FPCTxOutADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16FPCTxOutADC>>8U;
			*pLength = LEN_FPCTXOUTVOLTAGE_INFO + LEN_SUBADDRESS;
		break;

		case CMD_FPCRXOUTVOLINFO:
			*(pDataBuffer + 1U) = (gtDataCollectInfo.i16FPCRxOutVol*1U);
			*(pDataBuffer + 2U) = (gtDataCollectInfo.i16FPCRxOutVol*1U)>>8U;
			*(pDataBuffer + 3U) = gtDataCollectInfo.i16FPCRxOutAnaVol;
			*(pDataBuffer + 4U) = gtDataCollectInfo.i16FPCRxOutAnaVol>>8U;
			*(pDataBuffer + 5U) = gtDataCollectInfo.i16FPCRxOutADC;
			*(pDataBuffer + 6U) = gtDataCollectInfo.i16FPCRxOutADC>>8U;
			*pLength = LEN_FPCRXOUTVOLTAGE_INFO + LEN_SUBADDRESS;
		break;

#if(BACKDOOR_ICDIAG_OPEN)			
		case ICDIAG_CMD_READ:
			u8I2CICDiagBuffer = ICDIAG_GetRxBuffer();
			for(u8Counter = 0U; u8Counter<LEN_ICDIAG_INFO; u8Counter++)
			{
				//*(pDataBuffer + u8Counter + 1U) = u8I2CICDiagBuffer[u8Counter];
				*(pDataBuffer + u8Counter) = u8I2CICDiagBuffer[u8Counter];
			}					
			*pLength = LEN_ICDIAG_INFO + LEN_SUBADDRESS;
		break;
#endif

#if(BACKDOOR_WRITE_DERATINGDATA)
		case CMD_DERATING_LIMITED_TEMPERATURES_GET:
			/* Read Derating Limited Temperatures */
			TemperatureDerating_LimitedTemperature_Get((pDataBuffer + 1U), TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH);
			*pLength = TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH + LEN_SUBADDRESS;
		break;

		case CMD_DERATING_TABLE_GET:
			/* Read Derating Table */
			TemperatureDerating_DeratingTable_Get((pDataBuffer + 1U), TEMP_DERATING_TABLE_SIZE);
			*pLength = TEMP_DERATING_TABLE_SIZE + LEN_SUBADDRESS;
		break;
#endif

		default:
		/* Nothing */
		break;
	}
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Memory_Pool_Command_Info_Assign(uint8_t *pCmdBuffer)
{
	uint8_t u8Counter  = 0U;
	uint8_t u8Cmd = *pCmdBuffer;
	uint16_t u16DataTemp = 0U;
#if(BACKDOOR_ICDIAG_OPEN)
	uint32_t u32DataAddr = 0UL;
	uint8_t u8DiagBuf = 0U;
#endif 

	switch(u8Cmd)
	{
		case CMD_BACKLIGHT_PWM:
			u16DataTemp = *(pCmdBuffer + 2U) & 0x03U;
			u16DataTemp = (u16DataTemp << 8U) + *(pCmdBuffer + 1U);
			gtBacklightInfo.u16TargetDuty = u16DataTemp;
		break;

		case CMD_DISPLAY_SCANNING:
			gtDisplayManageInfo.u8DisplayScanning = *(pCmdBuffer + 1U) & 0x03U;
		break;

		case CMD_DISPLAY_ENABLE:
			if (Memory_Pool_DisplayBusy_Get() == false) //if display sequence isn't finished,won't change display status
			{
				gtDisplayManageInfo.u8DisplayEnable = *(pCmdBuffer + 1U) & 0x03U;
			}
		break;

		case CMD_DISPLAY_SHUTDOWN:
			gtDisplayManageInfo.u8DisplayShutdown = *(pCmdBuffer + 1U) & 0x01U;
		break;

		case CMD_FACTORY_MODE:
			gtDisplayManageInfo.u8FactoryMode = *(pCmdBuffer + 1U) & 0x07U;
		break;

		case CMD_DERATING_ENABLE:
			//Memory_Pool_PCBATemp_Set((uint16_t)(*(pCmdBuffer + 1U)));	//David Test de-rating function use 0xBA command input temperature.
			//gtBacklightInfo.bDeratingEnable = true;						//David Test de-rating function use 0xBA command input temperature.
			if ((*(pCmdBuffer + 1U) & BIT_DERATING_EN_POS) == BIT_DERATING_EN_POS)
			{
				gtBacklightInfo.bDeratingEnable = true;
			}
			else
			{
				gtBacklightInfo.bDeratingEnable = false;
			}
		break;

		case CMD_LOCK_DELIVERY_ASSEMBLY:
			gtCommunicationInfo.u8EppromIndex = CMD_DELIVERY_ASSEMBLY_DATA;
			gtCommunicationInfo.tFPNDeliverystatusInfo.WRT_ST = (*(pCmdBuffer + 1U) & BIT_WRT_ST_POS);
			gtCommunicationInfo.bWriteFPNDeliveryStatusReg = FPN_ENABLE;
		break;  

		case CMD_LOCK_SERIAL_NUMBER:
			gtCommunicationInfo.u8EppromIndex = CMD_SERIAL_NUMBER_DATA;
			gtCommunicationInfo.tFPNSerialstatusInfo.WRT_ST = (*(pCmdBuffer + 1U) & BIT_WRT_ST_POS);
			gtCommunicationInfo.bWriteSerNumPNStatusReg = FPN_ENABLE;
		break;

		case CMD_LOCK_PRODUCTION_PHASE_BYTE:
			gtCommunicationInfo.u8EppromIndex = CMD_PRODUCTION_PHASE_BYTE_DATA;
			gtCommunicationInfo.tFPNSProductPhaseStatusInfo.WRT_ST = (*(pCmdBuffer + 1U) & BIT_WRT_ST_POS);
			gtCommunicationInfo.bWriteProductPhasePNStatusReg = FPN_ENABLE;
		break;

		case CMD_DELIVERY_ASSEMBLY_DATA:
			gtCommunicationInfo.u8EppromIndex = u8Cmd;
			for ( u8Counter = 0; u8Counter < 26U; u8Counter++ )
			{
				gtCommunicationInfo.mu8WriteFFPNDelivery[u8Counter] = *(pCmdBuffer + u8Counter + 1U);
			}
			gtCommunicationInfo.bWriteFPNDeliveryStatusReg = FPN_ENABLE;
			gtCommunicationInfo.bWriteFPNDelivery = FPN_ENABLE;
		break;

		case CMD_SERIAL_NUMBER_DATA:
			gtCommunicationInfo.u8EppromIndex = u8Cmd;
			for ( u8Counter = 0; u8Counter < 26U; u8Counter++ )
			{
				gtCommunicationInfo.mu8WriteFPNSerial[u8Counter] = *(pCmdBuffer + u8Counter + 1U);
			}
			gtCommunicationInfo.bWriteSerNumPNStatusReg = FPN_ENABLE;
			gtCommunicationInfo.bWriteSerNumFPN = FPN_ENABLE;
		break;

		case CMD_PRODUCTION_PHASE_BYTE_DATA:
			gtCommunicationInfo.u8EppromIndex = u8Cmd;
			gtCommunicationInfo.u8WriteProductionPhase = *(pCmdBuffer + 1U);

			gtCommunicationInfo.bWriteProductPhasePNStatusReg = FPN_ENABLE;
			gtCommunicationInfo.bWriteProductPhaseFPN = FPN_ENABLE;
		break;

#if(BACKDOOR_ICDIAG_OPEN)
		case ICDIAG_CMD_ICFETCH:
			u32DataAddr=(uint32_t)((uint32_t)pCmdBuffer[4]|((uint32_t)pCmdBuffer[5]<<8U)|((uint32_t)pCmdBuffer[6]<<16U)|((uint32_t)pCmdBuffer[7]<<24U));
			ICDIAG_CmdTrigger(pCmdBuffer[0], pCmdBuffer[1], pCmdBuffer[2], pCmdBuffer[3], u32DataAddr, pCmdBuffer[8], &u8DiagBuf);
		break;

		case ICDIAG_CMD_ICCTRL:           
			u32DataAddr=(uint32_t)((uint32_t)pCmdBuffer[4]|((uint32_t)pCmdBuffer[5]<<8U)|((uint32_t)pCmdBuffer[6]<<16U)|((uint32_t)pCmdBuffer[7]<<24U));
			ICDIAG_CmdTrigger(pCmdBuffer[0], pCmdBuffer[1], pCmdBuffer[2], pCmdBuffer[3], u32DataAddr, pCmdBuffer[8], &pCmdBuffer[9]);
		break;
#endif

#if(BACKDOOR_WRITE_DERATINGDATA)
		case CMD_DERATING_DATAS_SET:
			TemperatureDerating_DeratingCalibrationData_Set(pCmdBuffer + 1U);
		break;
#endif

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
		case CMD_DIAGNOSIS_SIMULATE:
			gtDiagnosisSimulateInfo.u8DispFaultPinLevel = *(pCmdBuffer + 1U) & 0x01U;
			gtDiagnosisSimulateInfo.u8DispFaultStatus[0] = *(pCmdBuffer + 2U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[1] = *(pCmdBuffer + 3U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[2] = *(pCmdBuffer + 4U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[3] = *(pCmdBuffer + 5U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[4] = *(pCmdBuffer + 6U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[5] = *(pCmdBuffer + 7U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[6] = *(pCmdBuffer + 8U);
			gtDiagnosisSimulateInfo.u8DispFaultStatus[7] = *(pCmdBuffer + 9U);

			gtDiagnosisSimulateInfo.u8LedINTPinLevel = *(pCmdBuffer + 10U) & 0x01U;
			gtDiagnosisSimulateInfo.u8LedFaultStatus[0] = *(pCmdBuffer + 11U);
			gtDiagnosisSimulateInfo.u8LedFaultStatus[1] = *(pCmdBuffer + 12U);
			gtDiagnosisSimulateInfo.u8LedFaultStatus[2] = *(pCmdBuffer + 13U);
			gtDiagnosisSimulateInfo.u8LedFaultStatus[3] = *(pCmdBuffer + 14U);
			gtDiagnosisSimulateInfo.u8LedFaultStatus[4] = *(pCmdBuffer + 15U);
			gtDiagnosisSimulateInfo.u8LedFaultStatus[5] = *(pCmdBuffer + 16U);

			gtDiagnosisSimulateInfo.u8PG1V2PinLevel = *(pCmdBuffer + 17U) & 0x01U;
			gtDiagnosisSimulateInfo.u8PG3V3PinLevel = *(pCmdBuffer + 18U) & 0x01U;
			gtDiagnosisSimulateInfo.u8LockPinLevel = *(pCmdBuffer + 19U) & 0x01U;

			gtDiagnosisSimulateInfo.u16BatteryVol = (uint16_t)((*(pCmdBuffer + 20U))<<8U|(*(pCmdBuffer + 21U))<<0U);
			gtDiagnosisSimulateInfo.u16FPCTXVol = (uint16_t)((*(pCmdBuffer + 22U))<<8U|(*(pCmdBuffer + 23U))<<0U);
			gtDiagnosisSimulateInfo.u16FPCRXVol = (uint16_t)((*(pCmdBuffer + 24U))<<8U|(*(pCmdBuffer + 25U))<<0U);

			gtDiagnosisSimulateInfo.i16PCBATemperature = (int16_t)((*(pCmdBuffer + 26U))<<8U|(*(pCmdBuffer + 27U))<<0U);

			gtDiagnosisSimulateInfo.u16IcCommunicationDiagnosis = (uint16_t)((*(pCmdBuffer + 28U))<<8U|(*(pCmdBuffer + 29U))<<0U);
			gtDiagnosisSimulateInfo.u16FPCBLVol = (uint16_t)((*(pCmdBuffer + 30U))<<8U|(*(pCmdBuffer + 31U))<<0U);
		break;
#endif
		default:
		/* Nothing */
		break;
	}
}
