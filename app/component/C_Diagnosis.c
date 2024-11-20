#include "C_Diagnosis.h"

#include "../Mymain.h"
#include "C_Display_Management.h"
#include "C_Power_Management.h"
#include "Memory_Pool.h"
#include "C_Communication.h"
#include "ICDiagApp.h"
#include "FIDM_Config.h"

static tdiagnosis_task_def tDiagnosisTask;
static tgpio_debounce_def tLedInt;
static tgpio_debounce_def tDispFaultMaster;
static tgpio_debounce_def tSerdesLock;
static tgpio_debounce_def tFpcTx;
static tgpio_debounce_def tFpcRx;
static tgpio_debounce_def tP1V2Good;
static tgpio_debounce_def tP3V3Good;
static tgpio_debounce_def tIcComm;
static tgpio_debounce_def tFpcBL;

CALLBACK_DIAG_ACTION_PROTECT CallbackDiagActionProtect=NULL;
CALLBACK_DIAG_ACTION_RECOVER CallbackDiagActionRecover=NULL;

tdiagnosis_ctrl_def tDiagCtrl;

/*******************************************************************************************
 ;       Function Name			:	static void C_Diagnosis_IO_LedInt(uint16_t u16RoutineTime)
 ;       Function Description	:	This function will do LP8864 diagnosis
 ;       Parameters				:	uint16_t u16RoutineTime
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************************/
static void C_Diagnosis_IO_LedInt(uint16_t u16RoutineTime)
{
 	uint64_t u64LEDDiagnosis=0UL;
	//uint64_t u64LED;
	//u64LED = M_GPIOSense_LED_Driver_Diagnosis();

	/* Check diagnosis is enable or not*/
	if(tLedInt.blEnable == true)
	{
	   if (false == tDiagCtrl.bFirstLedIntDebounceFinsh)
       {
           // write 29 11 and delay 300ms ,then BL_EN pull high.When BL_EN is high,it could clear error bit.
		   //Clear BSTOVPL status. When power on sequence, cause BSTOVPL will be set.
           tDiagCtrl.bFirstLedIntDebounceFinsh = M_GPIOSense_LED_Driver_Clear();
        }
		/* When LED_INT debounce 3 times, start to read LP8864 status and record error flags. */
		if(M_GPIOSense_LevelDeboucne(U301_LED_INT_PORT, U301_LED_INT_PIN, &tLedInt) == true)
		{
			if(tLedInt.u8NewGPIOStatus == GPIO_LOW)
			{
				tDiagCtrl.u16LEDDriverCommTime+=u16RoutineTime;
				if(tDiagCtrl.u16LEDDriverCommTime >= C_DIAG_LP8864_I2CTIME)
				{
					tDiagCtrl.u16LEDDriverCommTime=0U;
					/* Read LED driver error message. */
					u64LEDDiagnosis = M_GPIOSense_LED_Driver_Diagnosis();

					if(u64LEDDiagnosis > 0UL)
					{
						tDiagCtrl.u8LEDDriverRegDebunce ++;
						tDiagCtrl.u8LEDDriverRegDebunce_RECOV = 0U;
						if(tDiagCtrl.u8LEDDriverRegDebunce >= C_DIAG_LP8864_REG_DEBUNCE)
						{
							tDiagCtrl.u8LEDDriverRegDebunce = C_DIAG_LP8864_REG_DEBUNCE;
							/* Record 0xA3 status */
							Memory_Pool_LEDDiagnosis_Set(u64LEDDiagnosis);					
						}
						else
						{/*Nothing*/}
					}
					else
					{
						tDiagCtrl.u8LEDDriverRegDebunce_RECOV ++;
						tDiagCtrl.u8LEDDriverRegDebunce = 0U;
						if(tDiagCtrl.u8LEDDriverRegDebunce_RECOV >= C_DIAG_LP8864_REG_DEBUNCE)
						{
							tDiagCtrl.u8LEDDriverRegDebunce_RECOV = C_DIAG_LP8864_REG_DEBUNCE;
							/* Record 0xA3 status */
							Memory_Pool_LEDDiagnosis_Set(u64LEDDiagnosis);
						}
						else
						{/*Nothing*/}
					}	
#if 0
					/* Clear corresponded registers to let LP8864 detect again. */
					M_GPIOSense_LED_Driver_DiagClear();
#endif
				}
				else
				{/*Nothing*/}
			}
			else if(tLedInt.u8NewGPIOStatus == GPIO_HIGH)
			{
				tDiagCtrl.u16LEDDriverCommTime=C_DIAG_LP8864_I2CTIME;
				tDiagCtrl.u8LEDDriverRegDebunce = 0U;
				tDiagCtrl.u8LEDDriverRegDebunce_RECOV = 0U;
			
					/* Record 0xA3 status */
				Memory_Pool_LEDDiagnosis_Set(0UL);
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
/*********************************************************************************************************
 ;       Function Name			:	static void C_Diagnosis_IO_DispFaultMaster(uint16_t u16RoutineTime)
 ;       Function Description	:	This function will do NT51926 diagnosis
 ;       Parameters				:	uint16_t u16RoutineTime
 ;       Return Values			:	void
 ;		Source ID				:
 *********************************************************************************************************/
static void C_Diagnosis_IO_DispFaultMaster(uint16_t u16RoutineTime)
{
	uint64_t u64Temp=0UL;
	uint64_t u64Diagnosis=0UL;
	
	/* Check diagnosis is enable or not*/
	if(tDispFaultMaster.blEnable == true)
	{
		/* When DISP_FAULT debounce 3 times, start to read NT51926 status and record error flags. */
		if(M_GPIOSense_LevelDeboucne(U301_DISP_FAULT_PORT, U301_DISP_FAULT_PIN, &tDispFaultMaster) == true)
		{
			if(tDispFaultMaster.u8NewGPIOStatus == GPIO_LOW)
			{
				tDiagCtrl.u16NT51926CommTime+=u16RoutineTime;
				if(tDiagCtrl.u16NT51926CommTime >= C_DIAG_NT51926_I2CTIME)
				{
					tDiagCtrl.u16NT51926CommTime = 0U;
					u64Temp=M_GPIOSense_DisplayFault_Read()&(BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_TYPEC_LCDERR_POS | BIT_A3_PANEL_TOUCHFAULT_ALL_POS);

					if((Memory_Pool_ScanStatus_Get() == SCAN_VB_HL) || (Memory_Pool_ScanStatus_Get() == SCAN_VB_HR))
					{
						u64Temp &= (~BIT_A3_PANEL_GATE_ERROR_POS);
					}
					else
					{/*Nothing*/}

					/*Debunce*/
					if((u64Temp & (BIT_A3_PANEL_DISPFAULT_TYPEB_POS)) > 0UL)
					{
						tDiagCtrl.u8NT51926DpRegDebunce_RECOV = 0U;
						tDiagCtrl.u8NT51926DpRegDebunce ++;
					}
					else
					{
						tDiagCtrl.u8NT51926DpRegDebunce = 0U;
						tDiagCtrl.u8NT51926DpRegDebunce_RECOV ++;
					}
					if((u64Temp & (BIT_A3_PANEL_TYPEC_LCDERR_POS)) > 0UL)
					{
						tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV = 0U;
						tDiagCtrl.u8NT51926TypeCRegDebunce ++;
					}
					else
					{
						tDiagCtrl.u8NT51926TypeCRegDebunce = 0U;
						tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV ++;
					}

					if(u64Temp == 0UL)
					{
						tDiagCtrl.u8NT51926BLDDebunce_RECOV = 0U ;
						tDiagCtrl.u8NT51926BLDDebunce ++;
					}
					else
					{
					 	tDiagCtrl.u8NT51926BLDDebunce = 0U;
					 	tDiagCtrl.u8NT51926BLDDebunce_RECOV ++;
					}
					/*Debunce*/

					/*Type B error_LCDERR*/
					if(tDiagCtrl.u8NT51926DpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926DpRegDebunce = C_DIAG_NT51926_REG_DEBUNCE;
						u64Diagnosis |= (u64Temp & (BIT_A3_PANEL_DISPFAULT_TYPEB_POS));
					}
					else if(tDiagCtrl.u8NT51926DpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926DpRegDebunce_RECOV = C_DIAG_NT51926_REG_DEBUNCE;
						// u64Diagnosis &= ~(BIT_A3_PANEL_DISPFAULT_TYPEB_POS);
					}
					else
					{/*Nothing*/}
					/*Type B error_LCDERR*/

					/*Type C error_LCDERR*/
					if(tDiagCtrl.u8NT51926TypeCRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926TypeCRegDebunce = C_DIAG_NT51926_REG_DEBUNCE;
						u64Diagnosis |= (u64Temp & (BIT_A3_PANEL_TYPEC_LCDERR_POS));
					}
					else if(tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV = C_DIAG_NT51926_REG_DEBUNCE;
						// u64Diagnosis &= ~(BIT_A3_PANEL_TYPEC_LCDERR_POS);
					}
					else
					{/*Nothing*/}
					/*Type C error_LCDERR*/
					
					/*Type C error_BLD Fail*/
					if(tDiagCtrl.u8NT51926BLDDebunce >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926BLDDebunce = C_DIAG_NT51926_REG_DEBUNCE;
						Memory_Pool_NT51926FWDiagnosis_Set(Memory_Pool_NT51926FWDiagnosis_Get() | BIT_A3_PANEL_FW_BLD_ERROR_POS);
						// HAL_UART_Printf("[A3] FW_BLD_ERROR = 1");
						
						if((Memory_Pool_ScanStatus_Get() == SCAN_VB_HL) || (Memory_Pool_ScanStatus_Get() == SCAN_VB_HR))
						{
							Memory_Pool_NT51926FWDiagnosis_Set(Memory_Pool_NT51926FWDiagnosis_Get() & ~BIT_A3_PANEL_FW_BLD_ERROR_POS);
						}
						else
						{/*Nothing*/}
					}
					else if(tDiagCtrl.u8NT51926BLDDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					{
						tDiagCtrl.u8NT51926BLDDebunce_RECOV = C_DIAG_NT51926_REG_DEBUNCE;
						//Memory_Pool_NT51926FWDiagnosis_Set(Memory_Pool_NT51926FWDiagnosis_Get() & BIT_A3_PANEL_FW_BLD_ERROR_POS);
						// HAL_UART_Printf("[A3] FW_BLD_ERROR = 0");
					}
					else
					{/*Nothing*/}
					/*Type C error_BLD Fail*/

					/*Type C error_TSCERR*/
					if((Memory_Pool_DisplayStatus_Get() & BIT_TSC_ST_POS) == BIT_TSC_ST_POS)
					{
						if((u64Temp & BIT_A3_PANEL_TOUCHFAULT_ALL_POS) > 0UL)
						{
							tDiagCtrl.u8NT51926TpRegDebunce_RECOV = 0U;
							tDiagCtrl.u8NT51926TpRegDebunce ++;
						}
						else
						{
							tDiagCtrl.u8NT51926TpRegDebunce = 0U;
							tDiagCtrl.u8NT51926TpRegDebunce_RECOV ++;
						}
						if(tDiagCtrl.u8NT51926TpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)
						{
							tDiagCtrl.u8NT51926TpRegDebunce = C_DIAG_NT51926_REG_DEBUNCE;		
							u64Diagnosis |= (u64Temp & BIT_A3_PANEL_TOUCHFAULT_ALL_POS);
						}
						else if(tDiagCtrl.u8NT51926TpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
						{
							tDiagCtrl.u8NT51926TpRegDebunce_RECOV = C_DIAG_NT51926_REG_DEBUNCE;
							// u64Diagnosis &= ~BIT_A3_PANEL_TOUCHFAULT_ALL_POS;
						}
						else
						{/*Nothing*/}
					}
					else
					{/*Nothing*/}
					/*Type C error_TSCERR*/

					// if(u64Temp == 0UL)
					// {
					// 	Memory_Pool_NT51926Diagnosis_Set((Memory_Pool_NT51926Diagnosis_Get() & (BIT_A3_PANEL_DP_STATUS_POS)) | BIT_A3_PANEL_FW_BLD_ERROR_POS);
					// }
			
					// if((tDiagCtrl.u8NT51926DpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)\
					// 	|| 	(tDiagCtrl.u8NT51926DpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					// 	||	(tDiagCtrl.u8NT51926TypeCRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)\
					// 	||	(tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					// 	||	(tDiagCtrl.u8NT51926TpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)\
					// 	||	(tDiagCtrl.u8NT51926TpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)
					// 	)
					// {
					// 	Memory_Pool_NT51926Diagnosis_Set((Memory_Pool_NT51926Diagnosis_Get() & (BIT_A3_PANEL_DP_STATUS_POS)) | u64Diagnosis);
					// }
					// else
					// {/*Nothing*/}

					/*FILL IN ERROR*/					
					if((tDiagCtrl.u8NT51926DpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE))
					{
						Memory_Pool_NT51926Diagnosis_Set(((Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_DISPFAULT_TYPEB_POS))\
						| (u64Diagnosis & (BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_DP_STATUS_POS))) );
					}
					else
					{/*Nothing*/}
					if((tDiagCtrl.u8NT51926TypeCRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE))
					{
						Memory_Pool_NT51926Diagnosis_Set(((Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_TYPEC_LCDERR_POS))\
						| (u64Diagnosis & (BIT_A3_PANEL_TYPEC_LCDERR_POS | BIT_A3_PANEL_DP_STATUS_POS))) );
					}
					else
					{/*Nothing*/}		
					if((tDiagCtrl.u8NT51926TpRegDebunce >= C_DIAG_NT51926_REG_DEBUNCE)\
					 && ((Memory_Pool_DisplayStatus_Get() & BIT_TSC_ST_POS) == BIT_TSC_ST_POS))
					{
						Memory_Pool_NT51926Diagnosis_Set(((Memory_Pool_NT51926Diagnosis_Get() & ~(BIT_A3_PANEL_TOUCHFAULT_ALL_POS))\
						| (u64Diagnosis & (BIT_A3_PANEL_TOUCHFAULT_ALL_POS | BIT_A3_PANEL_DP_STATUS_POS))) );
					}
					else
					{/*Nothing*/}	
					if((tDiagCtrl.u8NT51926BLDDebunce >= C_DIAG_NT51926_REG_DEBUNCE))
					{
						Memory_Pool_NT51926Diagnosis_Set(((Memory_Pool_NT51926Diagnosis_Get()\
						& ~(BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_TYPEC_LCDERR_POS | BIT_A3_PANEL_TOUCHFAULT_ALL_POS))\
						| (u64Diagnosis & (BIT_A3_PANEL_DP_STATUS_POS))) );
					}
					else
					{/*Nothing*/}	
					/*FILL IN ERROR*/

					/*RECOVER ERROR*/
					if(tDiagCtrl.u8NT51926DpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)/*Recover type B*/
					{
						Memory_Pool_NT51926Diagnosis_Set((Memory_Pool_NT51926Diagnosis_Get()\
						& (BIT_A3_PANEL_DP_STATUS_POS | BIT_A3_PANEL_TYPEC_LCDERR_POS | BIT_A3_PANEL_TOUCHFAULT_ALL_POS)) );
					}
					else
					{/*Nothing*/}
					if(tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)/*Recover type C LCDERR*/
					{
						Memory_Pool_NT51926Diagnosis_Set((Memory_Pool_NT51926Diagnosis_Get()\
						& (BIT_A3_PANEL_DP_STATUS_POS | BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_TOUCHFAULT_ALL_POS)) );
					}
					else
					{/*Nothing*/}
					if((tDiagCtrl.u8NT51926TpRegDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE) \
					&& ((Memory_Pool_DisplayStatus_Get() & BIT_TSC_ST_POS) == BIT_TSC_ST_POS))/*Recover type C TSCERR*/
					{
						Memory_Pool_NT51926Diagnosis_Set((Memory_Pool_NT51926Diagnosis_Get()\
						& (BIT_A3_PANEL_DP_STATUS_POS | BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_TYPEC_LCDERR_POS)) );
					}
					else
					{/*Nothing*/}
					 if(tDiagCtrl.u8NT51926BLDDebunce_RECOV >= C_DIAG_NT51926_REG_DEBUNCE)/*Recover type C BLD*/
					 {
						Memory_Pool_NT51926FWDiagnosis_Set(Memory_Pool_NT51926FWDiagnosis_Get() & ~BIT_A3_PANEL_FW_BLD_ERROR_POS);
					 }
					/*RECOVER ERROR*/
				}
				else
				{/*Nothing*/}
			}
			else if(tDispFaultMaster.u8NewGPIOStatus == GPIO_HIGH)
			{
				tDiagCtrl.u16NT51926CommTime= C_DIAG_NT51926_I2CTIME;
				tDiagCtrl.u8NT51926DpRegDebunce = 0U;
				tDiagCtrl.u8NT51926DpRegDebunce_RECOV = 0U;
				tDiagCtrl.u8NT51926TypeCRegDebunce = 0U;
				tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV = 0U;
				tDiagCtrl.u8NT51926TpRegDebunce = 0U;
				tDiagCtrl.u8NT51926TpRegDebunce_RECOV = 0U;
				tDiagCtrl.u8NT51926BLDDebunce = 0U;
				tDiagCtrl.u8NT51926BLDDebunce_RECOV = 0U;

				if(Memory_Pool_LcdStatus_Get() == DISPLAY_ON)
				{
					/* Record 0xA3 status */
					Memory_Pool_NT51926Diagnosis_Set(Memory_Pool_NT51926Diagnosis_Get() & BIT_A3_PANEL_DP_STATUS_POS );
					Memory_Pool_NT51926FWDiagnosis_Set( Memory_Pool_NT51926FWDiagnosis_Get() & (~BIT_A3_PANEL_FW_BLD_ERROR_POS));
					//HAL_UART_Printf("u8DisplayFWDiagnosis:%02x\n",Memory_Pool_NT51926FWDiagnosis_Get());
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
	else
	{/*Nothing*/}
}
/**********************************************************************************
 ;       Function Name			:	static void C_Diagnosis_IO_SerdesLock(void)
 ;       Function Description	:	This function will do Serdes Lock pin diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 **********************************************************************************/
static void C_Diagnosis_IO_SerdesLock(void)
{
 	/* Check diagnosis is enable or not*/
	tdINTBIF *ptrINTB = NULL;
	ptrINTB = GetINTB_Instance();

	if(tSerdesLock.blEnable == true)
	{
		if(M_GPIOSense_LevelDeboucne(U301_LOCK_PORT, U301_LOCK_PIN, &tSerdesLock) == true)
		{
			if(tSerdesLock.u8NewGPIOStatus == GPIO_LOW)
			{
				/* Record loss lock status*/
				Memory_Pool_LockLoss_Set(true);

				/* Record 0xA3 status*/
				Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() | (BIT_A3_COMM_LOSS_ERROR_POS));
			}
			else if(tSerdesLock.u8NewGPIOStatus == GPIO_HIGH) 
			{
				/* Record loss lock status*/
				Memory_Pool_LockLoss_Set(false);
				
				if((Memory_Pool_GeneralDiagnosis_Get()&(BIT_A3_COMM_LOSS_ERROR_POS)) > 0U)
				{
					/* Compares interrupt status if sending INTB. */
					if((Memory_Pool_IntStatus_Get()&(BIT_INT_ERR_POS | BIT_INT_TCH_POS)) > 0U)
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
				
				/* Record 0xA3 status*/
				Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() & (~BIT_A3_COMM_LOSS_ERROR_POS));
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
 ;       Function Name			:	static void C_Diagnosis_Vol_FPCTx(void)
 ;       Function Description	:	This function will do Lcd FPC TX diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Vol_FPCTx(void)
{
    uint16_t u16Temp;
 	int16_t i16VolTemp=Memory_Pool_FPCTxOutVol_Get();
#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	i16VolTemp=Memory_Pool_DiagnosisSimulateInfo_Get().i16FPCTXVol;
#endif

	/* Check diagnosis is enable or not*/
	if(tFpcTx.blEnable == true)
	{
		/* Count Debounce. */
		if(i16VolTemp < DIAG_FPC_TX_DISCON_VOL)
		{
			tFpcTx.u8DebounceHigh=0U;
			if(tFpcTx.u8DebounceLow < tFpcTx.u8DebounceMax)
			{
				tFpcTx.u8DebounceLow+=1U;
			}
			else
			{/*Nothing*/}
		}
		else
		{
			tFpcTx.u8DebounceLow=0U;
			if(tFpcTx.u8DebounceHigh < tFpcTx.u8DebounceMax)
			{
				tFpcTx.u8DebounceHigh+=1U;
			}
			else
			{/*Nothing*/}
		}
	
		/* Action: Protect */
		if(tFpcTx.u8DebounceLow >= tFpcTx.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_PANEL_FPC_TX_L_ERROR_POS);
		}
		else
		{/*Nothing*/}

		/* Action: Release protect. */
		if(tFpcTx.u8DebounceHigh >= tFpcTx.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			u16Temp&=~BIT_A3_PANEL_FPC_TX_L_ERROR_POS;
			Memory_Pool_GeneralDiagnosis_Set(u16Temp);
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}
}
/******************************************************************************
 ;       Function Name			:	static void C_Diagnosis_Vol_FPCRx(void)
 ;       Function Description	:	This function will do Lcd FPC RX diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Vol_FPCRx(void)
{
    uint16_t u16Temp;
 	int16_t i16VolTemp=Memory_Pool_FPCRxOutVol_Get();
#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	i16VolTemp=Memory_Pool_DiagnosisSimulateInfo_Get().i16FPCRXVol;
#endif

	/* Check diagnosis is enable or not*/
	if(tFpcRx.blEnable == true)
	{
		/* Count Debounce. */
		if(i16VolTemp < DIAG_FPC_RX_DISCON_VOL)
		{
			tFpcRx.u8DebounceHigh=0U;
			if(tFpcRx.u8DebounceLow < tFpcRx.u8DebounceMax)
			{
				tFpcRx.u8DebounceLow+=1U;
			}
			else
			{/*Nothing*/}
		}
		else
		{
			if(tFpcRx.u8DebounceHigh < tFpcRx.u8DebounceMax)
			{
				tFpcRx.u8DebounceHigh+=1U;
			}
			else
			{/*Nothing*/}
			tFpcRx.u8DebounceLow=0U;
		}

		/* Action: Protect */
		if(tFpcRx.u8DebounceLow >= tFpcRx.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_PANEL_FPC_RX_R_ERROR_POS);
		}
		else
		{/*Nothing*/}

		/* Action: Release protect. */
		if(tFpcRx.u8DebounceHigh >= tFpcRx.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			u16Temp&=~BIT_A3_PANEL_FPC_RX_R_ERROR_POS;
			Memory_Pool_GeneralDiagnosis_Set(u16Temp);
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}
}
/********************************************************************************
 ;       Function Name			:	static void C_Diagnosis_Vol_FPC_BL(void)
 ;       Function Description	:	This function will do backlight FPC diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		 Source ID				:
 ********************************************************************************/
static void C_Diagnosis_Vol_FPC_BL(void)
{
    uint16_t u16Temp;
 	uint16_t u16VolTemp = Memory_Pool_BLTempAnaVol_Get();
#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	u16VolTemp=Memory_Pool_DiagnosisSimulateInfo_Get().u16FPCBLVol;
#endif

	/* Check diagnosis is enable or not*/
	if(tFpcBL.blEnable == true)
	{
		/* Count Debounce. */
		if(u16VolTemp > DIAG_FPC_BL_DISCON_VOL)
		{
			tFpcBL.u8DebounceLow=0U;
			if(tFpcBL.u8DebounceHigh < tFpcBL.u8DebounceMax)
			{
				tFpcBL.u8DebounceHigh+=1U;
			}
			else
			{/*Nothing*/}
		}
		else
		{
			if(tFpcBL.u8DebounceLow < tFpcBL.u8DebounceMax)
			{
				tFpcBL.u8DebounceLow+=1U;
			}
			else
			{/*Nothing*/}
			tFpcBL.u8DebounceHigh=0U;
		}

		/* Action: Protect */
		if(tFpcBL.u8DebounceHigh >= tFpcBL.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_BL_FPC_ERROR_POS);
		}
		else
		{/*Nothing*/}

		/* Action: Release protect. */
		if(tFpcBL.u8DebounceLow >= tFpcBL.u8DebounceMax)
		{
			/* Record 0xA3 Status */
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			u16Temp&=~BIT_A3_BL_FPC_ERROR_POS;
			Memory_Pool_GeneralDiagnosis_Set(u16Temp);
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}
}

/******************************************************************************
 ;       Function Name			:	static void C_Diagnosis_IO_P1V2Good(void)
 ;       Function Description	:	This function will do P1V2 PG pin diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_IO_P1V2Good(void)
{
    uint16_t u16Temp;

 	/* Check diagnosis is enable or not*/
	if(tP1V2Good.blEnable == true)
	{
		if(M_GPIOSense_LevelDeboucne(U301_P1V2_PGOOD_PORT, U301_P1V2_PGOOD_PIN, &tP1V2Good) == true)
		{
			if(tP1V2Good.u8NewGPIOStatus == GPIO_LOW)
			{
				/* Record 0xA3 status*/
				u16Temp = Memory_Pool_GeneralDiagnosis_Get();
				Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_P1V2_ERROR_POS);
			}
			else if(tP1V2Good.u8NewGPIOStatus == GPIO_HIGH)
			{
				/* Record 0xA3 status*/
				u16Temp = Memory_Pool_GeneralDiagnosis_Get();
				u16Temp&=~BIT_A3_POWER_P1V2_ERROR_POS;
				Memory_Pool_GeneralDiagnosis_Set(u16Temp);
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
 ;       Function Name			:	static void C_Diagnosis_IO_P3V3Good(void)
 ;       Function Description	:	This function will do P3V3 PG pin diagnosis
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_IO_P3V3Good(void)
{
    uint16_t u16Temp;

	/* Check diagnosis is enable or not*/
	if(tP3V3Good.blEnable == true)
	{
		if((M_GPIOSense_LevelDeboucne(U301_P3V3_PGOOD_PORT, U301_P3V3_PGOOD_PIN, &tP3V3Good) == true) && (tP3V3Good.u8NewGPIOStatus == GPIO_LOW))
		{
			/* Record 0xA3 status*/
			u16Temp = Memory_Pool_GeneralDiagnosis_Get();
			Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_P3V3_ERROR_POS);
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}
}
/******************************************************************************************************
 ;       Function Name			:	static void C_Diagnosis_IC_Communitation(uint16_t u16RoutineTime)
 ;       Function Description	:	This function will do NT51926 status and I2C R/W diagnosis
 ;       Parameters				:	uint16_t u16RoutineTime
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************************************/
static void C_Diagnosis_IC_Communitation(uint16_t u16RoutineTime)
{
	uint8_t u8Temp;
	uint64_t u64Temp = 0UL;
	/* Check diagnosis is enable or not*/
	if(tIcComm.blEnable == true)
	{
		tDiagCtrl.u16NT51926I2cCommTime+=u16RoutineTime;
		if(tDiagCtrl.u16NT51926I2cCommTime >= C_DIAG_NT51926_I2CTIME)
		{
			tDiagCtrl.u16NT51926I2cCommTime = 0U;
			
			u8Temp = M_GPIOSense_NT51926_Status_Get();

			/* If I2C master bus read error， Set u8Temp = 0x07U */
			if(Memory_Pool_LcdStatus_Get() == DISPLAY_ON)
			{
				if(u8Temp != NT51925_STATUS_NORMAL)
				{
					if(u8Temp != 0x07U)
					{
						tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV = 0U;
						tDiagCtrl.u8NT51926DpStatusOnDebunce ++;
					}
					else
					{/*Nothing*/}
				}
				else
				{
					tDiagCtrl.u8NT51926DpStatusOnDebunce = 0U;
					tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV ++;
				}
				
				if(tDiagCtrl.u8NT51926DpStatusOnDebunce >= C_DIAG_NT51926_STATE_DEBUNCE)
				{
					tDiagCtrl.u8NT51926DpStatusOnDebunce = C_DIAG_NT51926_STATE_DEBUNCE;
					u64Temp = Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_DP_STATUS_POS);	
					u64Temp |= (((uint64_t)u8Temp) << 48U);
				}
				else if(tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE)
				{
					tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV = C_DIAG_NT51926_STATE_DEBUNCE;
					u64Temp = Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_DP_STATUS_POS);	
					u64Temp |= (((uint64_t)u8Temp) << 48U);	
				}
				else
				{/*Nothing*/}
				
				if((tDiagCtrl.u8NT51926DpStatusOnDebunce >= C_DIAG_NT51926_STATE_DEBUNCE)\
					||	(tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE))
				{
					Memory_Pool_NT51926Diagnosis_Set(u64Temp);
				}
				else
				{/*Nothing*/}								
			}
			else if(Memory_Pool_LcdStatus_Get() == DISPLAY_OFF)
			{
				if(u8Temp != NT51925_STATUS_STANDY)
				{
					if(u8Temp != 0x07U)
					{
						tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV = 0U;
						tDiagCtrl.u8NT51926DpStatusOfDebunce ++;
					}
					else
					{/*Nothing*/}
				}
				else
				{
					tDiagCtrl.u8NT51926DpStatusOfDebunce = 0U;
					tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV ++;
				}
				
				if(tDiagCtrl.u8NT51926DpStatusOfDebunce >= C_DIAG_NT51926_STATE_DEBUNCE)
				{
					tDiagCtrl.u8NT51926DpStatusOfDebunce = C_DIAG_NT51926_STATE_DEBUNCE;
					u64Temp = Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_DP_STATUS_POS);	
					u64Temp |= (((uint64_t)u8Temp) << 48U);
				}
				else if(tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE)
				{
					tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV = C_DIAG_NT51926_STATE_DEBUNCE;
					u64Temp = Memory_Pool_NT51926Diagnosis_Get() & (~BIT_A3_PANEL_DP_STATUS_POS);	
					u64Temp |= (((uint64_t)u8Temp) << 48U);	
				}
				else
				{/*Nothing*/}
				
				if((tDiagCtrl.u8NT51926DpStatusOfDebunce >= C_DIAG_NT51926_STATE_DEBUNCE)\
					||	(tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE))
				{
					
					if(tDiagCtrl.u8NT51926DpStatusOnDebunce < C_DIAG_NT51926_STATE_DEBUNCE)
					{
						Memory_Pool_NT51926Diagnosis_Set(u64Temp);
					}
					else
					{/*Nothing*/}
				}
				else
				{/*Nothing*/}
												
			}
			else
			{/*Nothing*/}

			if(Memory_Pool_IcCommDiagnosis_Get() > 0U)
			{
				Memory_Pool_IcCommDiagnosis_Set(0U);
				tDiagCtrl.u8NT51926I2cDebunce ++;
				if(tDiagCtrl.u8NT51926I2cDebunce >= C_DIAG_NT51926_Comm_DEBUNCE)
				{
					tDiagCtrl.u8NT51926I2cDebunce = C_DIAG_NT51926_Comm_DEBUNCE;
					Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() | (BIT_A3_PANEL_NT51926_COMM_ERROR_POS));
				}			
				else
				{/*Nothing*/}				
			}
			else
			{
				tDiagCtrl.u8NT51926I2cDebunce = 0U;
			}
		}
	}
	else
	{
		tDiagCtrl.u8NT51926DpStatusOfDebunce = 0U;
		tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV = 0U;
		tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV = 0U;
		tDiagCtrl.u8NT51926DpStatusOnDebunce = 0U;
		tDiagCtrl.u8NT51926I2cDebunce = 0U;
	}
}

/******************************************************************************
 ;       Function Name			:	void C_Diagnosis_ParaInit(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_ParaInit(void)
{
	tDiagnosisTask.u16Timer1 = TIME_DISABLE;
	tDiagnosisTask.u16Timer2 = TIME_DISABLE;

	Memory_Pool_IntStatus_Set(0U);
	Memory_Pool_DisplayStatus_Set(0U);
	Memory_Pool_DisplayStatusBp_Set(0U);
	Memory_Pool_DiagnosisEnable_Set(false);
	Memory_Pool_LockLoss_Set(false);
	Memory_Pool_NT51926Diagnosis_Set(0UL);
	Memory_Pool_IcCommDiagnosis_Set(0U);
	Memory_Pool_LEDDiagnosis_Set(0UL);	
	Memory_Pool_GeneralDiagnosis_Set(0U);
	
	tDiagCtrl.DiagProtectAction = DIAG_ACTION_NONE;
	tDiagCtrl.u16LEDDriverCommTime = C_DIAG_LP8864_I2CTIME;
	tDiagCtrl.u16NT51926CommTime = C_DIAG_NT51926_I2CTIME;
	tDiagCtrl.u8NT51926DpRegDebunce = 0U;
	tDiagCtrl.u8NT51926DpRegDebunce_RECOV = 0U;	
	tDiagCtrl.u8NT51926TpRegDebunce = 0U;	
	tDiagCtrl.u8NT51926TpRegDebunce_RECOV = 0U;	
	tDiagCtrl.u8NT51926TypeCRegDebunce = 0U;	
	tDiagCtrl.u8NT51926TypeCRegDebunce_RECOV = 0U;
	tDiagCtrl.u8NT51926BLDDebunce = 0U;
	tDiagCtrl.u8NT51926BLDDebunce_RECOV = 0U;
	tDiagCtrl.u8LEDDriverRegDebunce = 0U;
	tDiagCtrl.u8LEDDriverRegDebunce_RECOV = 0U;
	tDiagCtrl.u16NT51926I2cCommTime = C_DIAG_NT51926_I2CTIME;
	tDiagCtrl.u8NT51926I2cDebunce = 0U;
	tDiagCtrl.u8NT51926DpStatusOnDebunce = 0U;	
	tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV = C_DIAG_NT51926_STATE_DEBUNCE;
	tDiagCtrl.u8NT51926DpStatusOfDebunce = 0U;
	tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV = C_DIAG_NT51926_STATE_DEBUNCE;	
	tDiagCtrl.bFirstLedIntDebounceFinsh = false;

	tLedInt.u8DebounceHigh = 0U;
	tLedInt.u8DebounceLow = 0U;
	tLedInt.u8NewGPIOStatus = GPIO_HIGH;
	tLedInt.u8CurrentGPIOStatus = GPIO_HIGH;
	tLedInt.u8DebounceMax = DEBOUNCE_3_TIMES;
	tLedInt.blEnable = false;

	tDispFaultMaster.u8DebounceHigh = 0U;
	tDispFaultMaster.u8DebounceLow = 0U;
	tDispFaultMaster.u8NewGPIOStatus = GPIO_HIGH;
	tDispFaultMaster.u8CurrentGPIOStatus = GPIO_HIGH;
	tDispFaultMaster.u8DebounceMax = DEBOUNCE_3_TIMES;
	tDispFaultMaster.blEnable = false;


	tSerdesLock.u8DebounceHigh = 0U;
	tSerdesLock.u8DebounceLow = 0U;
	tSerdesLock.u8NewGPIOStatus = GPIO_HIGH;
	tSerdesLock.u8CurrentGPIOStatus = GPIO_HIGH;
	tSerdesLock.u8DebounceMax = DEBOUNCE_4_TIMES;
	tSerdesLock.blEnable = false;

	tFpcTx.u8DebounceHigh = 0U;
	tFpcTx.u8DebounceLow = 0U;
	tFpcTx.u8NewGPIOStatus = GPIO_HIGH;
	tFpcTx.u8CurrentGPIOStatus = GPIO_HIGH;
	tFpcTx.u8DebounceMax = DEBOUNCE_3_TIMES;
	tFpcTx.blEnable = true;

	tFpcRx.u8DebounceHigh = 0U;
	tFpcRx.u8DebounceLow = 0U;
	tFpcRx.u8NewGPIOStatus = GPIO_HIGH;
	tFpcRx.u8CurrentGPIOStatus = GPIO_HIGH;
	tFpcRx.u8DebounceMax = DEBOUNCE_3_TIMES;
	tFpcRx.blEnable = true;

	tP1V2Good.u8DebounceHigh = 0U;
	tP1V2Good.u8DebounceLow = 0U;
	tP1V2Good.u8NewGPIOStatus = GPIO_HIGH;
	tP1V2Good.u8CurrentGPIOStatus = GPIO_HIGH;
	tP1V2Good.u8DebounceMax = DEBOUNCE_3_TIMES;
	tP1V2Good.blEnable = true;

	tP3V3Good.u8DebounceHigh = 0U;
	tP3V3Good.u8DebounceLow = 0U;
	tP3V3Good.u8NewGPIOStatus = GPIO_HIGH;
	tP3V3Good.u8CurrentGPIOStatus = GPIO_HIGH;
	tP3V3Good.u8DebounceMax = DEBOUNCE_3_TIMES;
	tP3V3Good.blEnable = true;

	tIcComm.u8DebounceHigh = 0U;
	tIcComm.u8DebounceLow = 0U;
	tIcComm.u8NewGPIOStatus = GPIO_HIGH;
	tIcComm.u8CurrentGPIOStatus = GPIO_HIGH;
	tIcComm.u8DebounceMax = DEBOUNCE_3_TIMES;
	tIcComm.blEnable = false;

	tFpcBL.u8DebounceHigh = 0U;
	tFpcBL.u8DebounceLow = 0U;
	tFpcBL.u8NewGPIOStatus = GPIO_HIGH;
	tFpcBL.u8CurrentGPIOStatus = GPIO_HIGH;
	tFpcBL.u8DebounceMax = DEBOUNCE_3_TIMES;
	tFpcBL.blEnable = false;
	
#if(BACKDOOR_ICDIAG_OPEN)
	ICDIAG_Initialize();
#endif	
}
/******************************************************************************
 ;       Function Name			:	void (void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Action(void)
{
	uint16_t u16GeneralDiagnosis=0U;
	uint32_t u32Temp=0U;
	uint32_t u32CommDisplayStatus=0U;
	uint64_t u64LEDDiagnosis=0UL;
	uint64_t u64DisplayDiagnosis=0UL;
	uint8_t u8DisplayFWDiagnosis=0UL;

	/* Read 0xA3 status. */
	u64LEDDiagnosis=Memory_Pool_LEDDiagnosis_Get();
	u16GeneralDiagnosis=Memory_Pool_GeneralDiagnosis_Get();
	u64DisplayDiagnosis=Memory_Pool_NT51926Diagnosis_Get();
	u8DisplayFWDiagnosis=Memory_Pool_NT51926FWDiagnosis_Get();

	/************************************************************************************************/
	u32Temp = Memory_Pool_ActualDisplayStatus_Get();
	/* Record 0x00. */
	if((u64DisplayDiagnosis&(BIT_A3_PANEL_DISPFAULT_TYPEB_POS | BIT_A3_PANEL_GATE_ERROR_POS | BIT_A3_PANEL_SOURCE_ERROR_POS | BIT_A3_PANEL_FLASH_CRC_ERROR_POS)) > 0UL)
	{
		u32Temp|=BIT_LCDERR_POS;
	}
	else
	{/*Nothing*/}
	
	if((tDiagCtrl.u8NT51926DpStatusOnDebunce >= C_DIAG_NT51926_STATE_DEBUNCE) || (tDiagCtrl.u8NT51926DpStatusOfDebunce >= C_DIAG_NT51926_STATE_DEBUNCE))
	{
		u32Temp|=BIT_LCDERR_POS;
	}
	else
	{/*Nothing*/}

	if((u64DisplayDiagnosis&BIT_A3_PANEL_TOUCHFAULT_ALL_POS) > 0UL)
	{
		u32Temp|=BIT_TSCERR_POS;
	}
	else
	{/*Nothing*/}

	if((u8DisplayFWDiagnosis&BIT_A3_PANEL_FW_BLD_ERROR_POS) > 0UL)
	{
		u32Temp|=BIT_TSCERR_POS;
	}
	else
	{/*Nothing*/}

	if((u16GeneralDiagnosis&(BIT_A3_PANEL_FPC_TX_L_ERROR_POS|BIT_A3_PANEL_FPC_RX_R_ERROR_POS)) > 0U)
	{
		u32Temp|=BIT_DCERR_POS;
	}
	else
	{/*Nothing*/}

	if((u16GeneralDiagnosis&(BIT_A3_POWER_P1V2_ERROR_POS | BIT_A3_COMM_LOSS_ERROR_POS)) > 0U)
	{
		u32Temp|=BIT_LLOSS_POS;
	}
	else
	{/*Nothing*/}

	if((u64LEDDiagnosis != 0U) || ((u16GeneralDiagnosis&(BIT_A3_BL_FPC_ERROR_POS)) > 0U))
	{
		u32Temp|=BIT_BLERR_POS;
	}
	else
	{/*Nothing*/}

	/************************************************************************************************/
	/* Release 0x00. */
	/* Release LLOSS */
	if((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS | BIT_A3_POWER_P1V2_ERROR_POS | BIT_A3_COMM_LOSS_ERROR_POS)) == 0U)
	{
		if((Memory_Pool_ReadAfterDisplayStatus_Get()&(BIT_LLOSS_POS)) == BIT_LLOSS_POS)
		{
			u32Temp&=~BIT_LLOSS_POS;
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/* Release BLERR */
	if((u64LEDDiagnosis == 0U)
		&& ((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS | BIT_A3_POWER_LOW_VOL_ERROR_POS | BIT_A3_POWER_HIGH_VOL_ERROR_POS | BIT_A3_BL_FPC_ERROR_POS)) == 0U))
	{
		if((Memory_Pool_ReadAfterDisplayStatus_Get() & (BIT_BLERR_POS)) == BIT_BLERR_POS)
		{
			u32Temp&=~BIT_BLERR_POS;
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/* Release DCERR */
	if((u16GeneralDiagnosis&(BIT_A3_PANEL_FPC_TX_L_ERROR_POS | BIT_A3_PANEL_FPC_RX_R_ERROR_POS | BIT_A3_POWER_P3V3_ERROR_POS)) == 0U)
	{
		if((Memory_Pool_ReadAfterDisplayStatus_Get() & (BIT_DCERR_POS)) == BIT_DCERR_POS)
		{
		    u32Temp&=~BIT_DCERR_POS;
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/* Release TSCERR, TCERR. */
	if(((u16GeneralDiagnosis&BIT_A3_POWER_P3V3_ERROR_POS) == 0U) 
	  && ((u64DisplayDiagnosis&BIT_A3_PANEL_TOUCHFAULT_ALL_POS) == 0UL) 
	  && ((u8DisplayFWDiagnosis&BIT_A3_PANEL_FW_BLD_ERROR_POS) == 0UL))
	{
		if((Memory_Pool_ReadAfterDisplayStatus_Get() & BIT_TSCERR_POS) == BIT_TSCERR_POS)
		{
			u32Temp&=~(BIT_TSCERR_POS);
		}
		else
		{/*Nothing*/}

		if((Memory_Pool_ReadAfterDisplayStatus_Get() & BIT_TCERR_POS) == BIT_TCERR_POS)
		{
			u32Temp&=~(BIT_TCERR_POS);
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/* Release LCDERR. */
	if(((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS )) == 0U)
		&& ((u64DisplayDiagnosis&(BIT_A3_PANEL_DISPFAULT_TYPEB_POS)) == 0UL)
		&& ((u64DisplayDiagnosis&(BIT_A3_PANEL_TYPEC_LCDERR_POS)) == 0UL) 
		&& (tDiagCtrl.u8NT51926DpStatusOnDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE) 
		&& (tDiagCtrl.u8NT51926DpStatusOfDebunce_RECOV >= C_DIAG_NT51926_STATE_DEBUNCE))
	{
		if((Memory_Pool_ReadAfterDisplayStatus_Get() & (BIT_LCDERR_POS)) == BIT_LCDERR_POS)
		{
		    u32Temp&=~BIT_LCDERR_POS;
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/************************************************************************************************/
	/* Action State Machine. */
	if(tDiagCtrl.DiagProtectAction == DIAG_ACTION_NONE) 
	{
		if((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS)) > 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_SHUTDOWN;
			Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
			(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
		}
		else if(((u16GeneralDiagnosis&(BIT_A3_POWER_P1V2_ERROR_POS)) > 0U)
			|| (u64LEDDiagnosis > 0UL))
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_DISPBL_OFF_RSTRQ;
			u32Temp |= BIT_RST_RQ_POS;
			if(CallbackDiagActionProtect != NULL)
			{
				CallbackDiagActionProtect();
			}
			else
			{/*Nothing*/}
		}
		else if((u64DisplayDiagnosis&BIT_A3_PANEL_DISPFAULT_TYPEB_POS) > 0UL)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_DISPBL_OFF_RSTRQ;
			u32Temp |= BIT_RST_RQ_POS;
			if(CallbackDiagActionProtect != NULL)
			{
				CallbackDiagActionProtect();
			}
			else
			{/*Nothing*/}
		}
		else if((tDiagCtrl.u8NT51926DpStatusOnDebunce >= C_DIAG_NT51926_STATE_DEBUNCE) || (tDiagCtrl.u8NT51926DpStatusOfDebunce >= C_DIAG_NT51926_STATE_DEBUNCE))
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_DISPBL_OFF_RSTRQ;
			u32Temp |= BIT_RST_RQ_POS;
			if(CallbackDiagActionProtect != NULL)
			{
				CallbackDiagActionProtect();
			}
			else
			{/*Nothing*/}
		}

		else if((u16GeneralDiagnosis&(BIT_A3_PANEL_FPC_TX_L_ERROR_POS | BIT_A3_PANEL_FPC_RX_R_ERROR_POS | BIT_A3_BL_FPC_ERROR_POS)) > 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_DISPBL_OFF_RSTRQ;
			u32Temp |= BIT_RST_RQ_POS;
			if(CallbackDiagActionProtect != NULL)
			{
				CallbackDiagActionProtect();
			}
			else
			{/*Nothing*/}
		}
		else if((u16GeneralDiagnosis&(BIT_A3_COMM_LOSS_ERROR_POS)) > 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_DISPBL_OFF_RSTRQ_RECOV;
			u32Temp |= BIT_RST_RQ_POS;
			if(CallbackDiagActionProtect != NULL)
			{
				CallbackDiagActionProtect();
			}
			else
			{/*Nothing*/}
		}
		else
		{/*Nothing*/}
	}
	else if(tDiagCtrl.DiagProtectAction == DIAG_ACTION_DISPBL_OFF_RSTRQ)
	{
		if((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS)) > 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_SHUTDOWN;
			Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
			(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
		}		
		else
		{/*Nothing*/}
	}

	else if(tDiagCtrl.DiagProtectAction == DIAG_ACTION_DISPBL_OFF_RSTRQ_RECOV)
	{
		if((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS)) > 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_SHUTDOWN;
			Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
			(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
		}		
		else if((u16GeneralDiagnosis&(BIT_A3_POWER_P3V3_ERROR_POS | BIT_A3_POWER_P1V2_ERROR_POS | BIT_A3_COMM_LOSS_ERROR_POS)) == 0U)
		{
			tDiagCtrl.DiagProtectAction=DIAG_ACTION_NONE;
			/* Recover */
			if(CallbackDiagActionRecover != NULL)
			{
				CallbackDiagActionRecover();
			}
			else
			{/*Nothing*/}
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	/* Read 0x00 status. */
	//u32CommDisplayStatus = Memory_Pool_DisplayStatus_Get();//latch bit change status when error is removed
	//u32CommDisplayStatus |= u32Temp;//latch bit change status when error is removed

	/* Set 0x00 related status. */
	Memory_Pool_DisplayStatus_Set(u32Temp);
	Memory_Pool_ActualDisplayStatus_Set(u32Temp);
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Init(void)
{
	switch (Task_Current_Event_Get())
	{
		case EVENT_FIRST :
			C_Diagnosis_ParaInit();
			tDiagnosisTask.u16Timer1 = TIME_5ms;
		break;
		
		case EVENT_TIME_INITIAL_DELAY :
			/* Check power ready and MCU is in the normal run mode */
			if(Memory_Pool_PowerState_Get() == NORMAL_RUN_STATE)
			{
				tDiagnosisTask.u16Timer1 = TIME_DISABLE;
 				(void)Task_ChangeState(TYPE_DIAGNOSIS, LEVEL5, STATE_DIAGNOSIS_CTRL, Diagnosis_State_Machine[STATE_DIAGNOSIS_CTRL]);
				Memory_Pool_DiagnosisEnable_Set(true);
				//uart_printf("Diagnosis Enable");
			}
			else
			{
				tDiagnosisTask.u16Timer1 = TIME_5ms;
				Memory_Pool_DiagnosisEnable_Set(false);
			}
		break;
			
		default:
			/*Nothing*/
		break;
	}
	Task_TaskDone();
}

/******************************************************************************
 ;       Function Name			:	void C_Power_Manager_Control(void)
 ;       Function Description	:	This state will do power on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Control(void)
{
	switch(Task_Current_Event_Get())
	{
		case EVENT_FIRST :
			tDiagnosisTask.u16Timer2 = TIME_1000ms;
#if(BACKDOOR_ICDIAG_OPEN)
			tDiagnosisTask.u16Timer1 = TIME_1000ms;
#endif
 		break;

		case EVENT_MESSAGE_DISANOSIS_ENABLE :
			if(Memory_Pool_DiagnosisEnable_Get() == false) 
			{
				tDiagnosisTask.u16Timer2 = TIME_DISABLE;
			}
			else
			{ /* Nothing */}
		break;

		case EVENT_TIME_DIAGNOSIS_POLLING :
			if(Memory_Pool_DiagnosisEnable_Get() == true)
			{
				if(Memory_Pool_LcdResetStatus_Get() == LCD_RESET_HIGH)
				{
					tIcComm.blEnable = true;					
					tDispFaultMaster.blEnable = true;
				}
				else
				{ 
					tDispFaultMaster.blEnable = false;
					tIcComm.blEnable = false;
				}
				
				if((Memory_Pool_DisplayStatus_Get() & BIT_BL_ST_POS) == BIT_BL_ST_POS)
				{
					tLedInt.blEnable = true;
				}				
				else
				{
					tLedInt.blEnable = false;
				}
				
				if ((Memory_Pool_DisplayEnable_Reg_Get() & BIT_DISP_EN_POS) == DISPLAY_ENABLE)
				{
					tSerdesLock.blEnable = true;
					tFpcBL.blEnable = true;
				}
				else{ /* Nothing */}
			
				C_Diagnosis_Vol_FPCTx();
				C_Diagnosis_Vol_FPCRx();
				C_Diagnosis_IO_P1V2Good();
				C_Diagnosis_IO_P3V3Good();
				C_Diagnosis_IO_SerdesLock();
				C_Diagnosis_Vol_FPC_BL();
#if(CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
				C_Diagnosis_IO_DispFaultMaster((uint16_t)(TIME_10ms)- 1U);
				C_Diagnosis_IC_Communitation((uint16_t)(TIME_10ms)- 1U);			
#endif
				C_Diagnosis_IO_LedInt((uint16_t)(TIME_10ms)- 1U);

			}
			else
			{/*Nothing*/}
			//Cy_WDT_ClearWatchdog();
			C_Diagnosis_Action();
			tDiagnosisTask.u16Timer2 = TIME_10ms;
		break;
		
#if(BACKDOOR_ICDIAG_OPEN)
		case EVENT_MESSAGE_ICDIAG:		
			ICDIAG_Main();
			tDiagnosisTask.u16Timer1 = TIME_10ms;
		break;			
#endif

		default:
			(void)Task_ChangeState(TYPE_DIAGNOSIS, LEVEL5, STATE_DIAGNOSIS_ERROR, Diagnosis_State_Machine[STATE_DIAGNOSIS_ERROR]);
		break;
	}
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Diagnosis_Error(void)
{
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Diagnosis_Timer1(void)
{
	if(tDiagnosisTask.u16Timer1 > TIME_UP)
	{
		tDiagnosisTask.u16Timer1--;
		if (tDiagnosisTask.u16Timer1 == TIME_UP)
		{
			tDiagnosisTask.u16Timer1 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_DIAGNOSIS, LEVEL3, EVENT_TIMER1);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Diagnosis_Timer2(void)
{
	if(tDiagnosisTask.u16Timer2 > TIME_UP)
	{
		tDiagnosisTask.u16Timer2--;
		if (tDiagnosisTask.u16Timer2 == TIME_UP)
		{
			tDiagnosisTask.u16Timer2 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_DIAGNOSIS, LEVEL3, EVENT_TIMER2);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}
/******************************************************************************
 ;       Function Name			:	
 ;       Function Description	:	
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Diagnosis_Callback_Register(CALLBACK_DIAG_ACTION_PROTECT CBDiagActionProtectReg, CALLBACK_DIAG_ACTION_RECOVER CBDiagActionRecoverReg)
{
	CallbackDiagActionProtect=CBDiagActionProtectReg;
	CallbackDiagActionRecover=CBDiagActionRecoverReg;
}

void (*const Diagnosis_State_Machine[MAX_DN_STATE_NO])(void) =
{
	C_Diagnosis_Init, C_Diagnosis_Control, C_Diagnosis_Error
};

