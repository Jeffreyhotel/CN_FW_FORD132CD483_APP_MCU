
#ifndef MCUDIAGAPP_H    /* Guard against multiple inclusion */
#define MCUDIAGAPP_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../../Mymain.h"
#include "ICDiagApp.h"

#if(BACKDOOR_ICDIAG_OPEN)

//void MCUDIAG_MemRW(uint32_t u32DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen);
void MCUDIAG_NVMRW(uint32_t u32DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen);
void MCUDIAG_I2cMaster_ICDiag(uint8_t u8DeviceAddr, uint8_t u8Channel, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen);
void MCUDIAG_MemRW(uint8_t *u8DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen);

#endif
#endif

