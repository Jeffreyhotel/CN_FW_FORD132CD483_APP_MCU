/* -- Includes -- */
#include "C_Display_Management.h"
#include "C_Power_Management.h"
#include "C_Communication.h"
#include "Memory_Pool.h"
#include "C_Diagnosis.h"
#include "M_DetectTCHAttn.h"
#include "M_INTB.h"
#include "M_BatteryProtect.h"
#include "M_BacklightControl.h"
#include "C_Diagnosis.h"
#include "FIDM_Config.h"

/* -- Marco Define -- */
#define ATTN_ASSERT		BIT0
#define ATTN_RELEASE	BIT1

/* -- Type Define -- */

/* -- Global Variables -- */
static tdisplay_manage_task_def tDisplayManageTask;
static tdisplay_ctrl_def tDisplayCtrl;


/* Jacky@20221213 for losing ATTN */
static uint8_t gu8ATTNST;
static uint8_t gu8HighLvCNT, gu8LowLvCNT;
static uint32_t gu32LVTrigger;
const static cy_stc_sysint_t gtdATTNCfg =
{
	.intrSrc = U301_TSC_ATTN_IRQ,	/* Interrupt source is U301_TSC_ATTN_PIN interrupt */
	.intrPriority = 3UL,    		/* Interrupt priority is 3 */
};
/* -- Local Functions -- */
static void C_Display_Management_CallbackTCHClickHandler(void);
static void C_Display_Management_CallbackTCHClickRelHandler(void);

// static void HW_TIMER_ISR(void)
// {
// 	uint32_t i32interrupts = Cy_TCPWM_GetInterruptStatus(HW_TIMER_HW, HW_TIMER_NUM);
// 	uint32_t u32IO = Cy_GPIO_Read(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);
// 	uint8_t u8DebounceRDY = false;

// 	if(Memory_Pool_DisplayEnable_Get() == DISPLAY_ON_TOUCH_ON)
// 	{
// 		if(0UL != (CY_TCPWM_INT_ON_TC & i32interrupts))
// 		{
// 			/* There is a pending Terminal Count interrupt */
// 			if( 1UL ==  u32IO)	// If ATTN pull HIGH
// 			{
// 				// gu8HighLvCNT++;
// 				// if( 1UL == gu8HighLvCNT )
// 				// {
// 					Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
// 					u8DebounceRDY = true;
// 					gu8ATTNST |= ATTN_RELEASE;
// 					C_Display_Management_CallbackTCHClickRelHandler();
// 					Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_FALLING);
// 				// }
// 				// else
// 				// {
// 				// 	if( CY_GPIO_INTR_RISING != gu32LVTrigger)
// 				// 	{
// 				// 		gu8HighLvCNT = 0U;
// 				// 		u8DebounceRDY = true;
// 				// 		//Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
// 				// 	}
// 				// 	else
// 				// 	{
// 				// 		; /* still high */
// 				// 	}
// 				// }
// 			}
// 			else if( 0UL == u32IO ) // If ATTN pull LOW
// 			{
// 				// gu8LowLvCNT++;
// 				// if( 2UL == gu8LowLvCNT )
// 				// {
// 					Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
// 					u8DebounceRDY = true;
// 					gu8ATTNST |= ATTN_ASSERT;
// 					C_Display_Management_CallbackTCHClickHandler();
// 					Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_RISING);
					
// 				// }
// 				// else
// 				// {
// 				// 	if( CY_GPIO_INTR_FALLING != gu32LVTrigger)
// 				// 	{
// 				// 		gu8LowLvCNT = 0U;
// 				// 		u8DebounceRDY = true;
// 				// 		//Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
// 				// 	}
// 				// 	else
// 				// 	{
// 				// 		; /* still high */
// 				// 	}
// 				// }
// 			}
// 			else
// 			{
// 				;
// 			}

// 			if( true == u8DebounceRDY)
// 			{
// 				/* Stop the counter */
// 				Cy_TCPWM_TriggerStopOrKill(HW_TIMER_HW, HW_TIMER_MASK);
// 				/* Disable the counter */
// 				Cy_TCPWM_Counter_Disable(HW_TIMER_HW, HW_TIMER_NUM);
// 			}
// 		}
// 	}

// 	if(0UL != (CY_TCPWM_INT_ON_CC & i32interrupts))
// 	{
// 		/* There is a pending Compare Count interrupt */

// 	}

// 	Cy_TCPWM_ClearInterrupt(HW_TIMER_HW, HW_TIMER_NUM, i32interrupts );
// }

void Timer_Module_Init(void)
{
	//uint32_t u32Result = CY_TCPWM_SUCCESS;

	/* hw timer config */
	const cy_stc_sysint_t hw_timer_irq_config =
	{
		.intrSrc = (IRQn_Type)HW_TIMER_IRQ, 	/* Interrupt source is Des slave 0 interrupt */
		.intrPriority = 3UL             		/* Interrupt priority is 3 */
	};



	/* initial hw timer */
	if (CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(HW_TIMER_HW, HW_TIMER_NUM, &HW_TIMER_config))
	{
		/* TODO: Handle possible errors */
	}
	else
	{
		Cy_TCPWM_Counter_Disable(HW_TIMER_HW, HW_TIMER_NUM);

		/* Check if the desired interrupt is enabled prior to triggering */
		if (0UL != (CY_TCPWM_INT_ON_TC & Cy_TCPWM_GetInterruptMask(HW_TIMER_HW, HW_TIMER_NUM)))
		{
			Cy_TCPWM_SetInterrupt(HW_TIMER_HW, HW_TIMER_NUM, CY_TCPWM_INT_ON_TC);
		}

		// u32Result = Cy_SysInt_Init(&hw_timer_irq_config, &HW_TIMER_ISR);
		// if(u32Result != CY_SYSINT_SUCCESS)
		// {
		// 	CY_ASSERT(0);
		// }

		NVIC_EnableIRQ((IRQn_Type) hw_timer_irq_config.intrSrc);
#if 0
		/* Enable the initialized counter */
		Cy_TCPWM_Counter_Enable(HW_TIMER_HW, HW_TIMER_NUM);
		/* Then start the counter */
		Cy_TCPWM_TriggerStart(HW_TIMER_HW, HW_TIMER_MASK);
#endif
	}


	return;
}

