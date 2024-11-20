#include "public.h"
#include "M_FPNCtrl.h"
#include "Memory_Pool.h"
#include "M_FixedFlashAccess.h"

/* Declare Global value */
static tFPN_ctrl_def tFPNCtrlDelivery;
static tFPN_ctrl_def tFPNCtrlSerNum;
static tFPN_production_byte_def tFPNProductionByte;

static uint8_t M_FPNCtrl_FPNCheckSum_Calculate(uint8_t *pFPNBuff)
{
    uint8_t u8AddIndex = 0U;
    uint8_t u8SumTemp = 0U;
    uint8_t u8FPNbufftemp[FPN_SIZE];

    memcpy(u8FPNbufftemp,pFPNBuff,FPN_SIZE);

    /* get sum */
    for (u8AddIndex = 0U;u8AddIndex < FPN_CHECKSUM_INDEX;u8AddIndex++)
    {
    	u8SumTemp += u8FPNbufftemp[u8AddIndex];
    }

    return u8SumTemp;

}

static uint8_t  M_FPNCtrl_FPNCheckSum_Compare (uint8_t u8TCS,uint8_t u8SCS)
{
    uint8_t u8CheckReault = 0U;

    if(u8TCS == u8SCS)
    {
    	u8CheckReault = FPN_ENABLE;
    }
    else
    {
    	u8CheckReault = FPN_DISABLE;
    }

    return u8CheckReault;
}

