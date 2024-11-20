/* -- Avoid Re-Includes -- */
#ifndef M_DERATING_H
#define M_DERATING_H

#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes -- */
#include "../../Mymain.h"

/* -- Marco Define -- */
#define DERATINGAPP_FLOAT_OPERATION false /* It is used for supporting float operation. */

#define DERATINGAPP_CONTROL_SIZE 4U /* It is used for limiting the maximal size of derating types which coulld be registered. */
#define DERATINGAPP_MIN_PERIOD_MS 10U /* It take period time to call DeratingApp_Routine(). */
#define DERATINGAPP_MIN_CHECK_MS 20U /* It take check time to run derating state machine mechanism. */

#if(DERATINGAPP_FLOAT_OPERATION)
#define DERATINGAPP_DATA_INIT 0.0f /* The derating back-light default value. */
#else
#define DERATINGAPP_DATA_INIT 0 /* The derating back-light default value. */
#endif

/* -- Data Type Define -- */
/**
 * @brief It is used for defining derating type.
 * 
 */
typedef enum
{
    AMBIENT_TEMPERATURE = 0U, /**< 0 */
    RESERVED0 = 1U, /**< 1 */
    RESERVED1 = 2U, /**< 2 */
    RESERVED2 = 3U, /**< 3 */
}DeratingApp_Item;
/**
 * @brief It is used for defining each step of derating state machine.
 * 
 */
typedef enum
{
    INIT_MODE = 0U, /**< 0 */
    BOOT = 1U, /**< 1 */
    DERATING = 2U, /**< 2 */
    NORMAL = 3U, /**< 3 */
}DeratingApp_Status;
/**
 * @brief It is used for returning date when each state function is called successfully.
 * 
 */
typedef struct
{
    DeratingApp_Status MethodStatus; /**< It to indicate the state function of derating method next time. */
#if(DERATINGAPP_FLOAT_OPERATION)
    float DeratingOutData; /**< It to indicate minimal derating back-light value this time. */
#else
    int16_t DeratingOutData; /**< It to indicate minimal derating back-light value this time. */
#endif
}DeratingApp_ReturnMethod_Typedef;
/**
 * @brief The function pointer type of derating method.
 * 
 */
typedef DeratingApp_ReturnMethod_Typedef(DERATINGAPP_STATUS_METHOD)(void); /**< The function pointer type does not need to parameter and returns MethodStatus and DeratingOutData. */

/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern void DeratingApp_RegisterElement(\
    DeratingApp_Item DeratingItem,\
    DERATINGAPP_STATUS_METHOD* RegisterInitMethod,\
    DERATINGAPP_STATUS_METHOD* RegisterBootMethod,\
    DERATINGAPP_STATUS_METHOD* RegisterDeratingMethod,\
    DERATINGAPP_STATUS_METHOD* RegisterNormalMethod,\
    uint16_t u16PeriodTime,\
    uint16_t u16CheckTime);
extern void DeratingApp_Routine(void);
extern bool DeratingApp_RegisterDimmingControl(\
    DeratingApp_Item DeratingItem,\
    uint8_t u8SettingDataSize,\
    uint32_t* u32rProportionLevel,\
    uint32_t* u32rProportionMsTime);
#if(DERATINGAPP_FLOAT_OPERATION)
extern float DeratingApp_DeratingOutData_Get(void);
#else
extern int16_t DeratingApp_DeratingOutData_Get(void);
#endif

#endif
/* -- END -- */
