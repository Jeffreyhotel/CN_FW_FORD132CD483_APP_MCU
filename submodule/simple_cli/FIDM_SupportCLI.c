/*
 * FIDM_SupportCMD.c
 *
 *  Created on: 2022年9月27日
 *      Author: JackyHWWu
 */
//#include <stdlib.h>
#include "hal_uart.h"
#include "simple_cli.h"
#include "FIDM_SupportCLI.h"
#include "Memory_Pool.h"
#include "M_BacklightControl.h"

#ifdef DEBUG_UART_EN

#if (HIJACK_ADC)
int CMD_Derating(int argc , char * argv []);
#endif
int CMD_LED_ON(int argc , char * argv []);
int CMD_LED_OFF(int argc , char * argv []);
int CMD_Enter_Bootloader(int argc , char * argv []);
int CMD_List(int argc , char * argv []);
int CMD_TDDI(int argc , char * argv []);
int CMD_Version (int argc , char * argv []);


SCLI_initCB cli_cb;

static CMD g_cmdtb[] =
{   
#if (HIJACK_ADC)
	{"derating" , CMD_Derating},
#endif
    {"led_on", CMD_LED_ON},
    {"led_off",CMD_LED_OFF},
    {"enboot", CMD_Enter_Bootloader},
    {"version", CMD_Version},
    {"ls",CMD_List},
    {"tddi",CMD_TDDI},
};

#define NUM_OF_CMD (sizeof(g_cmdtb)/sizeof(g_cmdtb[0]))


#if (HIJACK_ADC)
int CMD_Derating(int argc , char * argv [])
{
	uint8_t u8BLState = 0u, u8Len = 0u, u8Loop = 0u, u8Mask = 0u;
	int16_t i16PCBATemperature;
	char number[10U] = {0x00U};



	if( 0U < argc )
	{
		if( 0U == strcmp("on",argv[1U]) )
		{
			MBacklightControl_ADCHiJack((uint8_t)0x01U);
		}
		else if( 0U == strcmp("off",argv[1U]) )
		{
			MBacklightControl_ADCHiJack((uint8_t)0x00U);
		}
		else if( 0U == strcmp("lv",argv[1U]) )
		{
			//i16PCBATemperature = atoi(argv[2U]);

			strcpy(number, argv[2U]);
			u8Len = strlen(number);
			u8Mask = 1u;
			while((u8Len - 1u) > 0u)
			{
				u8Mask *= 10u;
				u8Len--;
			};

			i16PCBATemperature = 0u;
			for(u8Loop = 0u ; u8Loop<strlen(number) ; u8Loop++)
			{
				i16PCBATemperature += ((number[u8Loop] - 0x30u) * (u8Mask) );
				u8Mask /= 10U;
			}
			HAL_UART_Printf("fake ADC:%d\r\n",i16PCBATemperature);
			MBacklightControl_SetHADC(i16PCBATemperature);
		}
		else if( 0U == strcmp("+",argv[1U]) )
		{
			MBacklightControl_SetHADC((MBacklightControl_GetHADC() + 1u));
		}
		else if( 0U == strcmp("-",argv[1U]) )
		{
			MBacklightControl_SetHADC((MBacklightControl_GetHADC() - 1u));
		}
		else
		{
			HAL_UART_Printf("No support parameter\r\n");
		}

		i16PCBATemperature = Memory_Pool_PCBATemp_Get();
		u8BLState = (uint8_t)MBacklightControl_GetBacklightState();

		HAL_UART_Printf("PCBA Temperature, Hex:0x%04xh, Dec:%d\r\n",i16PCBATemperature, i16PCBATemperature);
		HAL_UART_Printf("BL State:%02x\r\n",u8BLState);
	}
	else
	{
		HAL_UART_Printf("Miss parameter\r\n"); /* no parameters */
	}

	UNUSED(argc);
	UNUSED(argv);

    return 0;
}
#endif

int CMD_LED_ON(int argc , char * argv [])
{
    UNUSED(argc);
    UNUSED(argv);
    HAL_UART_Printf("LED ON\r\n");
    //G_LED_Write(0);
    return 0;
}

int CMD_LED_OFF(int argc , char * argv [])
{
    UNUSED(argc);
    UNUSED(argv);
    HAL_UART_Printf("LED OFF\r\n");
    //G_LED_Write(1);
    return 0;
}

int CMD_Enter_Bootloader(int argc , char * argv [])
{
    UNUSED(argc);
    UNUSED(argv);
    HAL_UART_Printf("Enter UART bootloader wait forever ... \r\n");
    //CyDelay(10); // wait for iprintf
    //Bootloadable_Load();
    return 0;
}