void U301_TSC_ATTN_ISR(void)
{
	uint32_t u32TriggerEdge = CY_GPIO_INTR_DISABLE;
	uint8_t u8StartDebounce = 0U;
	uint32_t u32TimerST = Cy_TCPWM_Counter_GetStatus(HW_TIMER_HW, HW_TIMER_NUM);
	
	/* Clears the triggered pin interrupt */
	Cy_GPIO_ClearInterrupt(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);
	NVIC_ClearPendingIRQ(gtdATTNCfg.intrSrc);

	u32TriggerEdge = Cy_GPIO_GetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN);

	if( CY_GPIO_INTR_RISING == u32TriggerEdge /*&& (Memory_Pool_TouchStatus_Get() == TOUCH_ON)*/ )
	{
		/* low to high */
		// gu32LVTrigger = CY_GPIO_INTR_RISING;
		// u8StartDebounce = 0U;
		// gu8HighLvCNT = 0U;
		// gu8ATTNST &= ~ATTN_RELEASE;
		//C_Display_Management_CallbackTCHClickHandler();
		C_Display_Management_CallbackTCHClickRelHandler();
		Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
		Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_FALLING);
	}
	else if( CY_GPIO_INTR_FALLING == u32TriggerEdge && (Memory_Pool_TouchStatus_Get() == TOUCH_ON) )
	{
		/* high to low */
		// gu32LVTrigger = CY_GPIO_INTR_FALLING;
		// u8StartDebounce = 0U;
		// gu8LowLvCNT = 0U;
		// gu8ATTNST &= ~ATTN_ASSERT;
		C_Display_Management_CallbackTCHClickHandler();
		//C_Display_Management_CallbackTCHClickRelHandler();
		Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);
		Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_RISING);
	}
	else if( CY_GPIO_INTR_BOTH == u32TriggerEdge)
	{
		/* Trigger both */
	}
	else
	{
		//HAL_UART_Printf("ATTN pin unknow\r\n"); /* should not be here */
	}

	// if( 1U == u8StartDebounce )
	// {
	// 	if( CY_TCPWM_COUNTER_STATUS_COUNTER_RUNNING == u32TimerST )
	// 	{
	// 		/* Stop the counter */
	// 		Cy_TCPWM_TriggerStopOrKill(HW_TIMER_HW, HW_TIMER_MASK);
	// 		/* Disable the counter */
	// 		Cy_TCPWM_Counter_Disable(HW_TIMER_HW, HW_TIMER_NUM);
	// 	}
	// 	else
	// 	{
	// 		;
	// 	}
	// 	/* Enable the initialized counter */
	// 	Cy_TCPWM_Counter_Enable(HW_TIMER_HW, HW_TIMER_NUM);
	// 	/* Then start the counter */
	// 	Cy_TCPWM_TriggerStart(HW_TIMER_HW, HW_TIMER_MASK);
	// }
	// else
	// {
	// 	; /* do nothing */
	// }
}
/* End of Jacky@20221213 for losing ATTN */



/**
 * @brief When Detected ATTN Module will Get TP Status,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA
 * 
 * @return MDetectTchAttn_TouchController_E Is Touch Controller Ready?
 * 
 */
static MDetectTchAttn_TouchController_E C_Display_Management_CallbackTCHState(void)
{
	uint8_t u8Return;
    if(Memory_Pool_TouchStatus_Get() != TOUCH_ON)
    {
        u8Return = TCH_CONTROLLER_NOTREADY;
    }
	else
	{
    	u8Return = TCH_CONTROLLER_READY;
	}
	return u8Return;
}
/**
 * @brief When SDM Detects The behavior of Clicking for TP,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackTCHClickHandler(void)
{
	tdINTBIF *ptrINTB = NULL;
	ptrINTB = GetINTB_Instance();
	uint8_t u8TempRegVal = Memory_Pool_IntStatus_Get();
    Memory_Pool_IntStatus_Set(Memory_Pool_IntStatus_Get() | BIT_INT_TCH_POS);
    
    /* Compares interrupt status if sending INTB. */
    if((u8TempRegVal & BIT_INT_TCH_POS) == 0U)
    {
		//if(Memory_Pool_PowerStatus_Get() != POWER_OFF_READY)
		//{
			/* Send INTB Strategy Control Msg. */
    		ptrINTB->Trigger(aTRUE);
			//(void)Task_ChangeEvent(TYPE_COMMUNICATION, LEVEL4, EVENT_MESSAGE_START_INTB_STRATEGY);
		//}
		//else
		//{/*Nothing*/}    
    }
	else
	{/*Nothing*/}
}
/**
 * @brief When SDM Detects The behavior of Leaving Click for TP,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackTCHClickRelHandler(void)
{
#if (!INT_TCH_LATCH)
    Memory_Pool_IntStatus_Set(Memory_Pool_IntStatus_Get() & ~BIT_INT_TCH_POS);
#endif
}
/**
 * @brief When SDM's Battery Voltage is at invaild reange,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackEnteringBatteryProtected(uint8_t u8BpStatus)
{
    /* Turns off backlight */
    Memory_Pool_BacklightEnable_Set(false);
	MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);

	/* Action => do shutdown2 sequence*/
	Memory_Pool_VBattProtectState_Set(BATT_PROTECT_ON);
	Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
	(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
	(void)u8BpStatus;
}
/**
 * @brief When SDM's Battery Voltage is from invaild to vaild,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackLeavingBatteryProtected(void)
{
	Memory_Pool_VBattProtectState_Set(BATT_PROTECT_OFF);
}

/**
 * @brief When SDM's Diagnosis detect error,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackEnteringDiagnosisProtected(void)
{
	/* Backup backlight status */
	tDisplayCtrl.bBacklightSetBackup = Memory_Pool_BacklightEnable_Get();
	
    /* Turns off backlight */
    Memory_Pool_BacklightEnable_Set(false);
	MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);
	/* Action => turn off the display, touch , back-light*/
	tDisplayCtrl.bDiagnosisProtect = true;
	(void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL4, EVENT_MESSAGE_DISPLAY_ENABLE);
}
/**
 * @brief When SDM's Diagnosis not detect error,
 * The Function will Be Called.
 * 
 * @details NA
 * 
 * @note NA 
 * 
 */
