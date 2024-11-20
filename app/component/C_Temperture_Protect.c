#include "C_Temperture_Protect.h"
#include "C_Diagnosis.h"
#include "Memory_Pool.h"
#include "C_Communication.h"

static ttemperture_protect_task_def tTempertureProtectTask;
static ttemperture_protect_manage_def tTempertureProtectManager;
/******************************************************************************
 ;       Function Name			:	void C_Temperture_Protect_Init(void)
 ;       Function Description	:	This state will do Temperture_Protect  initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Temperture_Protect_Init(void)
{
	M_TP_Init();
	tTempertureProtectManager.u8TempertureProtectMode = TP_STATE_INIT;
	tTempertureProtectManager.bTempertureProtectEnable = true;
	tTempertureProtectTask.u16Timer1 = TIME_DISABLE;
	(void)Task_ChangeState(TYPE_TEMPERTURE_PROTECT, LEVEL5, STATE_TEMPERTURE_PROTECT_CTRL, Temperture_Protect_State_Machine[STATE_TEMPERTURE_PROTECT_CTRL]);
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Temperture_Protect_Control(void)
 ;       Function Description	:	This state will do Temperture_Protect on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Temperture_Protect_Control(void)
{
	switch (Task_Current_Event_Get())
	{
		case EVENT_FIRST :
			tTempertureProtectTask.u16Timer1 = TIME_200ms;
		break;

		case EVENT_MESSAGE :
			if (Memory_Pool_TempProtectEn_Get() == false)
			{
				tTempertureProtectManager.bTempertureProtectEnable = false;
			}
			else
			{
				tTempertureProtectManager.bTempertureProtectEnable = true;
			}
		break;

		case EVENT_TIMER1 :
			(void)Task_ChangeEvent(TYPE_BACKLIGHT_MANAGE, LEVEL3, EVENT_MESSAGE);
			tTempertureProtectTask.u16Timer1 = TIME_2ms;
		break;

		default:
			(void)Task_ChangeState(TYPE_TEMPERTURE_PROTECT, LEVEL5, STATE_TEMPERTURE_PROTECT_ERROR, Temperture_Protect_State_Machine[STATE_TEMPERTURE_PROTECT_ERROR]);
		break;
	}
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Temperture_Protect_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Temperture_Protect_Error(void)
{
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Temperture_Protect_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Temperture_Protect_Timer1(void)
{
	if(tTempertureProtectTask.u16Timer1 > TIME_UP)
	{
		tTempertureProtectTask.u16Timer1--;
		if (tTempertureProtectTask.u16Timer1 == TIME_UP)
		{
			tTempertureProtectTask.u16Timer1 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_TEMPERTURE_PROTECT, LEVEL3, EVENT_TIMER1);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}

void (*const Temperture_Protect_State_Machine[MAX_TP_STATE_NO])(void) =
{   C_Temperture_Protect_Init, C_Temperture_Protect_Control, C_Temperture_Protect_Error };