int CMD_List(int argc , char * argv [])
{
    UNUSED(argc);
    UNUSED(argv);
    HAL_UART_Printf("List of CLI commands -->\r\n");
    for (int i = 0 ; i  < (int)NUM_OF_CMD ; i++)
    {
    	HAL_UART_Printf ("%s\r\n",g_cmdtb[i].name);
    }
    return 0;
}
int CMD_TDDI(int argc , char * argv [])
{
	uint8_t u8VGMA[4U] = {0x00U};
	uint32_t u32VcomTmp = 0U, u8VCOM[3U] = {0x00U};
	uint8_t u8Loop = 0U, u8Reg = 0U;

	if( 0U < argc )
	{
		if( 2U == argc )
		{
			u32VcomTmp = Memory_Pool_NT51926_Vcom_Get();
			u8VCOM[0U] = (uint8_t)((u32VcomTmp >> 16U) & 0xFF);
			u8VCOM[1U] = (uint8_t)(u32VcomTmp & 0xFF);
			u8VCOM[2U] = (uint8_t)((u32VcomTmp >> 8U) & 0xFF);
			if( 0 > Memory_Pool_NT51926_VGMA_Get(u8VGMA))
			{
				; /* fail */
			}
			else
			{
				if( 0U == strcmp("all",argv[1U]) )
				{
					HAL_UART_Printf("VCOM:",u32VcomTmp);
					u8Reg = 0x01U;
					for( u8Loop = 0U ; u8Loop < 3U ; u8Loop++)
					{
						HAL_UART_Printf("[%02xh:0x%02x]",u8Reg, u8VCOM[u8Loop]);
						u8Reg++;
					}
					u8Reg = 0x05U;
					HAL_UART_Printf("\r\nVGMA:");
					for( u8Loop = 0U ; u8Loop < 3U ; u8Loop++)
					{
						HAL_UART_Printf("[%02xh:0x%02x]",u8Reg, u8VGMA[u8Loop]);
						u8Reg++;
					}
					HAL_UART_Printf("\r\n");
				}
				else if( 0U == strcmp("vcom",argv[1U]) )
				{
					HAL_UART_Printf("VCOM:",u32VcomTmp);
					u8Reg = 0x01U;
					for( u8Loop = 0U ; u8Loop < 3U ; u8Loop++)
					{
						HAL_UART_Printf("[%02xh:0x%02x]",u8Reg, u8VCOM[u8Loop]);
						u8Reg++;
					}
					HAL_UART_Printf("\r\n");
				}
				else if( 0U == strcmp("vgma",argv[1U]) )
				{
					u8Reg = 0x05U;
					HAL_UART_Printf("VGMA:");
					for( u8Loop = 0U ; u8Loop < 3U ; u8Loop++)
					{
						HAL_UART_Printf("[%02xh:0x%02x]",u8Reg, u8VGMA[u8Loop]);
						u8Reg++;
					}
					HAL_UART_Printf("\r\n");
				}
				else
				{
					HAL_UART_Printf("No support parameter\r\n");
				}
			}
		}
		else
		{
			;
		}
	}
	else
	{
		HAL_UART_Printf("Miss parameter\r\n"); /* no parameters */
	}

    UNUSED(argc);
    UNUSED(argv);

    return 0;
}

int CMD_Version (int argc , char * argv [])
{
	uint8_t u8MCU[11] = {0x00U};

	if( 0U < argc )
	{
		if( 2U == argc )
		{
			if( 0U > Get_MCUVersion(u8MCU) )
			{
				HAL_UART_Printf("Can't get MCU version\r\n");
			}
			else
			{
				; /* got MCU version */
			}
			if( 0U == strcmp("all",argv[1U]) )
			{
				HAL_UART_Printf("MCU:%s\r\n",u8MCU);
			}
			else if( 0U == strcmp("mcu",argv[1U]) )
			{
				HAL_UART_Printf("MCU:%s\r\n",u8MCU);
			}
			else if( 0U == strcmp("tddi",argv[1U]) )
			{
				/* TODO: */
				HAL_UART_Printf("TDDI:\r\n");
			}
			else
			{
				HAL_UART_Printf("No support parameter\r\n");
			}
		}
		else
		{
			;
		}
	}
	else
	{
		HAL_UART_Printf("Miss parameter\r\n"); /* no parameters */
	}

	UNUSED(argc);
	UNUSED(argv);
    return 0;
}


void FIDM_CLI_Init(void)
{
    cli_cb.scb_getchar = UART_GetChar;
    cli_cb.scb_putchar = UART_PutChar;
    SCLI_Init(&cli_cb,g_cmdtb,NUM_OF_CMD);
}

#endif
