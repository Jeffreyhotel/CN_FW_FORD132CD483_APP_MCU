#include "M_PowerManagement.h"

#include "../../Mymain.h"
#include "M_INTB.h"
#include "M_DataCollecting.h"
#include "Memory_Pool.h"

static uint8_t M_PM_CheckPowerPG(uint8_t u8Status, uint8_t u8Times)
{
    uint8_t u8PGLevel;
	uint8_t u8CheckTime;
	uint8_t u8Return = LOW_LEVEL;
	
    
   	if (u8Times == 0U)
   	{   /* times can not be zero */
       u8Return = LOW_LEVEL;
   	}
	else
   	{
		for(u8CheckTime = u8Times; u8CheckTime > 0U; u8CheckTime--)
	    {
	        switch(u8Status)
	        {
		        case P3V3_PGOOD:
		            HAL_GPIO_Read(U301_P3V3_PGOOD_PORT, U301_P3V3_PGOOD_PIN, &u8PGLevel);
		            break;
		        case P1V2_PGOOD:
		            HAL_GPIO_Read(U301_P1V2_PGOOD_PORT, U301_P1V2_PGOOD_PIN, &u8PGLevel);
		            break;
		        default:
		            break;
	        }
	        
	        if (u8PGLevel == HIGH_LEVEL)
	        {   /* pin good! */
	        	u8Return = HIGH_LEVEL;
	            return u8Return;
	        }
	        else
	        {
	            Cy_SysLib_DelayUs(500U); /* 500us */
	        }
	        
	    }
	}
    /* error flag */
    return u8Return;
}

/******************************************************************************
 ;       Function Name			:	void M_PM_Sequnce_Execute(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
uint8_t M_PM_Sequnce_Execute(uint8_t u8Action)
{
    uint8_t u8Return;
    uint16_t u16Temp=0U;
	int16_t  s16VBatAdc = 0;

    switch (u8Action)
    {
        default:
            u8Return = POWER_CMD_FAIL;
            break;
        case POWER_OFF_READY:
        case POWER_ON_READY:
            u8Return = POWER_PASS;
            break;
        case POWER_ON:
            HAL_GPIO_High( U301_HV_LDO_EN_PORT, U301_HV_LDO_EN_PIN);
            HAL_GPIO_High( U301_P3V3_EN_PORT, U301_P3V3_EN_PIN);
            //Cy_SysLib_Delay(9U);  /* <=10ms */
			Cy_SysLib_Delay(6U);  /* <=10ms */
			HAL_GPIO_High( U301_P1V2_EN_PORT, U301_P1V2_EN_PIN);
	        Cy_SysLib_Delay(4U);

			/*Start AD Conversion*/
            M_DC_Function_Execute(DC_START_MEASURE);

	        if (HIGH_LEVEL == M_PM_CheckPowerPG(P3V3_PGOOD, 12U)) /* about 1ms */
			{
                Cy_SysLib_Delay(2U);  /* 2ms */
			    Cy_SysLib_DelayUs(200U); /* 200us */

				s16VBatAdc = HAL_ADC0_Get_ADCRead(HAL_ADC_VBATT_VOLT);
				s16VBatAdc = HAL_ADC0_Get_Channel_mVolt(HAL_ADC_VBATT_VOLT,(uint16_t)s16VBatAdc);
				if((s16VBatAdc < POWER_VMINRCV_ADC_CFG) ||(s16VBatAdc > POWER_VMAXRCV_ADC_CFG))
				{
					/* Record 0xA3 Status */
					u16Temp = Memory_Pool_GeneralDiagnosis_Get();
					if(s16VBatAdc < POWER_VMINRCV_ADC_CFG)
					{
						Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_ON_LOW_VOL_ERROR_POS); 
					}
					else if(s16VBatAdc > POWER_VMAXRCV_ADC_CFG)
					{
						Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_ON_HIGH_VOL_ERROR_POS); 
					}
					else
					{ /* Nothing */ }

					u8Return = POWER_VBAT_FAIL;
				}
				else
				{	
	                // HAL_GPIO_High( U301_P1V2_EN_PORT, U301_P1V2_EN_PIN);				
	                // Cy_SysLib_Delay(4U);	/* <=10ms */
					
	                if (HIGH_LEVEL == M_PM_CheckPowerPG(P1V2_PGOOD, 10U))
	                {
						Cy_SysLib_Delay(1U);	/* 1ms */
	                	HAL_GPIO_High( U301_TSC_RESET_PORT, U301_TSC_RESET_PIN);

						Cy_SysLib_Delay(11U); /* 11ms */
						HAL_GPIO_High( U301_DISP_GLOBAL_RESET_PORT, U301_DISP_GLOBAL_RESET_PIN);

						Cy_SysLib_Delay(51U); /* >50ms */
	                    HAL_GPIO_High( U301_PDB_PORT, U301_PDB_PIN);
	                    HAL_GPIO_High( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
	                    /* INTB Init. */
	                    MINTB_StrategyControl(STRATEGY_CTRL_INIT);
				
	                    u8Return = POWER_PASS;
	                }
	                else
	                {
	                    u8Return = P1V2_FAIL;
	                }
				}
	       	}
			else
			{
	            u8Return = P3V3_FAIL;
			}
           	break;
        case POWER_OFF:
			Cy_SysLib_Delay(6U); /* 6ms */
            /* INTB Deinit */
			MINTB_StrategyControl(STRATEGY_CTRL_DEINIT);
            HAL_GPIO_Low( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
            HAL_GPIO_Low( U301_PDB_PORT, U301_PDB_PIN);
			
			Cy_SysLib_Delay(6U); /* 6ms */
            HAL_GPIO_Low( U301_P1V2_EN_PORT, U301_P1V2_EN_PIN);
			
			Cy_SysLib_Delay(6U); /* 6ms */
            HAL_GPIO_Low( U301_P3V3_EN_PORT, U301_P3V3_EN_PIN);

			//Cy_SysLib_Delay(6U); 
			Cy_SysLib_Delay(101U); /* 100ms */
            HAL_GPIO_Low( U301_HV_LDO_EN_PORT, U301_HV_LDO_EN_PIN);

            HAL_GPIO_Low(EN_VBATT_SENSE_PORT,EN_VBATT_SENSE_PIN);
            
            u8Return = POWER_PASS;
            break;
    }

    return u8Return;
}

