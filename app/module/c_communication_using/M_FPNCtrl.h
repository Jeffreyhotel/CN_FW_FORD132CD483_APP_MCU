#ifndef M_FPNCTRL_H
#define M_FPNCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes -- */
#include "public.h"
#include "Memory_Pool.h"

/* -- Marco Define -- */
#define FPN_SIZE            25U
#define FPN_BYTE_SIZE        1U

#define FPN_WRITE_SUCCESS   1U
#define FPN_WRITE_FAIL      2U
#define FPN_CHECKSUM_ERR    4U
#define FPN_WRITE_NO_DATA   8U

#define FPN_CHECKSUM_INDEX  FPN_SIZE
#define FPN_CHECK_MASK      0xFF

#define FPN_DISABLE         0x00U
#define FPN_ENABLE          0x01U

typedef struct
{
    uint8_t FPNCtrlFlag : 1 ;
    uint8_t FPNDataFlag : 1 ;
    uint8_t RESERVED : 6 ;
}__attribute__((__packed__)) tFPN_flag_def;

typedef struct
{
    tFPN_flag_def FPN_Flag_t;
    tfpn_status_info_def FPN_Status_t;
    uint8_t *pFPNDataBuff;
    uint8_t FPNDataLength;
}tFPN_ctrl_def;

typedef struct
{
    tFPN_flag_def FPN_Flag_t;
    tProduct_pn_status_info_def FPN_Status_t;
    uint8_t *pFPNDataBuff;
    uint8_t FPNDataLength;
}tFPN_production_byte_def;

/* -- Data Type Define -- */


/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern uint8_t M_FPNCtrl_DeliveryFPNControl_Process(tFPN_ctrl_def *fpn_t);
extern uint8_t M_FPNCtrl_SerNumFPNControl_Process(tFPN_ctrl_def *fpn_t);
extern uint8_t M_FPNCtrl_ProductionByteFPNControl_Process(tFPN_production_byte_def *fpn_t);

#endif
/* -- END -- */

