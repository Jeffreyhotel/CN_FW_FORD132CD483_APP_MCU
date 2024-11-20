/**
 * @file M_Derating.c
 * 
 * @brief The program could execute various derating method which is made up of four fixed state machine types,
 * it returns minimal derating back-light value. 
 * 
 * @details 1.To call DeratingApp_RegisterElement() for registering a derating method.\n
 * 2.To call DeratingApp_RegisterDimmingControl() for setting dimming control curve of the derating method.\n
 * 3.To put DeratingApp_Routine() into a cyclic call function,and it's cycilc time is equal to u16PeriodTime which is a paramster of DeratingApp_RegisterElement().\n
 * 4.To call DeratingApp_DeratingOutData_Get() for getting the minimal derating back-light value.
 * 
 * @note Registered derating method must be incloud init,boot,derating,normal states. 
 * 
 * @author Orlando Huang (Orlando.Huang@auo.com)
 * 
 * @version 0.1
 * 
 * @date 2022-05-03 
 *  
 */

/* -- Includes -- */
#include "M_Derating.h"

/* -- Marco Define -- */
#if(DERATINGAPP_FLOAT_OPERATION)
#define DERATINGAPP_DEFAULT_OUTDATA     100.0f /* For derating PWM duty Init. */
#else
#define DERATINGAPP_DEFAULT_OUTDATA     100 /* For derating PWM duty Init. */
#endif

/* -- Data Type Define -- */
/**
 * @brief To call various derating method and calculate minimal derating back-light value automatically.
 * 
 */
typedef struct
{
    DERATINGAPP_STATUS_METHOD* InitMethod; /**< A reference of init. state function. */
    DERATINGAPP_STATUS_METHOD* BootMethod; /**< A reference of boot state function. */
    DERATINGAPP_STATUS_METHOD* DeratingMethod; /**< A reference of derating state function. */
    DERATINGAPP_STATUS_METHOD* NormalMethod; /**< A reference of normal state function. */
    uint16_t u16MethodPeriodTime; /**< To call DeratingApp_DimmingControl_Routine() once in the time. */
    uint16_t u16Methodu32UpdateTime; /**< To update derating method once in the time. */
    uint16_t u16MethodCheckTime; /**< To execute derating method once in the time. */
#if(DERATINGAPP_FLOAT_OPERATION)
    float DeratingDimmingOutputData; /**< To indicate derating back-light value . */
#else
    int16_t DeratingDimmingOutputData; /**< To indicate derating back-light value . */
#endif
}DeratingApp_Control_Typedef;
/**
 * @brief To calculate dimming variation value according proportion table.
 * 
 */
typedef struct
{
    uint8_t u8ProportionQuantity; /**< A quantity of proportion setting values. */
    uint32_t* u32ptrProportion; /**< A reference of proportion setting values. */
    uint32_t u32ProportionSum; /**< A denominator of proportion. */
    uint32_t* u32ptrUpdateProportionTime; /**< A update time of proportion setting values. */
    uint32_t u32PeriodTime; /**< To call DeratingApp_DimmingControl_Routine() once in the time. */
#if(DERATINGAPP_FLOAT_OPERATION)
    float BaseData; /**< To indicate output data last time. */
    float OutputData; /**< To indicate output data this time. */
#else
    int16_t BaseData; /**< To indicate output data last time. */
    int16_t OutputData; /**< To indicate output data this time. */
#endif
    bool bIsWorking; /**< To indicate dimming control function working status. */
    uint8_t u8ProportionIndex; /**< To indicate the index of proportion setting array. */
    uint32_t u32UpdateTime; /**< To update dimming control once in the time. */
#if(DERATINGAPP_FLOAT_OPERATION)
    float DataDiff; /**< To indicate the difference of proportion change. */
#else
    int16_t DataDiff; /**< To indicate the difference of proportion change. */
#endif
}DeratingApp_DimmingControl_Typedef;

/* -- Global Variables -- */
/**
 * @brief To indicate return data for a specific derating method.
 * 
 * @details 1.MethodStatus is a state of derating machine.\n
 * 2.DeratingOutData is the derating back-light value at this status.  
 * 
 */