static void C_Display_Management_CallbackLeavingDiagnosisProtected(void)
{
	tDisplayCtrl.bDiagnosisProtect = false;
	tDisplayCtrl.bDiagnosisProtectLeve = true;	
	(void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL4, EVENT_MESSAGE_DISPLAY_ENABLE);
  /* NOTHING */
}

/******************************************************************************
 ;       Function Name			:	uint8_t C_Display_Sequence_Control(uint8_t u8DispCtrlState, uint8_t u8SetValue)
 ;       Function Description	:	This state will do display 
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static uint8_t C_Display_Sequence_Control(uint8_t u8DispCtrlState, uint8_t u8SetValue)
{	
	uint8_t u8ReturnStatus = DS_ACTION_NONE;
//#if (BX726_TDDI_NT51926)
	uint8_t u8NTVGMA[4U] = {0x00};
//#endif

	if(tDisplayCtrl.bPowerStartupEvent == true)
	{
		switch (u8SetValue)
		{
			case DISPLAY_OFF_TOUCH_OFF :
					if(u8DispCtrlState == DS_ACTION_NONE)
					{
						u8DispCtrlState = DS_ACTION_BACKLIGHT;
						/* Delay time and disable back-light function */
						MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);						
					}
					else
					{ /* Nothing */ }

					switch (u8DispCtrlState)
					{
						case DS_ACTION_BACKLIGHT:
							/* Disable backlight function */
							Memory_Pool_BacklightEnable_Set(false);				          
							M_DM_BacklightControl(tDisplayCtrl.bBacklightSet, Memory_Pool_LockLoss_Get());
											
							/* Set 0x00 BL_ST bit */				             
				            Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_BL_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_BL_ST_POS));			            
						
							u8ReturnStatus  = DS_ACTION_DISPLAY_CTRL;
							tDisplayManageTask.u16Timer1 = TIME_1ms;							

							break;
						case DS_ACTION_DISPLAY_CTRL:
							/* Disable LCD */
							Memory_Pool_LcdStatus_Set(M_DM_DisplayControl(Memory_Pool_LcdStatus_Get(), u8SetValue, Memory_Pool_LockLoss_Get()));	
				            
							if(Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE)
							{
								u8ReturnStatus  = DS_ACTION_DISPLAY_RESET;
								tDisplayManageTask.u16Timer1 = TIME_151ms;						
							}
							else
							{							
								/* Set 0x00 TSC_ST bit */            				
                				Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS));
								
								Memory_Pool_TouchStatus_Set(TOUCH_OFF);
								
								u8ReturnStatus	= DS_ACTION_DISPLAY_STATUS;							
								tDisplayManageTask.u16Timer1 = TIME_151ms;
							}
							break;							
						case DS_ACTION_DISPLAY_STATUS:													
							/* Set 0x00 DISP_ST bit */							
							if (M_DM_NT51926_Status_Get() == NT51925_STATUS_STANDY)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS));						
							}
							else
							{ /* Nothing */ }
							
							tDisplayManageTask.u16Timer1 = TIME_2ms; /* TIME_1000ms */
							u8ReturnStatus  = DS_ACTION_CTRL_PROTECT;
												
							break;

						case DS_ACTION_DISPLAY_RESET:						
							/* Set 0x00 DISP_ST bit */	
							Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS));

							/* Reset Display  */
							Memory_Pool_LcdResetStatus_Set(M_DM_LcdControl(Memory_Pool_LcdResetStatus_Get(),u8SetValue));
							u8ReturnStatus  = DS_ACTION_TOUCH_CTRL;
							tDisplayManageTask.u16Timer1 = TIME_11ms;
							
							break;
						case DS_ACTION_TOUCH_CTRL:
							/* Reset Touch */
							Memory_Pool_TouchStatus_Set(M_DM_TouchControl(Memory_Pool_TouchStatus_Get(), u8SetValue));
							
							/* Set 0x00 TSC_ST bit */            				
                			Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS));
							if(Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE)
							{
								Memory_Pool_PowerState_Set(OFF_POWER_STATE);
								Memory_Pool_PowerStatus_Set(POWER_OFF);
								(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
							}
							else
							{ /* Nothing */ }
													
							tDisplayManageTask.u16Timer1 = TIME_2ms; /* TIME_1000ms */
							u8ReturnStatus  = DS_ACTION_CTRL_PROTECT;
							break;
						case DS_ACTION_CTRL_PROTECT:
							/* t_PON time 1sec */					
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;

							break;							
						default:
							/* Nothing */
							break;
					}
					break;
			case DISPLAY_ON_TOUCH_OFF :
					tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
					u8ReturnStatus  = DS_ACTION_NONE;
					tDisplayManageTask.u16Timer1 = TIME_DISABLE;
					break;

			case DISPLAY_OFF_TOUCH_ON : 
					tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
					u8ReturnStatus  = DS_ACTION_NONE;
					tDisplayManageTask.u16Timer1 = TIME_DISABLE;				
					break;
					
			case DISPLAY_ON_TOUCH_ON :
					if(u8DispCtrlState == DS_ACTION_NONE)
					{
						u8DispCtrlState = DS_ACTION_DISPLAY_CTRL;
					}
					else
					{ /* Nothing */ }
								
					switch (u8DispCtrlState)
					{
						case DS_ACTION_DISPLAY_CTRL:
							/* Enable LCD */
							Memory_Pool_LcdResetStatus_Set(LCD_RESET_HIGH);
							Memory_Pool_LcdStatus_Set(M_DM_DisplayControl(Memory_Pool_LcdStatus_Get(), u8SetValue, Memory_Pool_LockLoss_Get()));				
							u8ReturnStatus  = DS_ACTION_DISPLAY_STATUS;          
							tDisplayManageTask.u16Timer1 = TIME_190ms;

							break;
						case DS_ACTION_DISPLAY_STATUS:
							/* Set 0x00 DISP_ST bit */							
							if ((Memory_Pool_LcdStatus_Get() == DISPLAY_ON) && (M_DM_NT51926_Status_Get() == NT51925_STATUS_NORMAL))
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_DISP_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_DISP_ST_POS));

								/* Read TDDI Vcom value */
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
								Memory_Pool_NT51926_Vcom_Set(M_DM_VCOM_Get());
