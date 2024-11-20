/* -- Includes -- */
#include "M_FixedFlashAccess.h"

/* -- Marco Define -- */
#define FIXED_FLASH_ACCESS_FLASH_ADDRESS 0x0001E000UL

#define FIXED_FLASH_ACCESS_ROW_BYTE_SIZE CY_FLASH_SIZEOF_ROW /* < 256 Bytes. */
#define FIXED_FLASH_ACCESS_ROW_SIZE 1U
#define FIXED_FLASH_ACCESS_TOTLA_BYTE_SIZE (CY_FLASH_SIZEOF_ROW * FIXED_FLASH_ACCESS_ROW_SIZE)

/* -- Type Define -- */

/* -- Global Variables -- */
static uint8_t MFixedFlashAccess_Buffer[FIXED_FLASH_ACCESS_TOTLA_BYTE_SIZE] = {0x00};

/* -- Local Functions -- */

/* -- Global Functions -- */
bool MFixedFlashAccess_WritePage(uint32_t u32AddrOffset, uint8_t *ptrExternalSetBuffer, uint8_t u8ExternalSetBufferSize)
{
    bool bReturn;   
    if(((u32AddrOffset % FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE) != 0)\
        || (u32AddrOffset > (FIXED_FLASH_ACCESS_TOTLA_BYTE_SIZE - FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE))\
        || (ptrExternalSetBuffer == NULL)\
        || (u8ExternalSetBufferSize > FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE))
    {
        bReturn = false;
    }
    else
    {
        /* Read Total Flash */
        memcpy(MFixedFlashAccess_Buffer, (const void *)FIXED_FLASH_ACCESS_FLASH_ADDRESS, FIXED_FLASH_ACCESS_TOTLA_BYTE_SIZE);
        /* Recover Page Data */
        memcpy((MFixedFlashAccess_Buffer + u32AddrOffset), ptrExternalSetBuffer, u8ExternalSetBufferSize);
        /* Write Flash */
        if(CY_FLASH_DRV_SUCCESS != Cy_Flash_WriteRow(FIXED_FLASH_ACCESS_FLASH_ADDRESS , (uint32_t *)MFixedFlashAccess_Buffer))
        {
            bReturn = false;
        }
        else
        {
            bReturn = true;
        }
    }
    return bReturn;
}

bool MFixedFlashAccess_ReadPage(uint32_t u32AddrOffset, uint8_t *ptrExternalGetBuffer, uint8_t u8ExternalGetBufferSize)
{
    bool bReturn;

    if(((u32AddrOffset % FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE) != 0)\
        || (u32AddrOffset > (FIXED_FLASH_ACCESS_TOTLA_BYTE_SIZE - FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE))\
        || (ptrExternalGetBuffer == NULL)\
        || (u8ExternalGetBufferSize > FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE))
    {
        bReturn = false;
    }
    else
    {
        memcpy((void *)ptrExternalGetBuffer, (const void *)(FIXED_FLASH_ACCESS_FLASH_ADDRESS + u32AddrOffset), u8ExternalGetBufferSize);
        bReturn = true;
    }
    return bReturn;
}