DeratingApp_ReturnMethod_Typedef gReturnMethodSet[DERATINGAPP_CONTROL_SIZE]=
{
    {.MethodStatus = INIT_MODE,.DeratingOutData = DERATINGAPP_DEFAULT_OUTDATA},
    {.MethodStatus = INIT_MODE,.DeratingOutData = DERATINGAPP_DEFAULT_OUTDATA},
    {.MethodStatus = INIT_MODE,.DeratingOutData = DERATINGAPP_DEFAULT_OUTDATA},
    {.MethodStatus = INIT_MODE,.DeratingOutData = DERATINGAPP_DEFAULT_OUTDATA},
};
/**
 * @brief To indicate a set of various type derating methods.
 * 
 * @details DeratingApp_Routine() will execute a specific state machine for various type derating methods according to the set's content.
 * The set's content are modmodifiedify by calling DeratingApp_RegisterElement().
 * 
 */
static DeratingApp_Control_Typedef gDeratingAppControlSet[DERATINGAPP_CONTROL_SIZE] = 
{
    {NULL,NULL,NULL,NULL,DERATINGAPP_MIN_PERIOD_MS,0U,DERATINGAPP_MIN_CHECK_MS,DERATINGAPP_DEFAULT_OUTDATA},
    {NULL,NULL,NULL,NULL,DERATINGAPP_MIN_PERIOD_MS,0U,DERATINGAPP_MIN_CHECK_MS,DERATINGAPP_DEFAULT_OUTDATA},
    {NULL,NULL,NULL,NULL,DERATINGAPP_MIN_PERIOD_MS,0U,DERATINGAPP_MIN_CHECK_MS,DERATINGAPP_DEFAULT_OUTDATA},
    {NULL,NULL,NULL,NULL,DERATINGAPP_MIN_PERIOD_MS,0U,DERATINGAPP_MIN_CHECK_MS,DERATINGAPP_DEFAULT_OUTDATA},
};
/**
 * @brief To indicate a set of various dimming control for a specific derating method.
 * 
 * @details DeratingApp_DimmingControl_Routine() will calculate a proportion dimming data according to the set's content.
 * The set's content are modified by calling DeratingApp_RegisterDimmingControl().
 * 
 */
static DeratingApp_DimmingControl_Typedef gDimmingControlSet[DERATINGAPP_CONTROL_SIZE] = 
{
    {0U,NULL,0U,NULL,DERATINGAPP_MIN_PERIOD_MS,DERATINGAPP_DATA_INIT,DERATINGAPP_DATA_INIT,false,0U,0U,DERATINGAPP_DATA_INIT},
    {0U,NULL,0U,NULL,DERATINGAPP_MIN_PERIOD_MS,DERATINGAPP_DATA_INIT,DERATINGAPP_DATA_INIT,false,0U,0U,DERATINGAPP_DATA_INIT},
    {0U,NULL,0U,NULL,DERATINGAPP_MIN_PERIOD_MS,DERATINGAPP_DATA_INIT,DERATINGAPP_DATA_INIT,false,0U,0U,DERATINGAPP_DATA_INIT},
    {0U,NULL,0U,NULL,DERATINGAPP_MIN_PERIOD_MS,DERATINGAPP_DATA_INIT,DERATINGAPP_DATA_INIT,false,0U,0U,DERATINGAPP_DATA_INIT},
};

/* -- Local Functions -- */
/**
 * @brief To initialize the content of gDimmingControlSet.
 * 
 * @details To reset some variables which are include index,data difference,working flag...etc.
 * 
 * @note It is a local function.
 * 
 * @param DeratingItem A index of derating dimming control set.
 */