#endif
//#if (BX726_TDDI_NT51926)
								if( true == M_DM_NT51926_VGAMMA_Get(u8NTVGMA) )
								{
									/* Read success */
									if( true == Memory_Pool_NT51926_VGMA_Set(u8NTVGMA) )
									{
										/* Memory_Pool set success */
									}
									else
									{
										;/* TODO:Memory_Pool set fail */
									}
								}
								else
								{
									;/* TODO:NT51926 VGAMA read fail */
								}
//#endif							
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS));
							} 							
							u8ReturnStatus  = DS_ACTION_BACKLIGHT;	         
							tDisplayManageTask.u16Timer1 = TIME_11ms;
						
							break;
						case DS_ACTION_BACKLIGHT:
							/* Enable backlight function */		          
							M_DM_BacklightControl(tDisplayCtrl.bBacklightSet, Memory_Pool_LockLoss_Get());
							/* Set 0x00 BL_ST bit */							
							if (tDisplayCtrl.bBacklightSet == true)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_BL_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_BL_ST_POS));
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & ~BIT_BL_ST_POS);
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_BL_ST_POS));
							}
							tDisplayManageTask.u16Timer1 = TIME_13ms;
							u8ReturnStatus  = DS_ACTION_BACKLIGHT_PWM;												

							break;
						case DS_ACTION_BACKLIGHT_PWM:
							/* Enable backlight PWM function */
							if (tDisplayCtrl.bBacklightSet == true)
							{								
								MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_ENABLE_DIMMING);
							}
							else
							{ /* Nothing */ }				            
						
							u8ReturnStatus  = DS_ACTION_TOUCH_STATUS;
							tDisplayManageTask.u16Timer1 = TIME_32ms;													

							break;							
						case DS_ACTION_TOUCH_STATUS:
							Memory_Pool_TouchStatus_Set(TOUCH_ON);
							/* Set 0x00 TSC_ST bit */							
							if (Memory_Pool_TouchStatus_Get() == TOUCH_ON)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_TSC_ST_POS));
								tDisplayManageTask.u16Timer2 = TIME_2ms;
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS));
							}						
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;												

							break;							
						default:
							/* Nothing */
							break;
					}
					break;					
			default:
					tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
					u8ReturnStatus  = DS_ACTION_NONE;
					tDisplayManageTask.u16Timer1 = TIME_DISABLE;
				break;
		}
	}
	else
	{
		switch (u8SetValue)
		{
			case DISPLAY_OFF_TOUCH_OFF :
					if(tDisplayCtrl.u8LastDisplayStatus != DISPLAY_OFF_TOUCH_OFF)
					{
						if((tDisplayCtrl.u8LastDisplayStatus == DISPLAY_ON_TOUCH_ON) || (Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE) || (tDisplayCtrl.bDiagnosisProtect == true))
						{
							if(u8DispCtrlState == DS_ACTION_NONE)
							{
								u8DispCtrlState = DS_ACTION_BACKLIGHT;
								/* Delay time and disable back-light function */
								MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);									
							}
							else
							{ /* Nothing */ }
						}
						else
						{
							u8DispCtrlState = DS_ACTION_NONE;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
						}
					}
					else
					{
						
						if(Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE)
						{
							/* Delay time and disable back-light function */
							MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);								
							if(u8DispCtrlState == DS_ACTION_NONE)
							{
								u8DispCtrlState = DS_ACTION_DISPLAY_RESET;
							}
							else
							{ /* Nothing */ }							
						}
						else
						{
							u8DispCtrlState = DS_ACTION_NONE;
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
						}
					}

					switch (u8DispCtrlState)
					{
						case DS_ACTION_BACKLIGHT:
							/* Disable backlight function */
							Memory_Pool_BacklightEnable_Set(false);				          
							M_DM_BacklightControl(tDisplayCtrl.bBacklightSet, Memory_Pool_LockLoss_Get());

							/* Set 0x00 BL_ST bit */					
				            Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_BL_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_BL_ST_POS));				            
						
							u8ReturnStatus  = DS_ACTION_DISPLAY_CTRL;
							tDisplayManageTask.u16Timer1 = TIME_1ms;							

							break;
						case DS_ACTION_DISPLAY_CTRL:
							/* Disable LCD */
							Memory_Pool_LcdStatus_Set(M_DM_DisplayControl(Memory_Pool_LcdStatus_Get(), u8SetValue, Memory_Pool_LockLoss_Get()));	

							/* Set 0x00 TSC_ST bit */							
							Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS));
							Memory_Pool_TouchStatus_Set(TOUCH_OFF);
							
							if(Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE)
							{
								tDisplayManageTask.u16Timer1 = TIME_151ms;
								u8ReturnStatus  = DS_ACTION_DISPLAY_RESET;
							}
							else
							{
								u8ReturnStatus	= DS_ACTION_DISPLAY_STATUS;							
								tDisplayManageTask.u16Timer1 = TIME_151ms;
							}													
					
							break;
						case DS_ACTION_DISPLAY_STATUS:
							/* Set 0x00 DISP_ST bit */	
							if (M_DM_NT51926_Status_Get() == NT51925_STATUS_STANDY)
							{
				            	Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS)); 
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS));
							}
							else
							{ /* Nothing */ }
							
							tDisplayManageTask.u16Timer1 = TIME_2ms;
							u8ReturnStatus  = DS_ACTION_CTRL_PROTECT;

							break;
						case DS_ACTION_DISPLAY_RESET:
							/* Set 0x00 DISP_ST bit */	
							Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS));
							Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS));
							
							/* Reset Display  */
							Memory_Pool_LcdResetStatus_Set(M_DM_LcdControl(Memory_Pool_LcdResetStatus_Get(),u8SetValue));
							u8ReturnStatus  = DS_ACTION_TOUCH_CTRL;
							tDisplayManageTask.u16Timer1 = TIME_11ms;
							
							break;
						case DS_ACTION_TOUCH_CTRL:
							/* Reset Touch */
							Memory_Pool_TouchStatus_Set(M_DM_TouchControl(Memory_Pool_TouchStatus_Get(), u8SetValue));
							
							if(Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE)
							{
								Memory_Pool_PowerState_Set(OFF_POWER_STATE);
								Memory_Pool_PowerStatus_Set(POWER_OFF);
								(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
							}
							else
							{ /* Nothing */ }

							tDisplayManageTask.u16Timer1 = TIME_2ms;
							u8ReturnStatus  = DS_ACTION_CTRL_PROTECT;

							break;
						case DS_ACTION_CTRL_PROTECT:
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;

							break;							
						default:
							/* Nothing */
							break;
					}
					break;
			case DISPLAY_ON_TOUCH_OFF :
					if(tDisplayCtrl.u8LastDisplayStatus != DISPLAY_ON_TOUCH_OFF)
					{	
						if(tDisplayCtrl.u8LastDisplayStatus == DISPLAY_ON_TOUCH_ON)
						{
							if(u8DispCtrlState == DS_ACTION_NONE)
							{
								u8DispCtrlState = DS_ACTION_TOUCH_STATUS;
							}
							else
							{ /* Nothing */}
						}
						else
						{
							u8DispCtrlState = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
							u8ReturnStatus  = DS_ACTION_NONE;						
						}					
					}
					else
					{
						u8DispCtrlState = DS_ACTION_NONE;
						tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
						tDisplayCtrl.bPowerStartupEvent =false;
						tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
						tDisplayManageTask.u16Timer1 = TIME_DISABLE;
						u8ReturnStatus  = DS_ACTION_NONE;						
					}
					
					switch (u8DispCtrlState)
					{
						case DS_ACTION_TOUCH_STATUS:
							/* Reset Touch */
							Memory_Pool_TouchStatus_Set(M_DM_TouchControl(Memory_Pool_TouchStatus_Get(), u8SetValue));
							
							/* Set 0x00 TSC_ST bit */							
							if (Memory_Pool_TouchStatus_Get() == TOUCH_ON)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_TSC_ST_POS)); 
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS)); 
							}
						
							u8ReturnStatus  = DS_ACTION_CTRL_PROTECT;
							tDisplayManageTask.u16Timer1 = TIME_5ms;
							
							break;
						case DS_ACTION_CTRL_PROTECT:
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
							u8ReturnStatus  = DS_ACTION_NONE;
														
							break;							
						default:
							/* Nothing */
							break;
					}

					break;
			case DISPLAY_OFF_TOUCH_ON : 
					u8ReturnStatus  = DS_ACTION_NONE;
					tDisplayManageTask.u16Timer1 = TIME_DISABLE;
					tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;	

					break;
			case DISPLAY_ON_TOUCH_ON :
					if(tDisplayCtrl.u8LastDisplayStatus != DISPLAY_ON_TOUCH_ON)
					{	
						if((tDisplayCtrl.u8LastDisplayStatus == DISPLAY_OFF_TOUCH_OFF) || (tDisplayCtrl.u8LastDisplayStatus == DISPLAY_ON_TOUCH_OFF))
						{
							if(u8DispCtrlState == DS_ACTION_NONE)
							{
								u8DispCtrlState = DS_ACTION_TOUCH_CTRL;
							}
							else
							{ /* Nothing */}
						}
						else
						{
							u8DispCtrlState = DS_ACTION_NONE;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;						
						}
					}
					else
					{
						u8DispCtrlState = DS_ACTION_NONE;
						tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
						tDisplayCtrl.bPowerStartupEvent =false;
						tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
						u8ReturnStatus  = DS_ACTION_NONE;
						tDisplayManageTask.u16Timer1 = TIME_DISABLE;						
					}

					switch (u8DispCtrlState)
					{
						case DS_ACTION_TOUCH_CTRL:
							/* Reset Touch */
							M_DM_TouchControl(Memory_Pool_TouchStatus_Get(), u8SetValue);
							if(tDisplayCtrl.u8LastDisplayStatus == DISPLAY_ON_TOUCH_OFF)
							{
								u8ReturnStatus  = DS_ACTION_TOUCH_STATUS;          
								tDisplayManageTask.u16Timer1 = TIME_246ms;
							}
							else
							{
								u8ReturnStatus  = DS_ACTION_DISPLAY_CTRL;          
								tDisplayManageTask.u16Timer1 = TIME_1ms;
							}

							break;
						case DS_ACTION_DISPLAY_CTRL:
							/* Enable LCD */
							Memory_Pool_LcdStatus_Set(M_DM_DisplayControl(Memory_Pool_LcdStatus_Get(), u8SetValue, Memory_Pool_LockLoss_Get()));				
							u8ReturnStatus  = DS_ACTION_DISPLAY_STATUS;          
							tDisplayManageTask.u16Timer1 = TIME_200ms;

							break;
						case DS_ACTION_DISPLAY_STATUS:
							/* Set 0x00 DISP_ST bit */
							if ((Memory_Pool_LcdStatus_Get() == DISPLAY_ON) && (M_DM_NT51926_Status_Get() == NT51925_STATUS_NORMAL))
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_DISP_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_DISP_ST_POS)); 
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_DISP_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_DISP_ST_POS)); 
							} 							
							u8ReturnStatus  = DS_ACTION_BACKLIGHT;	         
							tDisplayManageTask.u16Timer1 = TIME_11ms;
						
							break;
						case DS_ACTION_BACKLIGHT:
							/* Enable backlight function */		          
							M_DM_BacklightControl(tDisplayCtrl.bBacklightSet, Memory_Pool_LockLoss_Get());
							
							/* Set 0x00 BL_ST bit */
							if (tDisplayCtrl.bBacklightSet == true)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_BL_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_BL_ST_POS)); 
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_BL_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_BL_ST_POS)); 
							}						
							u8ReturnStatus  = DS_ACTION_BACKLIGHT_PWM;	         
							tDisplayManageTask.u16Timer1 = TIME_11ms;

							break;
						case DS_ACTION_BACKLIGHT_PWM:
							/* Enable backlight PWM function */
							if (tDisplayCtrl.bBacklightSet == true)
							{								
								MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_ENABLE_DIMMING);
							}
							else
							{ /* Nothing */ }				            
						
							u8ReturnStatus  = DS_ACTION_TOUCH_STATUS;
							tDisplayManageTask.u16Timer1 = TIME_34ms;													

							break;							
						case DS_ACTION_TOUCH_STATUS:
							Memory_Pool_TouchStatus_Set(TOUCH_ON);

							/* Set 0x00 TSC_ST bit */
							if (Memory_Pool_TouchStatus_Get() == TOUCH_ON)
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_TSC_ST_POS)); 
								tDisplayManageTask.u16Timer2 = TIME_2ms;
							}
							else
							{
								Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() & (~BIT_TSC_ST_POS));
								Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TSC_ST_POS)); 
							}
							tDisplayCtrl.u8LastDisplayStatus = u8SetValue;
							tDisplayCtrl.bPowerStartupEvent =false;
							tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
							u8ReturnStatus  = DS_ACTION_NONE;
							tDisplayManageTask.u16Timer1 = TIME_DISABLE;
														
							break;							
						default:
							/* Nothing */
							break;
					}
					break;					
			default:
				/* Nothing */
				break;
		}
	}
	return u8ReturnStatus;
}

