#include "C_Power_Management.h"
#include "C_Battery_Protect.h"
#include "M_BatteryProtect.h"
#include "Memory_Pool.h"

static tbattery_protect_task_def tBatteryProtectTask;
/******************************************************************************
 ;       Function Name			:	void C_Battery_Protect_Init(void)
 ;       Function Description	:	This state will do Battery_Protect  initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Battery_Protect_Init(void)
{
	tBatteryProtectTask.u16Timer1 = TIME_DISABLE;
	(void)Task_ChangeState(TYPE_BATTERY_PROTECT, LEVEL5, STATE_BATTERY_PROTECT_CTRL, Battery_Protect_State_Machine[STATE_BATTERY_PROTECT_CTRL]);
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Battery_Protect_Control(void)
 ;       Function Description	:	This state will do Battery_Protect on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Battery_Protect_Control(void)
{
	switch(Task_Current_Event_Get())
	{
		case EVENT_FIRST :
			tBatteryProtectTask.u16Timer1=TIME_3000ms; /*Start battery protect after 3s*/
		break;

		case EVENT_TIMER1 :
			tBatteryProtectTask.u16Timer1=TIME_5ms;
			M_BP_State_Maching((uint16_t)(tBatteryProtectTask.u16Timer1-1));
		break;

		default:
			(void)Task_ChangeState(TYPE_BATTERY_PROTECT, LEVEL5, STATE_BATTERY_PROTECT_ERROR, Battery_Protect_State_Machine[STATE_BATTERY_PROTECT_ERROR]);
		break;
	}
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Battery_Protect_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Battery_Protect_Error(void)
{
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Battery_Protect_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Battery_Protect_Timer1(void)
{
	if(tBatteryProtectTask.u16Timer1 > TIME_UP)
	{
		tBatteryProtectTask.u16Timer1--;
		if (tBatteryProtectTask.u16Timer1 == TIME_UP)
		{
			tBatteryProtectTask.u16Timer1 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_BATTERY_PROTECT, LEVEL3, EVENT_TIMER1);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}

void (*const Battery_Protect_State_Machine[MAX_BP_STATE_NO])(void) =
{   C_Battery_Protect_Init, C_Battery_Protect_Control, C_Battery_Protect_Error };

