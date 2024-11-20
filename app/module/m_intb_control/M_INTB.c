
/* -- Includes -- */
#include "cybsp.h"
#include "FIDM_Config.h"
//#include "public_function_list.h"
//#include "Event_Group.h"
#include "M_INTB.h"



/* INTB interface */
static int8_t M_INTB_Init(uint8_t);
static int8_t M_INTB_StateMachine(uint8_t *);
static int8_t M_INTB_Trigger(uint8_t);

/* INTB private use(only in this c file) */
static int8_t M_INTB_TimerStart(const uint32_t cu32Period);

static uint8_t gu8TimeRDY = aFALSE;
static tdINTBCtl gtdINTBCtl;
static tdINTBIF gtdINTB =
{
	.Init = M_INTB_Init,
	.StateMachine = M_INTB_StateMachine,
	.Trigger = M_INTB_Trigger,
};




tdINTBIF* GetINTB_Instance(void)
{
	return &gtdINTB;
}
static void INTB_ISR(void)
{
	uint32_t i32interrupts = Cy_TCPWM_GetInterruptStatus(INTB_STRATEGY_HW, INTB_STRATEGY_NUM);
    //Cy_GPIO_Inv(TEST_PIN_PORT, TEST_PIN_PIN);

	if( aTRUE == Event_GroupSetBits(aEVENT_INTB, aINTB_TIME_RDY) )
	{
		/* set flag ready */
		;
	}
	else
	{
		//uart_printf(LIGHT_RED"ISRERR on set aINTB_TIME_RDY\r\n"NONE);
	}


	Cy_TCPWM_ClearInterrupt(INTB_STRATEGY_HW, INTB_STRATEGY_NUM, i32interrupts );
	Cy_TCPWM_TriggerStopOrKill(INTB_STRATEGY_HW, INTB_STRATEGY_MASK);
	UNUSED(i32interrupts);
}


static int8_t M_INTB_Init(uint8_t u8Init)
{
	int8_t i8Resut = aTRUE;
	uint32_t u32Result = CY_TCPWM_SUCCESS;

	/* hw timer config */
	const cy_stc_sysint_t intb_irq_config =
	{
		.intrSrc = (IRQn_Type)INTB_STRATEGY_IRQ, 	/* Interrupt source is Des slave 0 interrupt */
		.intrPriority = 3UL             		/* Interrupt priority is 3 */
	};


	gtdINTBCtl.u8State = INTB_IDLE;

	/**
	 *   NEED to modify
	 *   Set the timer period in milliseconds. To count N cycles, period should be
	 *   set to N-1.
	 */
	gtdINTBCtl.u32SetupTime = ((aSETUP_TIME * 16000U) - 1U); /**
															  * 12000U means 1ms period of timer module
															  * It needs to modify in different setting.
															  */
	gtdINTBCtl.u32HoldTime = ((aHOLD_TIME  * 16000U) - 1U);
	/* End of modification */


	/* initial INTB_STRATEGY timer */
	if (CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(INTB_STRATEGY_HW, INTB_STRATEGY_NUM, &INTB_STRATEGY_config))
	{
		/* TODO: Handle possible errors */
	}
	else
	{
		Cy_TCPWM_Counter_Disable(INTB_STRATEGY_HW, INTB_STRATEGY_NUM);


		/* Check if the desired interrupt is enabled prior to triggering */
		if (0UL != (CY_TCPWM_INT_ON_TC & Cy_TCPWM_GetInterruptMask(INTB_STRATEGY_HW, INTB_STRATEGY_NUM)))
		{
			Cy_TCPWM_SetInterrupt(INTB_STRATEGY_HW, INTB_STRATEGY_NUM, CY_TCPWM_INT_ON_TC);
		}

		u32Result = Cy_SysInt_Init(&intb_irq_config, &INTB_ISR);
		if(u32Result != CY_SYSINT_SUCCESS)
		{
			CY_ASSERT(0);
		}

		NVIC_EnableIRQ((IRQn_Type) intb_irq_config.intrSrc);

		/* Enable the initialized counter */
		Cy_TCPWM_Counter_Enable(INTB_STRATEGY_HW, INTB_STRATEGY_NUM);
		Cy_TCPWM_Counter_SetPeriod(INTB_STRATEGY_HW, INTB_STRATEGY_NUM, (gtdINTBCtl.u32SetupTime) );
		Cy_TCPWM_TriggerStart(INTB_STRATEGY_HW, INTB_STRATEGY_MASK);
	}
	return i8Resut;
}