/******************************************************************************
 ;       Function Name			:	static void C_Display_Management_ParaInit(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Display_Management_ParaInit(void)
{
    Memory_Pool_Shutdown_Set(0U);
    Memory_Pool_DisplayScan_Set(SCAN_VT_HL);
    Memory_Pool_DisplayEnable_Set(DISPLAY_OFF_TOUCH_OFF);
	Memory_Pool_VBattProtectState_Set(BATT_PROTECT_OFF);

    tDisplayManageTask.u16Timer1 = TIME_DISABLE;
    tDisplayManageTask.u16Timer2 = TIME_DISABLE;
	tDisplayManageTask.u16Timer3 = TIME_DISABLE;
    tDisplayManageTask.u32Timeout = TIME_DISABLE;

	tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_OFF;
	tDisplayCtrl.bPowerStartupEvent = false;
	tDisplayCtrl.u8LastDisplayStatus = DISPLAY_OFF_TOUCH_OFF;
	tDisplayCtrl.u8CurrentDisplaySet = DISPLAY_OFF_TOUCH_OFF;
	tDisplayCtrl.bBacklightSet = false;
	tDisplayCtrl.bDiagnosisProtectLeve = false;
	tDisplayCtrl.bDiagnosisProtect = false;
	tDisplayCtrl.bBacklightSetBackup = false;
}
/******************************************************************************
 ;       Function Name			:	static void C_Display_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Display_Manage_Init(void)
{
    switch (Task_Current_Event_Get())
    {
        case EVENT_FIRST :
            C_Display_Management_ParaInit();

			tDisplayManageTask.u16Timer1 = TIME_2ms;

            break;
        case EVENT_I2C_MCU_INITIAL_DELAY :
            /* Check power ready. */
            if (Memory_Pool_PowerState_Get() == NORMAL_RUN_STATE)
            {
            	/* Power is ready and start I2C Master bus Initialize */
                M_DM_I2cMasterInit();
				Memory_Pool_TouchStatus_Set(TOUCH_OFF);
				Memory_Pool_LcdResetStatus_Set(LCD_RESET_HIGH);
				tDisplayCtrl.bPowerStartupEvent = true;

                /* Registers Detect TCH ATTN Module. */
				/* Jacky@20221213 for losing ATTN */
#if 0
                MDetectTchAttn_Register(C_Display_Management_CallbackTCHState\
                ,M_DM_NT51926_ATTN_Read\
                ,C_Display_Management_CallbackTCHClickHandler\
                ,C_Display_Management_CallbackTCHClickRelHandler\
                ,ATTN_TRI_FALLING);
#endif
        		/* Initialize the interrupt with vector at U301_TSC_ATTN_ISR() */
                gu8HighLvCNT = 0U;
                gu8LowLvCNT = 0U;
                gu8ATTNST = 0U;
				gu32LVTrigger = CY_GPIO_INTR_FALLING;
                //gu32LVTrigger = CY_GPIO_INTR_RISING;
                Timer_Module_Init();

                Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_FALLING);
				//Cy_GPIO_SetInterruptEdge(U301_TSC_ATTN_PORT, U301_TSC_ATTN_PIN, CY_GPIO_INTR_RISING);
        		Cy_SysInt_Init(&gtdATTNCfg, &U301_TSC_ATTN_ISR);
        		NVIC_EnableIRQ(gtdATTNCfg.intrSrc);
        		/* End of Jacky@20221213 for losing ATTN */

                /* Registers Two Callback Functions for Entering Battery Protected and Leaving Battery Protected. */
                M_BP_Callback_Register(C_Display_Management_CallbackEnteringBatteryProtected\
                ,C_Display_Management_CallbackLeavingBatteryProtected);

                /* Registers Two Callback Functions for Entering Diagnostic Protected and Leaving Diagnostic  Protected. */
                C_Diagnosis_Callback_Register(C_Display_Management_CallbackEnteringDiagnosisProtected\
				, C_Display_Management_CallbackLeavingDiagnosisProtected);

                tDisplayManageTask.u16Timer1 = TIME_DISABLE;

				/* Delay 100ms =100ms + eeprom read time 91us */
				tDisplayManageTask.u16Timer2 = TIME_100ms;
            }
            else
            {
				/* Delay 2ms and retry again. */
				tDisplayManageTask.u16Timer1 = TIME_2ms;
            }
            break;
        case EVENT_DISPLAY_INITIAL_DELAY :
	        /* Delay display control. */