static void DeratingApp_DimmingControl_ReInit(uint8_t DeratingItem)
{

    if((gDimmingControlSet[DeratingItem].u32ptrProportion != NULL)\
    &&(gDimmingControlSet[DeratingItem].u32ptrUpdateProportionTime != NULL))
    {
        gDimmingControlSet[DeratingItem].DataDiff = DERATINGAPP_DATA_INIT;
        gDimmingControlSet[DeratingItem].bIsWorking = false;
        gDimmingControlSet[DeratingItem].u8ProportionIndex = 0U;
        gDimmingControlSet[DeratingItem].u32UpdateTime = 0U;
    }

}
/**
 * @brief To catch The derating back-light value this time.
 * 
 * @details To save the derating back-light value and calculate difference value this time.
 * 
 * @note It is a local function.
 * 
 * @param  DeratingItem The index of dimming control for a specific derating method.
 * 
 * @param  TargetData The target derating back-light this time.
 * 
 * @return float The derating back-light according to proportion.
 * 
 */
#if(DERATINGAPP_FLOAT_OPERATION)
static float DeratingApp_DimmingControl_Target_Set(DeratingApp_Item DeratingItem,float TargetData)
#else
static int16_t DeratingApp_DimmingControl_Target_Set(uint8_t DeratingItem,int16_t TargetData)
#endif
{
    if((gDimmingControlSet[DeratingItem].bIsWorking == false) &&\
        (TargetData != gDimmingControlSet[DeratingItem].BaseData) &&\
        (gDimmingControlSet[DeratingItem].u32ptrProportion != NULL) &&\
        (gDimmingControlSet[DeratingItem].u32ptrUpdateProportionTime != NULL))
    {
        gDimmingControlSet[DeratingItem].bIsWorking = true;
        gDimmingControlSet[DeratingItem].DataDiff = TargetData - gDimmingControlSet[DeratingItem].BaseData;
    }
    else
    {
        gDimmingControlSet[DeratingItem].OutputData = TargetData;
    }

    return gDimmingControlSet[DeratingItem].OutputData;
}
/**
 * @brief To calculate derating back-light value to each of various dimming control settings periodically
 * 
 * @details 1.Various dimming control settings are defined gDimmingControlSet.\n
 * 2.The maximal size of dimming control settings is defined DERATINGAPP_CONTROL_SIZE.\n
 * 3.The proportion and update time' pointer address must be not zero if the function could work.\n
 * 4.To call DeratingApp_DimmingControl_Target_Set() and catch derating back-light value.\n
 * 5.The function is called by DeratingApp_Routine() by default. 
 * 
 * @note It is a local function.
 * 
 */
static void DeratingApp_DimmingControl_Routine(void)
{
    uint8_t u8gDimmingControlSetIndex = 0;
#if(DERATINGAPP_FLOAT_OPERATION)
    float Offset = 0;
#else
    int16_t Offset = 0;
#endif
   
    for(u8gDimmingControlSetIndex = 0U ; u8gDimmingControlSetIndex < DERATINGAPP_CONTROL_SIZE ; u8gDimmingControlSetIndex++)
    {
        if((gDimmingControlSet[u8gDimmingControlSetIndex].u32ptrProportion != NULL)\
        &&(gDimmingControlSet[u8gDimmingControlSetIndex].u32ptrUpdateProportionTime != NULL))
        {
            if(gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionIndex >= gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionQuantity)
            {
                DeratingApp_DimmingControl_ReInit(u8gDimmingControlSetIndex);
                gDimmingControlSet[u8gDimmingControlSetIndex].BaseData = gDimmingControlSet[u8gDimmingControlSetIndex].OutputData;
            }
            else if((gDimmingControlSet[u8gDimmingControlSetIndex].DataDiff != 0)\
                &&(gDimmingControlSet[u8gDimmingControlSetIndex].u32ProportionSum != 0U))
            {
                /* Check Time */
                if(gDimmingControlSet[u8gDimmingControlSetIndex].u32UpdateTime <= 0U)
                {
#if(DERATINGAPP_FLOAT_OPERATION)
                    Offset = gDimmingControlSet[u8gDimmingControlSetIndex].DataDiff * \
                    ((float)gDimmingControlSet[u8gDimmingControlSetIndex].u32ptrProportion[gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionIndex] / (float)gDimmingControlSet[u8gDimmingControlSetIndex].u32ProportionSum);
#else
                    Offset = (gDimmingControlSet[u8gDimmingControlSetIndex].DataDiff * \
                    (int16_t)gDimmingControlSet[u8gDimmingControlSetIndex].u32ptrProportion[gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionIndex]) / (int16_t)gDimmingControlSet[u8gDimmingControlSetIndex].u32ProportionSum;
#endif
                    gDimmingControlSet[u8gDimmingControlSetIndex].OutputData = gDimmingControlSet[u8gDimmingControlSetIndex].BaseData + Offset;
                    gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionIndex++;
                }
                else
                {
                    /* Nothing */
                }
                /* Calculate Time */
                gDimmingControlSet[u8gDimmingControlSetIndex].u32UpdateTime = (gDimmingControlSet[u8gDimmingControlSetIndex].u32UpdateTime <= 0U)?\
                (gDimmingControlSet[u8gDimmingControlSetIndex].u32ptrUpdateProportionTime[gDimmingControlSet[u8gDimmingControlSetIndex].u8ProportionIndex]):\
                (gDimmingControlSet[u8gDimmingControlSetIndex].u32UpdateTime - gDimmingControlSet[u8gDimmingControlSetIndex].u32PeriodTime);
            }
            else
            {
                /* Dimming conteol element re-init */
                DeratingApp_DimmingControl_ReInit(u8gDimmingControlSetIndex);
            }
        }
        else
        {
            /* To send taget PWM */
            gDimmingControlSet[u8gDimmingControlSetIndex].OutputData = gDimmingControlSet[u8gDimmingControlSetIndex].DataDiff + gDimmingControlSet[u8gDimmingControlSetIndex].BaseData;
            DeratingApp_DimmingControl_ReInit(u8gDimmingControlSetIndex);
            gDimmingControlSet[u8gDimmingControlSetIndex].BaseData = gDimmingControlSet[u8gDimmingControlSetIndex].OutputData;
        }
    }

    (void)Offset;
}