static int8_t M_INTB_StateMachine(uint8_t *u8Input)
{
	int8_t i8Resut = aTRUE;
	uint32_t u32INTBFlag = 0UL;


	u32INTBFlag = Event_GroupWaitBits(aEVENT_INTB, aINTB_TIME_RDY, aNOTALL);
	if( aFALSE != (aINTB_TIME_RDY & u32INTBFlag) )
	{
		gu8TimeRDY = aTRUE;
		Event_GroupClearBits(aEVENT_INTB, aINTB_TIME_RDY);
	}
	else
	{
		;/* nothing to do */
	}


	if( aFALSE != (INTB_IDLE & gtdINTBCtl.u8State) )
	{
		if( aTRUE == gtdINTBCtl.bTrigger )
		{
			gtdINTBCtl.u8State = INTB_CHECK;
			//uart_printf("I to C\r\n");
		}
		else
		{
			;/* IDLE run */
		}
	}
	else if( aFALSE != (INTB_CHECK & gtdINTBCtl.u8State) )
	{
		if( aTRUE == gu8TimeRDY )
		{
			gu8TimeRDY = aFALSE;
			gtdINTBCtl.bTimerStart = aFALSE;
			gtdINTBCtl.u8State = INTB_ASSERT;
			//uart_printf("C to A\r\n");
		}
		else
		{
			gtdINTBCtl.u8State = INTB_WAIT;
			//uart_printf("C to W\r\n");
		}
	}
	else if( aFALSE != (INTB_WAIT & gtdINTBCtl.u8State) )
	{
		if( aTRUE == gu8TimeRDY )
		{
			gu8TimeRDY = aFALSE;
			gtdINTBCtl.bTimerStart = aFALSE;
			gtdINTBCtl.u8State = INTB_ASSERT;
			//uart_printf("W to A\r\n");
		}
		else
		{
			;
		}
	}
	else if( aFALSE != (INTB_ASSERT & gtdINTBCtl.u8State) )
	{
		if( aFALSE == gtdINTBCtl.bTimerStart )
		{
			Cy_GPIO_Clr( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);

			/* start the "hold time" timer */
			M_INTB_TimerStart(gtdINTBCtl.u32HoldTime);
			gtdINTBCtl.bTimerStart = aTRUE;
		}
		else
		{
			if( aTRUE == gu8TimeRDY )
			{
				gu8TimeRDY = aFALSE;
				gtdINTBCtl.bTimerStart = aFALSE;
				gtdINTBCtl.u8State = INTB_DEASSERT;
				//uart_printf("A to D\r\n");
			}
			else
			{
			}
		}
	}
	else if( aFALSE != (INTB_DEASSERT & gtdINTBCtl.u8State) )
	{
		Cy_GPIO_Set( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
		gtdINTBCtl.u8State = INTB_IDLE;
		//uart_printf("D to I\r\n");

		/* start the "setup time" timer */
		M_INTB_TimerStart(gtdINTBCtl.u32SetupTime);
		gtdINTBCtl.bTimerStart = aTRUE;
		gtdINTBCtl.bTrigger = aFALSE;
	}
	else
	{
		;
	}
	return i8Resut;
}

static int8_t M_INTB_Trigger(uint8_t u8Input)
{
	//HAL_UART_Printf("\r\nIN_ptrINTB_Trigger!\r\n");
	int8_t i8Resut = aTRUE;
	gtdINTBCtl.bTrigger = aTRUE;
	return i8Resut;
}

static int8_t M_INTB_TimerStart(const uint32_t cu32Period)
{
	Cy_TCPWM_TriggerStopOrKill(INTB_STRATEGY_HW, INTB_STRATEGY_MASK);
	Cy_TCPWM_Counter_SetPeriod(INTB_STRATEGY_HW, INTB_STRATEGY_NUM, cu32Period );
	Cy_TCPWM_TriggerStart(INTB_STRATEGY_HW, INTB_STRATEGY_MASK);

	return aTRUE;
}


#if (1)
/* -- Marco Define -- */
typedef enum
{
    INTB_SM_WAIT_REGISTER = 0U,
    INTB_SM_INIT,
    INTB_SM_WAIT_TRIGGER,
    INTB_SM_SETUP_TIME,
    INTB_SM_HOLD_TIME,
    INTB_SM_DEINIT,
}MINTB_StateMachine_E;

/* -- Type Define -- */
typedef struct
{
    MINTB_StateMachine_E eStateMachine;
    MINTB_InttruptType_E eInterruptType;
    MINTB_StrategyCtrl_E eStrategyCtrl;
    CALLBACK_SDM_INTB_DO_SET CallbackSDMIntbDoSet;
    uint16_t u16AssertedTimer;
    uint16_t u16DeAssertedTimer;
}MINTB_Control;

/* -- Global Variables -- */
static MINTB_Control mINTBControl = 
{
    .eStateMachine = INTB_SM_WAIT_REGISTER,
    .eInterruptType = INTB_INT_TYPE_DEFAULT,
    .eStrategyCtrl = STRATEGY_CTRL_CANCEL,
    .CallbackSDMIntbDoSet = NULL,
    .u16AssertedTimer = 0x0000U,
    .u16DeAssertedTimer = 0x0000U,
};

/* -- Local Functions -- */
/**
 * @brief Pulls INTB pins low to alert host.
 * 
 * @details 1.Clears deasserted time. 
 * 
 */
static void mINTB_SendEvent(uint16_t u16RoutineTime)
{
    /* Sets up INTB */
    if(mINTBControl.eInterruptType == INTB_INT_TYPE_RISING)
    {
        mINTBControl.CallbackSDMIntbDoSet(1U);
    }
    else
    {
        mINTBControl.CallbackSDMIntbDoSet(0U);
    }

    /* Clears DeAsserted Time */
    mINTBControl.u16DeAssertedTimer = 0;
    
    /* Increates Asserted Timer. */
    mINTBControl.u16AssertedTimer = (mINTBControl.u16AssertedTimer < MINTB_SATISFIED_ASSERTED_TIME)?\
    (mINTBControl.u16AssertedTimer + u16RoutineTime):\
    (MINTB_SATISFIED_ASSERTED_TIME);
}
/**
 * @brief Waits for registering callback func. which controls INTB GPIO and trigger type.
 * 
 * @details N/A 
 * 
 * @note Calls MINTB_Register() to register these.
 * 
 */
static void mINTB_StateWaitRegister(uint16_t u16RoutineTime)
{
    if((mINTBControl.eStrategyCtrl == STRATEGY_CTRL_INIT)\
        && (mINTBControl.CallbackSDMIntbDoSet != NULL)\
        && (mINTBControl.eInterruptType != INTB_INT_TYPE_DEFAULT))
    {
        mINTBControl.eStateMachine = INTB_SM_INIT;
    }
    else
    {
        /* Wait */
    }

    (void)u16RoutineTime;
}
/**
 * @brief Waits for that system sets up INTB init.
 * 
 * @details N/A 
 * 
 * @note Calls MINTB_StrategyControl(STRATEGY_CTRL_INIT).
 * 
 */
static void mINTB_StateInit(uint16_t u16RoutineTime)
{
    if(mINTBControl.eInterruptType == INTB_INT_TYPE_RISING)
    {
        mINTBControl.CallbackSDMIntbDoSet(0U);
    }
    else
    {
        mINTBControl.CallbackSDMIntbDoSet(1U);
    }

    /* Increates DeAsserted Timer. */
    mINTBControl.u16DeAssertedTimer = (mINTBControl.u16DeAssertedTimer < MINTB_SATISFIED_DEASSERTED_TIME)?\
    (mINTBControl.u16DeAssertedTimer + u16RoutineTime):\
    (MINTB_SATISFIED_DEASSERTED_TIME);

    mINTBControl.eStateMachine = INTB_SM_WAIT_TRIGGER;
}

static void mINTB_StateWaitTrigger(uint16_t u16RoutineTime)
{
    if(mINTBControl.eInterruptType == INTB_INT_TYPE_RISING)
    {
        mINTBControl.CallbackSDMIntbDoSet(0U);
    }
    else
    {
        mINTBControl.CallbackSDMIntbDoSet(1U);
    }

    /* Increates DeAsserted Timer. */
    mINTBControl.u16DeAssertedTimer = (mINTBControl.u16DeAssertedTimer < MINTB_SATISFIED_DEASSERTED_TIME)?\
    (mINTBControl.u16DeAssertedTimer + u16RoutineTime):\
    (MINTB_SATISFIED_DEASSERTED_TIME);

    if(mINTBControl.eStrategyCtrl == STRATEGY_CTRL_START)
    {
        if(mINTBControl.u16DeAssertedTimer < MINTB_SATISFIED_DEASSERTED_TIME)
        {
            mINTBControl.u16DeAssertedTimer+=u16RoutineTime;
            mINTBControl.eStateMachine = INTB_SM_SETUP_TIME;
        }
        else
        {
            mINTB_SendEvent(u16RoutineTime);         
            mINTBControl.eStateMachine = INTB_SM_HOLD_TIME;
        }       
    }
    else if(mINTBControl.eStrategyCtrl == STRATEGY_CTRL_DEINIT)
    {
        mINTBControl.eStateMachine = INTB_SM_DEINIT;
    }
    else{/* Wait for trigger */}

}

static void mINTB_StateSetup(uint16_t u16RoutineTime)
{
    if(mINTBControl.eStrategyCtrl == STRATEGY_CTRL_CANCEL)
    {
        mINTBControl.eStateMachine = INTB_SM_WAIT_TRIGGER;
        mINTBControl.u16DeAssertedTimer = 0;
    }
    else if(mINTBControl.u16DeAssertedTimer < MINTB_SATISFIED_DEASSERTED_TIME)
    {
        /* Wait Time */
        /* Increates DeAsserted Timer. */
        mINTBControl.u16DeAssertedTimer += u16RoutineTime;
    }
    else
    {
        mINTB_SendEvent(u16RoutineTime);        
        mINTBControl.eStateMachine = INTB_SM_HOLD_TIME;
    }
}

static void mINTB_StateHold(uint16_t u16RoutineTime)
{
    if(mINTBControl.u16AssertedTimer < MINTB_SATISFIED_ASSERTED_TIME)
    {
        /* Wait Time */
        mINTBControl.u16AssertedTimer += u16RoutineTime;
    }
    else
    {
        if(mINTBControl.eInterruptType == INTB_INT_TYPE_RISING)
        {
            mINTBControl.CallbackSDMIntbDoSet(0U);
        }
        else
        {
            mINTBControl.CallbackSDMIntbDoSet(1U);
        }

        /* Clears eStrategyCtrl */
        mINTBControl.eStrategyCtrl = STRATEGY_CTRL_CANCEL;
        
        /* Changes State Machine */
        mINTBControl.eStateMachine = INTB_SM_WAIT_TRIGGER;

        /* Clears Asserted Time */
        mINTBControl.u16AssertedTimer = 0;

        /* Increates DeAsserted Timer. */
        mINTBControl.u16DeAssertedTimer = (mINTBControl.u16DeAssertedTimer < MINTB_SATISFIED_DEASSERTED_TIME)?\
        (mINTBControl.u16DeAssertedTimer + u16RoutineTime):\
        (MINTB_SATISFIED_DEASSERTED_TIME);
    }
}

static void mINTB_StateDeInit(uint16_t u16RoutineTime)
{
    if(mINTBControl.eInterruptType == INTB_INT_TYPE_RISING)
    {
        mINTBControl.CallbackSDMIntbDoSet(0U);
    }
    else
    {
        mINTBControl.CallbackSDMIntbDoSet(0U);
    }

    /* Deinit mINTBControl */
    mINTBControl.CallbackSDMIntbDoSet = NULL;
    mINTBControl.eInterruptType = INTB_INT_TYPE_DEFAULT;
    mINTBControl.eStateMachine = INTB_SM_WAIT_REGISTER;
    mINTBControl.u16AssertedTimer = 0x0000U;
    mINTBControl.u16DeAssertedTimer = 0x0000U;

    (void)u16RoutineTime;
}

/* -- Global Functions -- */
bool MINTB_Register(CALLBACK_SDM_INTB_DO_SET CallbackSDMIntbDoSet\
,MINTB_InttruptType_E eInterruptType)
{
    if((CallbackSDMIntbDoSet == NULL)\
        || ((eInterruptType != INTB_INT_TYPE_RISING) && (eInterruptType != INTB_INT_TYPE_FALLING))
        || (mINTBControl.eStateMachine != INTB_SM_WAIT_REGISTER))
    {
        return false;
    }

    mINTBControl.CallbackSDMIntbDoSet = CallbackSDMIntbDoSet;
    mINTBControl.eInterruptType = eInterruptType;

    return true;
}

bool MINTB_StrategyControl(MINTB_StrategyCtrl_E eStrategyCtrl)
{
    if((mINTBControl.CallbackSDMIntbDoSet == NULL)\
        || (mINTBControl.eInterruptType == INTB_INT_TYPE_DEFAULT))
    {
        return false;
    }
    else
    {
        mINTBControl.eStrategyCtrl = eStrategyCtrl;
    }

    return true;
}
/**
 * @brief INTB strategy control routine in period of 2ms.
 * 
 * @details N/A 
 * 
 * @note Sends communication event to call MINTB_StrategyControl() and change state machine.
 * 
 * @dot
 * digraph INTBStateMachine{
 *  init [label = "Init: \n Increates set-time."];
 *  wait_trigger [label = "Wait Trigger: \n 1.GPIO_INTB = H \n 2.Increates set-time. \n 3.Clears hold-time."];
 *  set_up [label = "Set Up: \n 1.Increates set-time. \n 2.GPIO_INTB = L when set-time >= 2ms."];
 *  hold [label = "Hold: \n 1.Clears set-time. \n 2.Increates hold-time. \n 3.GPIO_INTB = H when hold-time >= 2ms."];
 *  de_init [label = "Deinit: \n Rleases INTB Module."];
 * 
 *  init -> wait_trigger; 
 *  wait_trigger -> set_up [label = "Receives set up interrupt."];
 *  set_up -> hold [label = "GPIO_INTB is from H to L."];
 *  set_up -> wait_trigger [label = "Receives cancelled interrupt."];
 *  hold -> wait_trigger [label = "GPIO_INTB is from L to H."];
 *  wait_trigger -> de_init [label = "Receives power-off interrupt."];
 * }
 * @enddot
 * 
 */
void MINTB_Routine(uint16_t u16RoutineTime)
{
    switch(mINTBControl.eStateMachine)
    {
        default:
            break;

        case INTB_SM_WAIT_REGISTER:
            mINTB_StateWaitRegister(u16RoutineTime);
            break;

        case INTB_SM_INIT:
            mINTB_StateInit(u16RoutineTime);
            break;

        case INTB_SM_WAIT_TRIGGER:
            mINTB_StateWaitTrigger(u16RoutineTime);
            break;

        case INTB_SM_SETUP_TIME:
            mINTB_StateSetup(u16RoutineTime);
            break;

        case INTB_SM_HOLD_TIME:
            mINTB_StateHold(u16RoutineTime);
            break;

        case INTB_SM_DEINIT:
            mINTB_StateDeInit(u16RoutineTime);
            break;
    }
}
#endif
/* -- END -- */