#if(DEBUG_POWER_UP)			
			HAL_GPIO_Toggle( U301_INTB_IN_PORT,  U301_INTB_IN_PIN); 
#endif
	        tDisplayManageTask.u16Timer2 = TIME_DISABLE;
	        (void)Task_ChangeState(TYPE_DISPLAY_MANAGE, LEVEL5, STATE_DISPLAY_MANAGE_CTRL, Display_Manage_State_Machine[STATE_DISPLAY_MANAGE_CTRL]);

            break;
        default:
        	/* Nothing */
            break;
    }
    Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	static void C_Display_Manage_Control(void)
 ;       Function Description	:	This state will do power on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Display_Manage_Control(void)
{
	uint8_t u8Status = tDisplayCtrl.u8DispSeqStatus;
    uint32_t u32Temp=0U;
	uint32_t u32CommDisplayStatus=0U;

    switch (Task_Current_Event_Get())
    {
        case EVENT_FIRST :
			tDisplayManageTask.u16Timer3 = TIME_2ms;
            break;
        case EVENT_MESSAGE_DISPLAY_ENABLE :
			if((tDisplayCtrl.u8DisplayEnLock == DISP_SEQ_LOCK_OFF) && (Memory_Pool_PowerStatus_Get() != POWER_OFF) && (Memory_Pool_PowerStatus_Get() != POWER_OFF_READY))
			{	
				tDisplayCtrl.u8DisplayEnLock = DISP_SEQ_LOCK_ON;
				
				tDisplayCtrl.u8CurrentDisplaySet = Memory_Pool_DisplayEnable_Get();
				tDisplayCtrl.bBacklightSet = Memory_Pool_BacklightEnable_Get();
								
				if((Memory_Pool_PowerState_Get() == SHUTDOWN1OR2_STATE) || (Memory_Pool_SyncStatus_Get() == SYNC_DISABLE))
				{
					/* Action => do shutdown sequence*/
					MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_DISABLE_NODIMMNG);
					tDisplayCtrl.u8CurrentDisplaySet = DISPLAY_OFF_TOUCH_OFF;
					tDisplayCtrl.bBacklightSet = false;
					Memory_Pool_DisplayEnable_Set(DISPLAY_OFF_TOUCH_OFF);
					u8Status = DS_ACTION_NONE;
					u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
				}
				else if(tDisplayCtrl.bDiagnosisProtect == true )
				{	
					/* Action => turn off the display, touch , back-light*/
		            if (Memory_Pool_LcdStatus_Get() == DISPLAY_ON)
		            {
		                /* Backup the DisplayEnable status*/
		            	Memory_Pool_DisplayEnableBp_Set(Memory_Pool_DisplayEnable_Get());
		            }
					else
					{ /* Nothing */ }	
					Memory_Pool_DisplayEnable_Set(DISPLAY_OFF_TOUCH_OFF);
					tDisplayCtrl.u8CurrentDisplaySet = DISPLAY_OFF_TOUCH_OFF;
					tDisplayCtrl.bBacklightSet = false;
					u8Status = DS_ACTION_NONE;
					u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
					tDisplayCtrl.u8DispSeqStatus = u8Status;
				}
				else if (Memory_Pool_FactoryMode_Get() == OTP_MODE)
				{
					/* In OTP test mode, enable back-light */
					M_DM_BacklightControl(tDisplayCtrl.bBacklightSet, false);
					MBacklightControl_ExternalTurnOnOffBL(E_MBL_EXTERNAL_ENABLE_DIMMING);
				}
				else if (Memory_Pool_FactoryMode_Get() == TPT_MODE)
				{
					/* In OTP test mode, enable back-light */
					Memory_Pool_DisplayEnable_Set(DISPLAY_ON_TOUCH_ON);
					tDisplayCtrl.u8CurrentDisplaySet = DISPLAY_ON_TOUCH_ON;

					u8Status = DS_ACTION_NONE;
					u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
					tDisplayCtrl.u8DispSeqStatus = u8Status;
				}
				else if(tDisplayCtrl.bDiagnosisProtectLeve == true)
				{
					/* Action =>if not detect error, turn on the display, touch , back-light*/
					Memory_Pool_DisplayEnable_Set(Memory_Pool_DisplayEnableBp_Get());
					tDisplayCtrl.u8CurrentDisplaySet = Memory_Pool_DisplayEnable_Get();
					
					Memory_Pool_BacklightEnable_Set(tDisplayCtrl.bBacklightSetBackup);
					tDisplayCtrl.bBacklightSet = Memory_Pool_BacklightEnable_Get();						

					u8Status = DS_ACTION_NONE;
					u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
					tDisplayCtrl.u8DispSeqStatus = u8Status;					
					tDisplayCtrl.bDiagnosisProtectLeve = false; /* Recover disable */
					tDisplayCtrl.bBacklightSetBackup = false; /* Clear Recover setting */
				}
				else if(Memory_Pool_DisplayBusy_Get() == false)
				{
					u8Status = DS_ACTION_NONE;
					u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
					tDisplayCtrl.u8DispSeqStatus = u8Status;
					if(tDisplayCtrl.u8DispSeqStatus == DS_ACTION_NONE)
					{
						Memory_Pool_DisplayBusy_Set(false);
					}
					else
					{
						Memory_Pool_DisplayBusy_Set(true);
					}

				}
				else
				{/* Nothing */}
				
			}
			else
			{
				if((Memory_Pool_PowerStatus_Get() != POWER_OFF) && (Memory_Pool_PowerStatus_Get() != POWER_OFF_READY))
				{
					(void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL4, EVENT_MESSAGE_DISPLAY_ENABLE);
				}
				else
				{ /* Nothing */}
            }
			
            break;
        case EVENT_DISPLAY_CONTORL_DELAY :
			u8Status = C_Display_Sequence_Control(u8Status, tDisplayCtrl.u8CurrentDisplaySet);
			tDisplayCtrl.u8DispSeqStatus = u8Status;
			if(tDisplayCtrl.u8DispSeqStatus == DS_ACTION_NONE)
			{
				Memory_Pool_DisplayBusy_Set(false);
			}
			else
			{
				Memory_Pool_DisplayBusy_Set(true);
			}
			//HAL_UART_Printf("\r\nEVENT_DISPLAY_CONTORL_DELAY:%d\r\n",tDisplayCtrl.u8DispSeqStatus);

            break;
        case EVENT_MESSAGE_SCANNING :
			if ((Memory_Pool_LcdStatus_Get() == DISPLAY_ON) && (Memory_Pool_LcdResetStatus_Get() == LCD_RESET_HIGH))
			{
				Memory_Pool_ScanStatus_Set(M_DM_ScanningControl(Memory_Pool_ScanStatus_Get(), Memory_Pool_DisplayScan_Get()));
			}			
			else
			{ /* Nothing */}

            break;
        case EVENT_MESSAGE_TDDI_VCOM :
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
			if ((Memory_Pool_LcdStatus_Get() == DISPLAY_ON) && (Memory_Pool_LcdResetStatus_Get() == LCD_RESET_HIGH))
			{	
				Memory_Pool_NT51926_Vcom_Set(M_DM_VCOM_Get());
			}			
			else
			{ /* Nothing */}				
#endif
            break;

        case EVENT_TIME_ATTN_POLLING :
            tDisplayManageTask.u16Timer2 = TIME_2ms;
            MDetectTchAttn_Routine2ms();

        	break;
        case EVENT_TIME_INIT_POLLING :
			if(Memory_Pool_PowerStatus_Get() != POWER_OFF)
			{
				if(Memory_Pool_DisplayEnable_Get() == DISPLAY_ON_TOUCH_ON)
				{
	                u32CommDisplayStatus = Memory_Pool_DisplayStatus_Get();
					u32Temp = Memory_Pool_ActualDisplayStatus_Get();
                	Memory_Pool_DisplayStatus_Set(u32CommDisplayStatus | BIT_INIT_POS);
					Memory_Pool_ActualDisplayStatus_Set(u32Temp | BIT_INIT_POS);			
				}
				else
				{ /* Nothing */}
				tDisplayManageTask.u16Timer3 = TIME_2ms;
			}
			else
			{ /* Nothing */ }

			break;
			
        default:
        	(void)Task_ChangeState(TYPE_DISPLAY_MANAGE, LEVEL5, STATE_DISPLAY_MANAGE_ERROR, Display_Manage_State_Machine[STATE_DISPLAY_MANAGE_ERROR]);
            break;
    }
    Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	static void C_Display_Manage_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Display_Manage_Error(void)
{
    Task_TaskDone();
}