/* -- Global Functions -- */
/**
 * @brief It checks whether the member of derating method set is vaild or not. 
 * 
 * @details 1.It checks whether each function pointer is zero or not.\n
 * 2.It checks whether time variables are out of limited range or not. 
 * 
 * @note It is a local function.
 * 
 * @param DeratingItem The index of derating method set. 
 *  
 * @return bool Is the member of derating method set vaild?
 * 
 * @retval true the member of derating method set is vaild.
 * 
 * @retval false the member of derating method set is not vaild.
 *  
 */
static bool DeratingApp_Method_Check(uint8_t DeratingItem)
{
    bool bResult = false;

    if((gDeratingAppControlSet[DeratingItem].InitMethod != NULL)\
        &&(gDeratingAppControlSet[DeratingItem].BootMethod != NULL)\
        &&(gDeratingAppControlSet[DeratingItem].DeratingMethod != NULL)\
        &&(gDeratingAppControlSet[DeratingItem].NormalMethod != NULL)\
        &&(gDeratingAppControlSet[DeratingItem].u16MethodPeriodTime >= DERATINGAPP_MIN_PERIOD_MS)\
        &&(gDeratingAppControlSet[DeratingItem].u16MethodCheckTime >= DERATINGAPP_MIN_CHECK_MS))
    {
        bResult = true;
    }
    else
    {
        bResult = false;
    }

    return bResult;
}
/**
 * @brief It is used for registering a specific derating type.
 * 
 * @details 1.To register a specific derating type.\n
 * 2.The register derating type incloud four derating state methods:init.,boot,derating,normal.
 * 
 * @note It is a global function.
 * 
 * @param DeratingApp_Item The index of dimming control for a specific derating method.
 * 
 * @param RegisterInitMethod The function pointer of derating method whose state is initial.
 * 
 * @param RegisterBootMethod The function pointer of derating method whose state is boot.
 * 
 * @param RegisterDeratingMethod The function pointer of derating method whose state is derating.
 * 
 * @param RegisterNormalMethod The function pointer of derating method whose state is normal.
 * 
 * @param u16PeriodTime The time value is used for accumulating time. 
 * When the accumulating time is equal to the checking time,
 * these derating method function would be called.
 * 
 * @param u16CheckTime The time value is used for these derating method function are called.
 * 
 */
