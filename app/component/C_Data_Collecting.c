#include "C_Data_Collecting.h"
#include "Memory_Pool.h"

static tdata_collecting_task_def tDataCollectingTask;
/******************************************************************************
 ;       Function Name			:	void C_Data_Collecting_Init(void)
 ;       Function Description	:	This state will do Data_Collecting  initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Data_Collecting_Init(void)
{
	M_DC_Init();
	M_DC_Function_Execute(DC_INIT_ADCVALUE);
	/*Set ADC values.*/
	Memory_Pool_BatteryADC_Set(gtDCInfo.i16PresentVbattADC);
	Memory_Pool_SyncADC_Set(gtDCInfo.i16PresentVSyncADC);
	Memory_Pool_FPCTxOutADC_Set(gtDCInfo.i16PresentFPCTxOutADC);
	Memory_Pool_FPCRxOutADC_Set(gtDCInfo.i16PresentFPCRxOutADC);
	Memory_Pool_PCBATempADC_Set(gtDCInfo.i16PresentPCBTempADC);
	Memory_Pool_BLTempADC_Set(gtDCInfo.i16PresentBacklightTempADC);	
	/*Set Analog Voltage values.*/
	Memory_Pool_BatteryAnaVol_Set(gtDCInfo.i16PresentVbattAnaVol);
	Memory_Pool_SyncAnaVol_Set(gtDCInfo.i16PresentVSyncAnaVol);
	Memory_Pool_FPCTxOutAnaVol_Set(gtDCInfo.i16PresentFPCTxAnaVol);
	Memory_Pool_FPCRxOutAnaVol_Set(gtDCInfo.i16PresentFPCRxAnaVol);
	Memory_Pool_PCBATempAnaVol_Set(gtDCInfo.i16PresentTPCBAnaVol);
	Memory_Pool_BLTempAnaVol_Set(gtDCInfo.i16PresentTBacklightAnaVol);	
	/*Set Voltage values.*/
	Memory_Pool_BatteryVol_Set(gtDCInfo.i16PresentVbatt);
	Memory_Pool_SyncVol_Set(gtDCInfo.i16PresentVSync);
	Memory_Pool_FPCTxOutVol_Set(gtDCInfo.i16PresentFPCTxOut);
	Memory_Pool_FPCRxOutVol_Set(gtDCInfo.i16PresentFPCRxOut);
	/*Set temperature values.*/
	Memory_Pool_PCBATemp_Set(gtDCInfo.i16PresentTPCB);
	Memory_Pool_BLTemp_Set(gtDCInfo.i16PresentTBacklight);
	
	tDataCollectingTask.u16Timer1 = TIME_DISABLE;
	(void)Task_ChangeState(TYPE_DATA_COLLECTING, LEVEL5, STATE_DATA_COLLECTING_CTRL, Data_Collecting_State_Machine[STATE_DATA_COLLECTING_CTRL]);
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Data_Collecting_Control(void)
 ;       Function Description	:	This state will do Data_Collecting on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Data_Collecting_Control(void)
{
	switch (Task_Current_Event_Get())
	{
		case EVENT_FIRST :
			M_DC_Function_Execute(DC_TURNON_VBATT_MEASURE);
			tDataCollectingTask.u16Timer1 = TIME_10ms;
		break;

		case EVENT_TIMER1 :
			/*Convert to MV than look up table.*/
			M_DC_Function_Execute(DC_QUERY_VBATT_TABLE);
			M_DC_Function_Execute(DC_QUERY_VSYNC_TABLE);
			M_DC_Function_Execute(DC_QUERY_PCBT_TABLE);
			M_DC_Function_Execute(DC_QUERY_BLT_TABLE);
			M_DC_Function_Execute(DC_QUERY_FPCTXOUT_TABLE);
			M_DC_Function_Execute(DC_QUERY_FPCRXOUT_TABLE);
			/*Set ADC values.*/
			Memory_Pool_BatteryADC_Set(gtDCInfo.i16PresentVbattADC);
			Memory_Pool_SyncADC_Set(gtDCInfo.i16PresentVSyncADC);
			Memory_Pool_FPCTxOutADC_Set(gtDCInfo.i16PresentFPCTxOutADC);
			Memory_Pool_FPCRxOutADC_Set(gtDCInfo.i16PresentFPCRxOutADC);
			/*Set Analog Voltage values.*/
			Memory_Pool_BatteryAnaVol_Set(gtDCInfo.i16PresentVbattAnaVol);
			Memory_Pool_SyncAnaVol_Set(gtDCInfo.i16PresentVSyncAnaVol);
			Memory_Pool_FPCTxOutAnaVol_Set(gtDCInfo.i16PresentFPCTxAnaVol);
			Memory_Pool_FPCRxOutAnaVol_Set(gtDCInfo.i16PresentFPCRxAnaVol);
			/*Set Voltage values.*/
			Memory_Pool_BatteryVol_Set(gtDCInfo.i16PresentVbatt);
			Memory_Pool_SyncVol_Set(gtDCInfo.i16PresentVSync);
			Memory_Pool_FPCTxOutVol_Set(gtDCInfo.i16PresentFPCTxOut);
			Memory_Pool_FPCRxOutVol_Set(gtDCInfo.i16PresentFPCRxOut);

			if(gtDCInfo.u8IIRInit == 1U)
			{
				/*Set ADC values.*/
				Memory_Pool_PCBATempADC_Set(gtDCInfo.i16PresentPCBTempADC);
				Memory_Pool_BLTempADC_Set(gtDCInfo.i16PresentBacklightTempADC);
				/*Set Analog Voltage values.*/
				Memory_Pool_PCBATempAnaVol_Set(gtDCInfo.i16PresentTPCBAnaVol);
				Memory_Pool_BLTempAnaVol_Set(gtDCInfo.i16PresentTBacklightAnaVol);
				/*Set temperature values.*/
				//Memory_Pool_PCBATemp_Set(gtDCInfo.i16PresentTPCB);	//David Test de-rating function use 0xBA command input temperature.
				Memory_Pool_PCBATemp_Set(gtDCInfo.i16PresentTPCB);
				Memory_Pool_BLTemp_Set(gtDCInfo.i16PresentTBacklight);
			}
			else
			{ /* Nothing */ }
			
			/*Start Conversion*/
			M_DC_Function_Execute(DC_START_MEASURE);
			tDataCollectingTask.u16Timer1 = TIME_10ms;
		break;

		default:
			(void)Task_ChangeState(TYPE_DATA_COLLECTING, LEVEL5, STATE_DATA_COLLECTING_ERROR, Data_Collecting_State_Machine[STATE_DATA_COLLECTING_ERROR]);
		break;
	}
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Data_Collecting_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Data_Collecting_Error(void)
{
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Data_Collecting_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Data_Collecting_Timer1(void)
{
	if (tDataCollectingTask.u16Timer1 > TIME_UP)
	{
		tDataCollectingTask.u16Timer1--;
		if (tDataCollectingTask.u16Timer1 == TIME_UP)
		{
			tDataCollectingTask.u16Timer1 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_DATA_COLLECTING, LEVEL3, EVENT_TIMER1);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}
void (*const Data_Collecting_State_Machine[MAX_DC_STATE_NO])(void) =
{   C_Data_Collecting_Init, C_Data_Collecting_Control, C_Data_Collecting_Error };

