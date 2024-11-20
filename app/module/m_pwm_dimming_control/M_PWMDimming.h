/* -- Avoid Re-Includes -- */
#ifndef M_PWMDIMMING_H
#define M_PWMDIMMING_H

#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes -- */
#include "../../Mymain.h"

/* -- Marco Define -- */
typedef enum
{
    eProcess_Ready = 0U, /**< It is ready to dimming. */
    eDimmingCal_finished, /**< It is finished to dimming. */
    eNonDimmingCtrl_finished /**< It is no dimming. */
}MPWMDimmingBLBrightness_E;

typedef enum
{
    MPWMDIMMING_CHANGEPWM_STOP = 0U,
    MPWMDIMMING_CHANGEPWM_RUN,
}MPWMDimmingChangePWM_E;

/* -- Data Type Define -- */
typedef void (*CallbackPWMDriverCtrl)(uint16_t u16PWMSignal);

/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern void MPWMDimming_RegisterPWMDriverCtrl(CallbackPWMDriverCtrl CallbackPWMDriverFunc);
extern uint16_t MPWMDimming_CurrentBLPWM10bit_Get (void);
extern MPWMDimmingBLBrightness_E MPWMDimming_BLBrightness_Ctrl(uint8_t u8BLPWMVal_8_9,uint8_t u8BLPWMVal_0_7,uint8_t u8DimmingEnable);
extern MPWMDimmingChangePWM_E MPWMDimming_ChangePWMStatus_Get(void);
extern void MPWMDimming_Routine6ms(void);

#endif
/* -- END -- */
