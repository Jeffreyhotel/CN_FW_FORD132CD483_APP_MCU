/******************************************************************************
 ;				Program		:	main.c
 ;				Function	:	Main Function
 ;				Chip		:	Cypress CY8C4149AZI-S598
 ;				Clock		:	IMO Internal 48MHz
 ;				Date		:	2021 / 10 / 07
 ;				Author		:	Fenderson Lu
 ;				Describe	:
 ******************************************************************************/
/*---------------------------- Include File ----------------------------------*/
#include "Mymain.h"

#include "C_Power_Management.h"
#include "C_Communication.h"
#include "C_Backlight_Management.h"
#include "C_Display_Management.h"
#include "C_Data_Collecting.h"
#include "C_Diagnosis.h"
#include "C_Temperture_Protect.h"
#include "C_Battery_Protect.h"
#include "FIDM_Config.h"
#ifdef DEBUG_UART_EN
#include "simple_cli.h"
#include "FIDM_SupportCLI.h"
#endif

static tdINTBIF *ptrINTB = NULL;
/******************************************************************************
 ;       Function Name			:	void Main_TaskInit(void)
 ;       Function Description	:	Initial all Tasks.
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void Main_TaskInit(void)
{
	uint8_t u8counter = 0;
	
	Task_RecorderInit();
	Task_QueueManagerInit();

	// If you want to increase or decrease the tasks,please change TASK_NUM at same time.
	// The recorder will remember the task that it need to execute.
	Task_TaskInRecorder(Power_Manage_State_Machine[STATE_INIT ], TYPE_POWER_MANAGE, u8counter++);
	Task_TaskInRecorder(Display_Manage_State_Machine[STATE_INIT ], TYPE_DISPLAY_MANAGE, u8counter++);
	Task_TaskInRecorder(Communication_State_Machine[STATE_INIT ], TYPE_COMMUNICATION, u8counter++);
	Task_TaskInRecorder(Backlight_Manage_State_Machine[STATE_INIT ], TYPE_BACKLIGHT_MANAGE, u8counter++);
	Task_TaskInRecorder(Data_Collecting_State_Machine[STATE_INIT ], TYPE_DATA_COLLECTING, u8counter++);
	Task_TaskInRecorder(Diagnosis_State_Machine[STATE_INIT ], TYPE_DIAGNOSIS, u8counter++);
	Task_TaskInRecorder(Battery_Protect_State_Machine[STATE_INIT ], TYPE_BATTERY_PROTECT, u8counter++);
	Task_TaskInRecorder(Temperture_Protect_State_Machine[STATE_INIT ], TYPE_TEMPERTURE_PROTECT, u8counter++);
	// The assigner will arbitrate the priority for each tasks.
	Task_AssignedTask();

	// Executer will push task into the queue.
	Task_PushTaskInQueue();
}
/******************************************************************************
 ;       Function Name			:	void Main_UartInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
#ifdef DEBUG_UART_EN
static void Main_UartInit(void)
{
	debug_uart_tydef tDebugUart;

	tDebugUart.pBase = DEBUG_PORT;
	tDebugUart.pConfig = &DEBUG_UART_config;

	(void) HAL_UART_Init(tDebugUart);
}

#endif
/******************************************************************************
 ;       Function Name			:	void Main_BspInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
static void Main_SysTickInit(void)
{
	systick_typedef tSysTick;

	tSysTick.tClockSource = CY_SYSTICK_CLOCK_SOURCE_CLK_CPU;
	tSysTick.u32Interval = SYSTICK_COUNT;
	tSysTick.pCallback = &Main_TimerCore;

	(void) HAL_SYSTICK_Init(tSysTick);
}
/******************************************************************************
 ;       Function Name			:	void Main_BspInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
static void Main_BspInit(void)
{
	(void) cybsp_init();
#ifdef DEBUG_UART_EN
	(void) Main_UartInit();
	(void) FIDM_CLI_Init();
#endif
	(void) Main_SysTickInit();
//	(void) Cy_WDT_Init(); // Test WDT 108
//	(void) Cy_WDT_Enable(); // Test WDT 109
}
/******************************************************************************

 ;       Function Name			:	void main(void)
 ;       Function Description	:	Main function.
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void appmain(void)
{
	uint32_t u32SuperLoopCnt = 0UL;

	Main_BspInit();
	Main_TaskInit();

	__enable_irq();
	EventGroup_Create();   //Submodule function
	ptrINTB = GetINTB_Instance();  //INTB function 2023/01/17
	ptrINTB->Init(aTRUE);  //INTB function 2023/01/17

	HAL_UART_Printf("\r\nTask Start!\r\n");

	while (u32SuperLoopCnt < (uint32_t) (LOOP_MAX))
	{
#ifdef DEBUG_UART_EN
		SCLI_Run();
#endif
		//Cy_WDT_ClearWatchdog();
		Task_ProcessTask();
		ptrINTB->StateMachine(NULL);  //INTB function 2023/01/17
	}

	// return 0U;
}
/******************************************************************************
 ;       Function Name			:	void Main_TimerCore(void)
 ;       Function Description	:	Timer core function.
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void Main_TimerCore(void)
{
	C_Diagnosis_Timer1();
	C_Diagnosis_Timer2();
	C_Power_Manage_Timer1();
	C_Display_Manage_Timer1();
	C_Display_Manage_Timer2();
	C_Display_Manage_Timer3();
	C_Communication_Timer1();
	C_Communication_Timer2();
	C_Backlight_Manage_Timer1();
	C_Backlight_Manage_Timer2();
	C_Data_Collecting_Timer1();
	C_Temperture_Protect_Timer1();
	C_Battery_Protect_Timer1();
	//ptrINTB->Trigger(aTRUE);
}