/* -- Global Functions -- */
/******************************************************************************
 ;       Function Name			:	void C_Display_Manage_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Display_Manage_Timer1(void)
{
    if (tDisplayManageTask.u16Timer1 > TIME_UP)
    {
        tDisplayManageTask.u16Timer1--;
        if (tDisplayManageTask.u16Timer1 == TIME_UP)
        {
            tDisplayManageTask.u16Timer1 = TIME_DISABLE;
            (void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL3, EVENT_TIMER1);
        }
		else
		{ /* Nothing */ }
    }
	else
	{ /* Nothing */ }
}
/******************************************************************************
 ;       Function Name			:	void C_Display_Manage_Timer2(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Display_Manage_Timer2(void)
{
    if (tDisplayManageTask.u16Timer2 > TIME_UP)
    {
        tDisplayManageTask.u16Timer2--;
        if (tDisplayManageTask.u16Timer2 == TIME_UP)
        {
            tDisplayManageTask.u16Timer2 = TIME_DISABLE;
            (void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL3, EVENT_TIMER2);
        }
		else
		{ /* Nothing */ }
    }
	else
	{ /* Nothing */ }
}
/******************************************************************************
 ;       Function Name			:	void C_Display_Manage_Timer3(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Display_Manage_Timer3(void)
{
    if (tDisplayManageTask.u16Timer3 > TIME_UP)
    {
        tDisplayManageTask.u16Timer3--;
        if (tDisplayManageTask.u16Timer3 == TIME_UP)
        {
            tDisplayManageTask.u16Timer3 = TIME_DISABLE;
            (void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL3, EVENT_TIMER3);
        }
		else
		{ /* Nothing */ }
    }
	else
	{ /* Nothing */ }
}

void (*const Display_Manage_State_Machine[MAX_DM_STATE_NO])(void) =
{   C_Display_Manage_Init, C_Display_Manage_Control, C_Display_Manage_Error };
/* -- END -- */

