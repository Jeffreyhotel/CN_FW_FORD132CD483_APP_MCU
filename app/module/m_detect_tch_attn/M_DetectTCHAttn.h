/* -- Avoid Re-Includes -- */
#ifndef M_DETECTTCHATTN_H
#define M_DETECTTCHATTN_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes -- */
#include "../../Mymain.h"

/* -- Marco Define -- */
#define M_DETECT_TCH_ATTN_EX_INT false

#if(M_DETECT_TCH_ATTN_EX_INT)
#define M_DETECT_TCH_ATTN_FIX_LOST_EX true
#define M_DETECT_TCH_ATTN_NO_DEBUNCE false
#else
#define M_DETECT_TCH_ATTN_FIX_LOST_EX true
#define M_DETECT_TCH_ATTN_NO_DEBUNCE false
#endif

#if(M_DETECT_TCH_ATTN_FIX_LOST_EX)
#define M_DETECT_TCH_ATTN_ROUTINE_TIME 2 /* Cyclic time of MDetectTchAttn_Routine2ms() and uint is ms. */
#define M_DETECT_TCH_ATTN_KEEP_LOW_TIME_THRESHOLD 4 /* ATTN pin keeps low Time and unit is ms. */
#define M_DETECT_TCH_ATTN_KEEP_HIGH_TIME_THRESHOLD 4 /* ATTN pin keeps low Time and unit is ms. */
#endif
/**
 * @brief 
 * 
 */
typedef enum
{
    TCH_CONTROLLER_READY = 0U,
    TCH_CONTROLLER_NOTREADY,
}MDetectTchAttn_TouchController_E;
/**
 * @brief Decides ATTN Trigger Type.
 * 
 */
typedef enum
{
    ATTN_TRI_FALLING = 0U,
    ATTN_TRI_RISING,
}MDetectTchAttn_ATTNTriggerType_E;

/* -- Data Type Define -- */
typedef MDetectTchAttn_TouchController_E (*CALLBACK_TCH_CONTROLLER_ST_GET)(void);
typedef uint8_t (*CALLBACK_TCH_ATTN_DI_GET)(void);
typedef void (*CALLBACK_TCH_CLICK)(void);
typedef void (*CALLBACK_TCH_CLICK_RELEASE)(void);

/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern bool MDetectTchAttn_Register(\
CALLBACK_TCH_CONTROLLER_ST_GET CallbackTchControllerGet,\
CALLBACK_TCH_ATTN_DI_GET CallbackTchAttnDiGet,\
CALLBACK_TCH_CLICK CallbackTchClick,\
CALLBACK_TCH_CLICK_RELEASE CallbackTchClickRel,\
MDetectTchAttn_ATTNTriggerType_E eAttnTriType);

extern void MDetectTchAttn_Routine2ms(void);

#endif
/* -- END -- */