void DeratingApp_RegisterElement(DeratingApp_Item DeratingItem,\
DERATINGAPP_STATUS_METHOD* RegisterInitMethod,\
DERATINGAPP_STATUS_METHOD* RegisterBootMethod,\
DERATINGAPP_STATUS_METHOD* RegisterDeratingMethod,\
DERATINGAPP_STATUS_METHOD* RegisterNormalMethod,\
uint16_t u16PeriodTime,\
uint16_t u16CheckTime)
{
    uint8_t u8DeratingItem = (uint8_t)(DeratingItem);
    
    gDeratingAppControlSet[u8DeratingItem].InitMethod = RegisterInitMethod;
    gDeratingAppControlSet[u8DeratingItem].BootMethod = RegisterBootMethod;
    gDeratingAppControlSet[u8DeratingItem].DeratingMethod = RegisterDeratingMethod;
    gDeratingAppControlSet[u8DeratingItem].NormalMethod = RegisterNormalMethod;
    gDeratingAppControlSet[u8DeratingItem].u16MethodPeriodTime = u16PeriodTime;
    gDeratingAppControlSet[u8DeratingItem].u16MethodCheckTime = u16CheckTime;
    gDeratingAppControlSet[u8DeratingItem].u16Methodu32UpdateTime = gDeratingAppControlSet[u8DeratingItem].u16MethodCheckTime;
}
/**
 * @brief To catch the minimal derating back-light value.
 * 
 * @details None.
 * 
 * @note It is a global function.
 * 
 * @return 1.float The minimal derating back-light value is within all derating types.\n
 * 2.The unit is %.
 * 
 */
#if(DERATINGAPP_FLOAT_OPERATION)
float DeratingApp_DeratingOutData_Get(void)
#else
int16_t DeratingApp_DeratingOutData_Get(void)
#endif
{
    uint8_t u8ControlSetIndex = 0;
#if(DERATINGAPP_FLOAT_OPERATION)
    float MinDeratingOutData = DERATINGAPP_DEFAULT_OUTDATA;
#else
    int16_t MinDeratingOutData = DERATINGAPP_DEFAULT_OUTDATA;
#endif

    for(u8ControlSetIndex = 0U ; u8ControlSetIndex < DERATINGAPP_CONTROL_SIZE ; u8ControlSetIndex++)
    {
        if((DeratingApp_Method_Check(u8ControlSetIndex))\
            &&(MinDeratingOutData > gDeratingAppControlSet[u8ControlSetIndex].DeratingDimmingOutputData))
        {
            MinDeratingOutData = gDeratingAppControlSet[u8ControlSetIndex].DeratingDimmingOutputData;
        }
        else
        {
            /* Nothing */
        }
    }

    return MinDeratingOutData;
}
/**
 * @brief To run a state machine mechanism of all derating types.
 * 
 * @details 1.To run a state machine mechanism automatically when it's register contect is vaild.\n
 * 2.It should be called cyclically,the cycle time is equal to the register period time.\n
 * 3.The final derating back-light value could be computed proportionally before saving it.
 * 
 * @note It is a global function.
 * 
 */