static uint8_t M_FPNCtrl_DeliveryFPNWriteToEE_Process(void)
{
    uint8_t *pu8Delivery_assembly = NULL;
    uint8_t  u8ProcessStatus = 0U;
    uint8_t u8Temp = 0U;
    uint8_t u8CompResulat = 0U;

    /* Write FPN to EEPROM */
    if(tFPNCtrlDelivery.FPN_Flag_t.FPNCtrlFlag == FPN_ENABLE)
    {
        if(tFPNCtrlDelivery.FPN_Flag_t.FPNDataFlag == FPN_ENABLE)
        {
            /* Caculate sum */
             u8Temp = M_FPNCtrl_FPNCheckSum_Calculate(tFPNCtrlDelivery.pFPNDataBuff);

            /* Compare with checksum */
            u8CompResulat = M_FPNCtrl_FPNCheckSum_Compare(u8Temp,tFPNCtrlDelivery.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;

            if(u8CompResulat == FPN_ENABLE)
            {
                pu8Delivery_assembly = tFPNCtrlDelivery.pFPNDataBuff;
                MFixedFlashAccess_WritePage(ADDR_DELIVERY_ASSEMBLY, pu8Delivery_assembly, tFPNCtrlDelivery.FPNDataLength);
                u8ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
            	u8ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
		else
		{  
			u8ProcessStatus = FPN_WRITE_NO_DATA;
		}
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
    	u8ProcessStatus = FPN_WRITE_FAIL;
    }
    return u8ProcessStatus;
}

uint8_t M_FPNCtrl_DeliveryWRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = FPN_DISABLE;
    uint8_t u8ProcessState = FPN_DISABLE;

    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;

        if(u8WRTST == FPN_ENABLE)
        {
            
            u8ProcessState = FPN_DISABLE;
        }
        else
        {
            u8ProcessState = FPN_ENABLE;            
        }
    }
    else
    { /* Nothing */ }

    return u8ProcessState;
}

uint8_t M_FPNCtrl_DeliveryFPNControl_Process(tFPN_ctrl_def *fpn_t)
{
    uint8_t  u8ProcessStatus = 0U;

    tFPNCtrlDelivery = *fpn_t;

    if(M_FPNCtrl_DeliveryWRTST_Check(tFPNCtrlDelivery.FPN_Status_t.WRT_ST)== FPN_ENABLE)
    {
        tFPNCtrlDelivery.FPN_Flag_t.FPNCtrlFlag = FPN_ENABLE;
    }
    else
    {
        tFPNCtrlDelivery.FPN_Flag_t.FPNCtrlFlag = FPN_DISABLE;
    }
    
    u8ProcessStatus = M_FPNCtrl_DeliveryFPNWriteToEE_Process();

    return u8ProcessStatus;
}


uint8_t M_FPNCtrl_SerNumFPNWriteToEE_Process(void)
{
    uint8_t *pu8SerNum_assembly = NULL;
    uint8_t  u8ProcessStatus = 0U;
    uint8_t u8Temp = 0U;
    uint8_t u8CompResulat = 0U;

    /* Write FPN to EEPROM */
    if(tFPNCtrlSerNum.FPN_Flag_t.FPNCtrlFlag == FPN_ENABLE)
    {

        if(tFPNCtrlSerNum.FPN_Flag_t.FPNDataFlag == FPN_ENABLE)
        {
            /* Caculate sum */
            u8Temp = M_FPNCtrl_FPNCheckSum_Calculate(tFPNCtrlSerNum.pFPNDataBuff);

            /* Compare with checksum */
            u8CompResulat = M_FPNCtrl_FPNCheckSum_Compare(u8Temp,tFPNCtrlSerNum.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;

            if(u8CompResulat == FPN_ENABLE)
            {
                pu8SerNum_assembly = tFPNCtrlSerNum.pFPNDataBuff;
                MFixedFlashAccess_WritePage(ADDR_SERIAL_NUMBER, pu8SerNum_assembly, tFPNCtrlSerNum.FPNDataLength);              
                u8ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
            	u8ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
		else
		{  
			u8ProcessStatus = FPN_WRITE_NO_DATA;
		}		
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
    	u8ProcessStatus = FPN_WRITE_FAIL;
    }

    return u8ProcessStatus;
}

uint8_t M_FPNCtrl_SerNum_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = FPN_DISABLE;
    uint8_t u8ProcessState = FPN_DISABLE;

    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;

        if(u8WRTST == FPN_ENABLE)
        {            
            u8ProcessState = FPN_DISABLE;
        }
        else
        {
            u8ProcessState = FPN_ENABLE;            
        }
    }
    else
    { /* Nothing */ }

    return u8ProcessState;
}

uint8_t M_FPNCtrl_SerNumFPNControl_Process(tFPN_ctrl_def *fpn_t)
{
    uint8_t  u8ProcessStatus = 0U;

    tFPNCtrlSerNum = *fpn_t;

    if(M_FPNCtrl_SerNum_WRTST_Check(tFPNCtrlSerNum.FPN_Status_t.WRT_ST)== FPN_ENABLE)
    {
        tFPNCtrlSerNum.FPN_Flag_t.FPNCtrlFlag = FPN_ENABLE;
    }
    else
    {
        tFPNCtrlSerNum.FPN_Flag_t.FPNCtrlFlag = FPN_DISABLE;
    }
   
    u8ProcessStatus = M_FPNCtrl_SerNumFPNWriteToEE_Process();

    return u8ProcessStatus;
}

uint8_t M_FPNCtrl_ProductionByte_FPNWriteToEE_Process(void)
{
    uint8_t *pu8FCE = NULL;
    uint8_t  u8ProcessStatus = 0U;

    /* Write FPN to EEPROM */
    if(tFPNProductionByte.FPN_Flag_t.FPNCtrlFlag == FPN_ENABLE)
    {
        if(tFPNProductionByte.FPN_Flag_t.FPNDataFlag == FPN_ENABLE)
        {
            pu8FCE = tFPNProductionByte.pFPNDataBuff;
            MFixedFlashAccess_WritePage(ADDR_PRODUCTION_PHASE_BYTE, pu8FCE, tFPNProductionByte.FPNDataLength);
            u8ProcessStatus = FPN_WRITE_SUCCESS;
        }
		else
		{  
			u8ProcessStatus = FPN_WRITE_NO_DATA;
		}
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
    	u8ProcessStatus = FPN_WRITE_FAIL;
    }

    return u8ProcessStatus;
}

uint8_t M_FPNCtrl_ProductionByte_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = FPN_DISABLE;
    uint8_t u8ProcessState = FPN_DISABLE;

    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;

        if(u8WRTST == FPN_ENABLE)
        {            
            u8ProcessState = FPN_DISABLE;
        }
        else
        {
            u8ProcessState = FPN_ENABLE;            
        }
    }
    else
    { /* Nothing */ }

    return u8ProcessState;
}

uint8_t M_FPNCtrl_ProductionByteFPNControl_Process(tFPN_production_byte_def *fpn_t)
{
    uint8_t  u8ProcessStatus = 0U;

    tFPNProductionByte = *fpn_t;

    if(M_FPNCtrl_ProductionByte_WRTST_Check(tFPNProductionByte.FPN_Status_t.WRT_ST)== FPN_ENABLE)
    {
        tFPNProductionByte.FPN_Flag_t.FPNCtrlFlag = FPN_ENABLE;
    }
    else
    {
        tFPNProductionByte.FPN_Flag_t.FPNCtrlFlag = FPN_DISABLE;
    }
   
    u8ProcessStatus = M_FPNCtrl_ProductionByte_FPNWriteToEE_Process();

    return u8ProcessStatus;
}