void DeratingApp_Routine(void)
{
    uint8_t u8ControlSetIndex = 0;

    for(u8ControlSetIndex = 0U ; u8ControlSetIndex < DERATINGAPP_CONTROL_SIZE ; u8ControlSetIndex++)
    {
        if(DeratingApp_Method_Check(u8ControlSetIndex))
        {            
            if(gDeratingAppControlSet[u8ControlSetIndex].u16Methodu32UpdateTime <= 0U)
            {
                /* Run status machine. */
                switch(gReturnMethodSet[u8ControlSetIndex].MethodStatus)
                {
                    case INIT_MODE:
                        gReturnMethodSet[u8ControlSetIndex] = (*gDeratingAppControlSet[u8ControlSetIndex].InitMethod)();
                    break;

                    case BOOT:
                        gReturnMethodSet[u8ControlSetIndex] = (*gDeratingAppControlSet[u8ControlSetIndex].BootMethod)();                        
                    break;
                    
                    case DERATING:
                        gReturnMethodSet[u8ControlSetIndex] = (*gDeratingAppControlSet[u8ControlSetIndex].DeratingMethod)();
                        gDeratingAppControlSet[u8ControlSetIndex].DeratingDimmingOutputData = \
                            DeratingApp_DimmingControl_Target_Set(u8ControlSetIndex,gReturnMethodSet[u8ControlSetIndex].DeratingOutData);
                    break;
                    
                    case NORMAL:
                        gReturnMethodSet[u8ControlSetIndex] = (*gDeratingAppControlSet[u8ControlSetIndex].NormalMethod)();
                    break;
                    
                    default:
                        /* Do nothing */
                    break;
                }
            }
            else
            {
                /* Wait update time is euqal to zero. */
            }
            /* Calculate Update Time. */
            gDeratingAppControlSet[u8ControlSetIndex].u16Methodu32UpdateTime = (gDeratingAppControlSet[u8ControlSetIndex].u16Methodu32UpdateTime <= 0U)?\
            (gDeratingAppControlSet[u8ControlSetIndex].u16MethodCheckTime):\
            (gDeratingAppControlSet[u8ControlSetIndex].u16Methodu32UpdateTime - gDeratingAppControlSet[u8ControlSetIndex].u16MethodPeriodTime);
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Derating Dimming Routine */
    DeratingApp_DimmingControl_Routine();
}
/**
 * @brief It is used for registering dimming control of a specific derating type.
 * 
 * @details 1.To register a dimming control.\n
 * 2.The roportional dimming control size is more than one.\n
 * 3.It checks whether setting array pointer is zero or not.
 * 
 * @note It is a global function. 
 * 
 * @param DeratingItem The index of dimming control for a specific derating method.
 * 
 * @param u8SettingDataSize The proportional dimming control size.
 * 
 * @param u32rProportionLevel A reference of the proportional percetage datas.
 * 
 * @param u32rProportionMsTime A reference of the proportional update times.
 * 
 * @return bool Is the member of dimming control set vaild?
 * 
 * @retval true the member of dimming control set is vaild.
 * 
 * @retval false the member of dimming control set is not vaild.
 * 
 */
bool DeratingApp_RegisterDimmingControl(DeratingApp_Item DeratingItem,uint8_t u8SettingDataSize,uint32_t* u32rProportionLevel,uint32_t* u32rProportionMsTime)
{
    uint8_t u8DeratingItem = (uint8_t)(DeratingItem);
    bool bResult = false;

    if((u8SettingDataSize == 0U) && (u32rProportionLevel == NULL) && (u32rProportionMsTime == NULL))
    {
        bResult = false;
    }
    else
    {
#if(DERATINGAPP_FLOAT_OPERATION)
        gDimmingControlSet[u8DeratingItem].DataDiff = 0.0f;
#else
        gDimmingControlSet[u8DeratingItem].DataDiff = 0;
#endif
        gDimmingControlSet[u8DeratingItem].BaseData = DERATINGAPP_DEFAULT_OUTDATA;
        gDimmingControlSet[u8DeratingItem].bIsWorking = false;
        gDimmingControlSet[u8DeratingItem].u32ptrProportion = u32rProportionLevel;
        gDimmingControlSet[u8DeratingItem].u8ProportionIndex = 0U;
        gDimmingControlSet[u8DeratingItem].u8ProportionQuantity = u8SettingDataSize;
        gDimmingControlSet[u8DeratingItem].u32ProportionSum = *(gDimmingControlSet[u8DeratingItem].u32ptrProportion + gDimmingControlSet[u8DeratingItem].u8ProportionQuantity - 1U);
        gDimmingControlSet[u8DeratingItem].OutputData = DERATINGAPP_DEFAULT_OUTDATA;
        gDimmingControlSet[u8DeratingItem].u32ptrUpdateProportionTime = u32rProportionMsTime;
        gDimmingControlSet[u8DeratingItem].u32UpdateTime = *u32rProportionMsTime;
        bResult = true;
    }

    return bResult;
}
/* -- END -- */
