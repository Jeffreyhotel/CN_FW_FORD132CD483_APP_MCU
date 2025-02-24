/******************************************************************************
 ;       Program		: public.h
 ;       Function	:
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		: 2021 / 10 / 07
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef PUBLIC_H
#define PUBLIC_H
/*---------------------------- Include File ---------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "cy_pdl.h"
#include "cybsp.h"
#include "cycfg_pins.h"
#include "cycfg_peripherals.h"
#include "system.h"
#include "task.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_i2c_master.h"
#include "hal_i2c_slave.h"
#include "hal_i2c_master_slave.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "hal_pwm.h"
#include "hal_exit.h"

#include "CRC8.h"
#include "Common.h"
#include "M_TemperatureDerating.h"
#include "Mymain.h"
/*---------------------------- Definition -----------------------------------*/

#define UNUSED(x) ( (void)(x) )

#define BIT0	0x00000001
#define BIT1	0x00000002
#define BIT2	0x00000004
#define BIT3	0x00000008
#define BIT4	0x00000010
#define BIT5	0x00000020
#define BIT6	0x00000040
#define BIT7	0x00000080
#define BIT8	0x00000100
#define BIT9	0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000


#define DRIVER_FALSE						0x00U
#define DRIVER_TRUE							0x01U

#define LOW_NIBBLE_MASK 					0x00FFU
#define HI_NIBBLE_MASK 						0xFF00U

#if (BACKDOOR_ICDIAG_OPEN ==true)
	#define BUFFER_SIZE 					80U
#else
	#define BUFFER_SIZE 					32U
#endif

/*Read/Write Action*/
#define I2C_WRITE							0x00U
#define I2C_READ							0x01U

/*Data Byte Format Position*/
#define CMD_SUBADDRESS_POS					0x00U
#define CMD_DATA1_POS						0x01U
#define CMD_DATA2_POS						0x02U

/*Display Status Shift*/
#define SHIFT_LCDERR_POS					0x00U
#define SHIFT_BLERR_POS						0x01U
#define SHIFT_TERR_POS						0x02U
#define SHIFT_DCERR_POS						0x03U
#define SHIFT_RST_RQ_POS					0x04U
#define SHIFT_LLOSS_POS						0x05U
#define SHIFT_TSCERR_POS					0x06U
#define SHIFT_TCERR_POS						0x07U

#define SHIFT_DISP_ST_POS					0x00U
#define SHIFT_TSC_ST_POS					0x01U
#define SHIFT_INIT_POS						0x02U
#define SHIFT_BL_ST_POS						0x06U
#define SHIFT_DISPERR_POS					0x07U

#define SHIFT_HSD_POS						0x00U
#define SHIFT_VSD_POS						0x01U

#define SHIFT_DISP_EN_POS					0x00U
#define SHIFT_TSC_EN_POS					0x01U

#define SHIFT_SHDWN_POS						0x00U

#define SHIFT_INT_ERR_POS					0x00U
#define SHIFT_INT_BTN_POS					0x01U
#define SHIFT_INT_TCH_POS					0x02U
#define SHIFT_INT_ROT_POS					0x03U

#define BIT_HSD_MASK						0x01U
#define BIT_VSD_MASK						0x02U

#define BIT_DISP_EN_POS						0x01U
#define BIT_TSC_EN_POS						0x02U

#define BIT_SHDWN_POS						0x01U

#define BIT_INT_ERR_POS						0x01U
#define BIT_INT_BTN_POS						0x02U
#define BIT_INT_TCH_POS						0x04U
#define BIT_INT_ROT_POS						0x08U

/*Display Status Bit Format Position*/
#define BIT_LCDERR_POS						0x00000001UL
#define BIT_BLERR_POS						0x00000002UL
#define BIT_TERR_POS						0x00000004UL
#define BIT_DCERR_POS						0x00000008UL
#define BIT_RST_RQ_POS						0x00000010UL
#define BIT_LLOSS_POS						0x00000020UL
#define BIT_TSCERR_POS						0x00000040UL
#define BIT_TCERR_POS						0x00000080UL
#define BIT_DISP_ST_POS						0x00000100UL
#define BIT_TSC_ST_POS						0x00000200UL
#define BIT_INIT_POS						0x00000400UL
#define BIT_BL_ST_POS						0x00004000UL
#define BIT_DISPERR_POS						0x00008000UL
#define BIT_ALL_ERROR_POS					(BIT_TCERR_POS | BIT_TSCERR_POS | BIT_LLOSS_POS | BIT_DCERR_POS | BIT_TERR_POS | BIT_BLERR_POS | BIT_LCDERR_POS | BIT_DISPERR_POS)

/*FPN Lock Bit Format Position*/
#define BIT_WRT_ST_POS						0x01U
#define BIT_INT_WRT_POS						0x02U
#define BIT_CKSUM_ERR_POS					0x04U

/*De-rating Enable Bit Format Position*/
#define BIT_DERATING_EN_POS					0x01U

/*Custom Command*/
#define CMD_DISPLAY_STATUS					0x00U
#define CMD_DISPLAY_IDENTIFICATION			0x01U
#define CMD_BACKLIGHT_PWM					0x02U
#define CMD_DISPLAY_SCANNING				0x03U
#define CMD_DISPLAY_ENABLE					0x04U
#define CMD_DISPLAY_SHUTDOWN				0x05U
#define CMD_INTERRUPT_STATUS				0x30U
#define CMD_CORE_ASSEMBLY					0x31U
#define CMD_DELIVERY_ASSEMBLY				0x32U
#define CMD_SOFT_PART_NUMBER				0x33U
#define CMD_SERIAL_NUMBER					0x34U
#define CMD_MAIN_CALI_PART_NUMBER			0x35U
#define CMD_DETIAL_DIAGNOSIS_GET			0xA3U

#define CMD_FACTORY_MODE					0xB1U
#define CMD_TEMPERATURE_GET					0xB2U

#define CMD_VOLTAGE_GET						0xB8U
#define CMD_DERATING_ENABLE					0xBAU
#define CMD_PRESENT_BACKLIGHT_PWM_GET		0xBCU
#define CMD_PRODUCTION_PHASE_BYTE_GET		0xBDU
#define CMD_VCOM_VALUE_GET					0xBEU

#define CMD_LOCK_DELIVERY_ASSEMBLY			0xC0U
#define CMD_DELIVERY_ASSEMBLY_DATA			0xC1U

#define CMD_LOCK_SERIAL_NUMBER				0xC2U
#define CMD_SERIAL_NUMBER_DATA				0xC3U

#define CMD_LOCK_PRODUCTION_PHASE_BYTE		0xC4U
#define CMD_PRODUCTION_PHASE_BYTE_DATA		0xC5U

#define CMD_PCBATEMPINFO					0xC9U
#define CMD_BLTEMPINFO						0xCAU
#define CMD_BATTVOLINFO						0xCBU
#define CMD_SYNCVOLINFO						0xCCU
#define CMD_FPCTXOUTVOLINFO					0xCDU
#define CMD_FPCRXOUTVOLINFO					0xCEU

#define CMD_UPDATE_REQUEST					0xE4U
#define CMD_MCU_VERSION_GET					0xDFU
#define CMD_JUMP_TO_BOOTLOADER				0xF4U

#if (BACKDOOR_WRITE_DERATINGDATA)
#define CMD_DERATING_LIMITED_TEMPERATURES_GET 0xE0U
#define CMD_DERATING_TABLE_GET 0xE1U
#define CMD_DERATING_DATAS_SET 0xE2U
#endif

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
#define CMD_DIAGNOSIS_SIMULATE				0xEEU
#endif

/*Custom Command Length*/
#define LEN_SUBADDRESS						0x01U

/*Custom Command Data Length*/
#if(FORD_I2CCOMV1P9)
#define LEN_DISPLAY_STATUS					0x02U
#else
#define LEN_DISPLAY_STATUS					0x03U
#endif
#define LEN_DISPLAY_IDENTIFICATION			0x02U
#define LEN_BACKLIGHT_PWM					0x02U
#define LEN_DISPLAY_SCANNING				0x01U
#define LEN_DISPLAY_ENABLE					0x01U
#define LEN_DISPLAY_SHUTDOWN				0x01U
#define LEN_INTERRUPT_STATUS				0x01U
#define LEN_CORE_ASSEMBLY					0x19U
#define LEN_DELIVERY_ASSEMBLY				0x19U
#define LEN_SOFT_PART_NUMBER				0x19U
#define LEN_SERIAL_NUMBER					0x19U
#define LEN_MAIN_CALI_PART_NUMBER			0x19U
#define LEN_DETIAL_DIAGNOSIS 				0x11U
#define LEN_TOUCH_RESET						0x01U
#define LEN_FACTORY_MODE					0x01U
#define LEN_TD7800_DATA						0x01U
//#if (BX726_TDDI_NT51926)
#define LEN_TEMPERATURE_GET					0x05U
//#else
//#define LEN_TEMPERATURE_GET					0x04U
//#endif
#define LEN_TEMPERATURE_ADC_GET				0x04U
#define LEN_POWER_ERROR_STATUS				0x01U
#define LEN_VOLTAGE_GET						0x04U
#define LEN_DERATING_ENABLE					0x01U
#define LEN_WATCHDOG_ENABLE					0x01U
#define LEN_PRESENT_BACKLIGHT_PWM_GET		0x02U
#define LEN_MCU_VERSION_GET					0x0AU

#define LEN_LOCK_SOFT_PART_NUMBER			0x01U
#define LEN_SOFT_PART_NUMBER_WRITE			0x1AU

#define LEN_LOCK_CORE_ASSEMBLY				0x01U
#define LEN_CORE_ASSEMBLY_WRITE				0x1AU

#define LEN_LOCK_MAIN_CALI_PART_NUMBER		0x01U
#define LEN_MAIN_CALI_PART_NUMBER_WRITE		0x1AU

#define LEN_LOCK_DISPLAY_IDENTIFICATION		0x01U
#define LEN_DISPLAY_IDENTIFICATION_WRITE	0x1AU

#define LEN_LOCK_DELIVERY_ASSEMBLY			0x01U
#define LEN_DELIVERY_ASSEMBLY_WRITE			0x1AU

#define LEN_LOCK_SERIAL_NUMBER				0x01U
#define LEN_SERIAL_NUMBER_WRITE				0x1AU

#define LEN_LOCK_PRODUCTION_PHASE_BYTE		0x01U
#define LEN_PRODUCTION_PHASE_BYTE			0x01U
//#if (BX726_TDDI_NT51926)
#define LEN_VCOM_VALUE_GET					0x07U
//#else
//#define LEN_VCOM_VALUE_GET					0x03U
//#endif
#define LEN_WRITE_DEL_SER_PART_NUMBER	    0x1AU
#define LEN_PCBATEMPERATURE_INFO			0x06U
#define LEN_BACKLIGHTTEMPERATURE_INFO		0x06U
#define LEN_BATTERYVOLTAGE_INFO				0x06U
#define LEN_SYNCVOLTAGE_INFO				0x06U
#define LEN_FPCTXOUTVOLTAGE_INFO			0x06U
#define LEN_FPCRXOUTVOLTAGE_INFO			0x06U

#define SCMD_DISPLAY_STATUS_GET				0x0AU
#define SCMD_FW_DISPLAY_STATUS_GET			0x0FU
#define SCMD_DISPLAY_ID1_GET				0xDAU
#define SCMD_DISPLAY_ID2_GET				0xDBU
#define SCMD_DISPLAY_ID3_GET				0xDCU
#define SCMD_FAULT_DETECT_ENABLE			0xB5U
#define SCMD_VCOM_SET						0xD5U

#define LEN_DISPLAY_STATUS_GET				0x01U
#define LEN_FW_DISPLAY_STATUS_GET			0x02U
#define LEN_DISPLAY_ID1_GET					0x01U
#define LEN_DISPLAY_ID2_GET					0x01U
#define LEN_DISPLAY_ID3_GET					0x01U
#define LEN_FAULT_DETECT_ENABLE				0x05U
#define LEN_VCOM_SET						0x04U

/*Custom Rolling Counter Length*/
#define LEN_ROLLING_COUNTER					0x01U

/*Custom CRC8 Length*/
#define LEN_CRC8							0x01U

#define LEN_UPDATE_REQUEST					0x01U
#define LEN_JUMP_TO_BOOTLOADER				0x01U
#define LEN_LEN								0x01U
#define LEN_CHECKSUM						0x01U
#define LEN_SCMD							0x01U
#define LEN_LOCK_FPN						0x01U

/*Backdoor suubaddress data Length*/
#if (BACKDOOR_ICDIAG_OPEN == true)
	#define LEN_ICDIAG_INFO					64U
	#define LEN_ICDIAG_FETCH				8U
	#define LEN_ICDIAG_ICCTRL				72U
#endif

#if (BACKDOOR_WRITE_DERATINGDATA)
#define LEN_DERATING_TABLE TEMP_DERATING_TABLE_SIZE
#define LEN_DERATING_SETTING_DATA 3U
#endif

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
#define LEN_DIAGNOSIS_SIMULATE				0x1FU
#endif

#define SHUTDOWN_DISABLE					0x00U
#define SHUTDOWN_ENABLE						0x01U

#define DISPLAY_DISABLE						0x00U
#define DISPLAY_ENABLE						0x01U

#define DISPLAY_OFF							0x00U
#define DISPLAY_ON							0x01U
#define DISPLAY_UNKNOW						0x02U

#define TOUCH_OFF							0x00U
#define TOUCH_ON							0x01U
#define TOUCH_UNKNOW						0x02U

#define LCD_RESET_LOW						0x00U
#define LCD_RESET_HIGH						0x01U
#define LCD_RESET_UNKNOW					0x02U


#define POWER_PASS							0x00U
#define P1V2_FAIL							0x01U
#define P3V3_FAIL							0x02U
#define POWER_VBAT_FAIL						0x03U
#define POWER_CMD_FAIL						0x04U

#define DISPLAY_OFF_TOUCH_OFF				0x00U
#define DISPLAY_ON_TOUCH_OFF				0x01U
#define DISPLAY_OFF_TOUCH_ON				0x02U
#define DISPLAY_ON_TOUCH_ON					0x03U

#define VERTICAL_TOP_TO_BOTTOM				0x00U
#define VERTICAL_BOTTOM_TO_TOP				0x01U

#define HORIZONTAL_LEFT_TO_RIGHT			0x00U
#define HORIZONTAL_RIGHT_TO_LEFT			0x01U

#define SCAN_VT_HL							0x00U       /* Vertical scan Top to Bottom and Horizontal Left to Right */
#define SCAN_VT_HR							0x01U		/* Vertical scan Top to Bottom and Horizontal Right to Left */
#define SCAN_VB_HL							0x02U		/* Vertical scan Bottom to Top and Horizontal Left to Right */
#define SCAN_VB_HR							0x03U		/* Vertical scan Bottom to Top and Horizontal Right to Left */
#define SCAN_UNKNOW							0x04U

#define LOW_LEVEL							0x00U
#define HIGH_LEVEL							0x01U

#define NORMAL_MODE                         0x00U
#define OTP_MODE							0x01U
#define TPT_MODE							0x02U
#define NORMAL_DISALBE_MODE                 0x03U
#define DISALBE_DIAG_MODE                 	0x04U
#define ENALBE_DIAG_MODE                 	0x05U

#define  DISP_SEQ_LOCK_OFF					0x00U
#define  DISP_SEQ_LOCK_ON					0x01U

/* Power & Connector & Communication Part */
#define BIT_A3_POWER_P1V2_ERROR_POS			0x0001U
#define BIT_A3_POWER_P3V3_ERROR_POS			0x0002U
#define BIT_A3_POWER_LOW_VOL_ERROR_POS		0x0004U
#define BIT_A3_POWER_HIGH_VOL_ERROR_POS		0x0008U
#define BIT_A3_PANEL_FPC_TX_L_ERROR_POS 	0x0010U
#define BIT_A3_PANEL_FPC_RX_R_ERROR_POS 	0x0020U
#define BIT_A3_COMM_LOSS_ERROR_POS			0x0040U
#define BIT_A3_BL_SHUTDOWN_ERROR_POS		0x0080U
#define BIT_A3_TEMP_PCB_LOW_ERROR_POS		0x0100U
#define BIT_A3_TEMP_BL_LOW_ERROR_POS		0x0200U
#define BIT_A3_POWER_ON_P1V2_ERROR_POS		0x0400U
#define BIT_A3_POWER_ON_P3V3_ERROR_POS		0x0800U
#define BIT_A3_POWER_ON_LOW_VOL_ERROR_POS	0x1000U
#define BIT_A3_POWER_ON_HIGH_VOL_ERROR_POS	0x2000U
#define BIT_A3_PANEL_NT51926_COMM_ERROR_POS	0x4000U
#define BIT_A3_BL_FPC_ERROR_POS				0x8000U


/* Battery Part */
#define BATT_PROTECT_OFF					0x00U
#define BATT_PROTECT_ON						0x01U

/* LED Driver Part */
/* Supply Status */
#define BIT_A3_BL_VINUVLO_ERROR_POS			0x0000000000000002UL
#define BIT_A3_BL_VINOVP_ERROR_POS			0x0000000000000008UL
#define BIT_A3_BL_VDDUVLO_ERROR_POS			0x0000000000000020UL
#define BIT_A3_BL_VINOCP_ERROR_POS			0x0000000000000080UL
#define BIT_A3_BL_CPCAP_ERROR_POS			0x0000000000000200UL
#define BIT_A3_BL_CP_ERROR_POS				0x0000000000000800UL
#define BIT_A3_BL_BSTSYNC_ERROR_POS			0x0000000000002000UL
#define BIT_A3_BL_CRCERR_ERROR_POS			0x0000000000008000UL

/* Boost Status */
#define BIT_A3_BL_BSTOVPL_ERROR_POS			0x0000000000020000UL
#define BIT_A3_BL_BSTOVPH_ERROR_POS			0x0000000000080000UL
#define BIT_A3_BL_BSTOCP_ERROR_POS			0x0000000000200000UL
#define BIT_A3_BL_FSET_ERROR_POS			0x0000000000800000UL
#define BIT_A3_BL_MODESEL_ERROR_POS			0x0000000002000000UL
#define BIT_A3_BL_LEDSET_ERROR_POS			0x0000000008000000UL
#define BIT_A3_BL_ISET_ERROR_POS			0x0000000020000000UL
#define BIT_A3_BL_TSD_ERROR_POS				0x0000000080000000UL

/* LED Status*/
#define BIT_A3_BL_LED1_ERROR_POS			0x0000000100000000UL
#define BIT_A3_BL_LED2_ERROR_POS			0x0000000200000000UL
#define BIT_A3_BL_LED3_ERROR_POS			0x0000000400000000UL
#define BIT_A3_BL_LED4_ERROR_POS			0x0000000800000000UL
#define BIT_A3_BL_OPEN_ERROR_POS			0x0000004000000000UL
#define BIT_A3_BL_SHORT_ERROR_POS			0x0000008000000000UL
#define BIT_A3_BL_GND_ERROR_POS				0x0000010000000000UL
#define BIT_A3_BL_LED_ERROR_POS				0x0000040000000000UL
#define BIT_A3_BL_INVSTRING_ERROR_POS		0x0000100000000000UL
#define BIT_A3_BL_I2C_ERROR_POS				0x0000400000000000UL

/* Panel and Touch Driver Part*/
// #define BIT_A3_PANEL_VDDI_LVD_ERROR_POS		0x0000000000000002UL
// #define BIT_A3_PANEL_AVDD_LVD_ERROR_POS		0x0000000000000008UL
// #define BIT_A3_PANEL_AVEE_LVD_ERROR_POS		0x0000000000000010UL
// #define BIT_A3_PANEL_VDDI_OVD_ERROR_POS		0x0000000000000080UL
#define BIT_A3_PANEL_VDDI_LVD_ERROR_POS		0x0000000000000001UL
#define BIT_A3_PANEL_AVDD_LVD_ERROR_POS		0x0000000000000004UL
#define BIT_A3_PANEL_AVEE_LVD_ERROR_POS		0x0000000000000008UL
#define BIT_A3_PANEL_VGH_LVD_ERROR_POS		0x0000000000000010UL
#define BIT_A3_PANEL_VGL_LVD_ERROR_POS		0x0000000000000020UL
#define BIT_A3_PANEL_VDDI_OVD_ERROR_POS		0x0000000000000040UL
#define BIT_A3_PANEL_DISPFAULT_BYTE1_POS	(BIT_A3_PANEL_VDDI_LVD_ERROR_POS | BIT_A3_PANEL_AVDD_LVD_ERROR_POS | BIT_A3_PANEL_AVEE_LVD_ERROR_POS | BIT_A3_PANEL_VGH_LVD_ERROR_POS | BIT_A3_PANEL_VGL_LVD_ERROR_POS | BIT_A3_PANEL_VDDI_OVD_ERROR_POS | BIT_A3_PANEL_VDDI_OVD_ERROR_POS)

#define BIT_A3_PANEL_AVDD_OVD_ERROR_POS		0x0000000000000100UL
#define BIT_A3_PANEL_AVEE_OVD_ERROR_POS		0x0000000000000200UL
#define BIT_A3_PANEL_DISPFAULT_BYTE2_POS	(BIT_A3_PANEL_AVDD_OVD_ERROR_POS | BIT_A3_PANEL_AVEE_OVD_ERROR_POS)

#define BIT_A3_PANEL_GATE_ERROR_POS			0x0000000000010000UL

#define BIT_A3_PANEL_SOURCE_ERROR_POS		0x0000000001000000UL

#define BIT_A3_PANEL_NO_CLOCK_ERROR_POS		0x0000000100000000UL
#define BIT_A3_PANEL_NO_VS_ERROR_POS		0x0000000200000000UL
#define BIT_A3_PANEL_NO_HSDE_ERROR_POS		0x0000000400000000UL
#define BIT_A3_PANEL_LVDS_DLL_ERROR_POS		0x0000000800000000UL
#define BIT_A3_PANEL_LVDS_FAIL_ERROR_POS	0x0000001000000000UL
#define BIT_A3_PANEL_HS_SHORT_ERROR_POS		0x0000004000000000UL
#if (CX430_TDDI_NT51926 || BX726_TDDI_NT51926)
#define BIT_A3_PANEL_DISPFAULT_BYTE3_POS	(BIT_A3_PANEL_NO_CLOCK_ERROR_POS | BIT_A3_PANEL_NO_VS_ERROR_POS | BIT_A3_PANEL_NO_HSDE_ERROR_POS | BIT_A3_PANEL_LVDS_DLL_ERROR_POS | BIT_A3_PANEL_LVDS_FAIL_ERROR_POS | BIT_A3_PANEL_HS_SHORT_ERROR_POS)
#elif (U717_TDDI_NT51926)
#define BIT_A3_PANEL_DISPFAULT_BYTE3_POS	(BIT_A3_PANEL_NO_CLOCK_ERROR_POS | BIT_A3_PANEL_NO_VS_ERROR_POS | BIT_A3_PANEL_NO_HSDE_ERROR_POS | BIT_A3_PANEL_LVDS_DLL_ERROR_POS | BIT_A3_PANEL_LVDS_FAIL_ERROR_POS | BIT_A3_PANEL_HS_SHORT_ERROR_POS)
#else
#endif

#define BIT_A3_PANEL_FLASH_CRC_ERROR_POS	0x0000020000000000UL

#define BIT_A3_PANEL_DISPFAULT_TYPEB_POS	(BIT_A3_PANEL_DISPFAULT_BYTE1_POS | BIT_A3_PANEL_DISPFAULT_BYTE2_POS | BIT_A3_PANEL_DISPFAULT_BYTE3_POS )

#define BIT_A3_PANEL_DP_STANDBY_POS			0x0000000000000000UL
#define BIT_A3_PANEL_DP_NORMAL_RUN_POS		0x0001000000000000UL
#define BIT_A3_PANEL_DP_BIST_POS			0x0002000000000000UL
#define BIT_A3_PANEL_DP_MUTE_POS			0x0003000000000000UL
#define BIT_A3_PANEL_DP_BLACK_POS			0x0004000000000000UL
#define BIT_A3_PANEL_DP_ABNORMAL_POS		0x0005000000000000UL
#define BIT_A3_PANEL_DP_POWERON_POS			0x0006000000000000UL

#define BIT_A3_PANEL_DP_STATUS_POS			0x0007000000000000UL


/* Panel and Touch Driver Part*/
#define BIT_A3_PANEL_TP_SHORT_POS			0x0100000000000000UL
#define BIT_A3_PANEL_TP_OPEN_POS			0x0200000000000000UL
#define BIT_A3_PANEL_FW_ERROR_POS			0x0400000000000000UL
#define BIT_A3_PANEL_TOUCHFAULT_ALL_POS 	(BIT_A3_PANEL_TP_SHORT_POS | BIT_A3_PANEL_TP_OPEN_POS | BIT_A3_PANEL_FW_ERROR_POS)

#define BIT_A3_PANEL_TYPEC_LCDERR_POS 		(BIT_A3_PANEL_GATE_ERROR_POS | BIT_A3_PANEL_SOURCE_ERROR_POS | BIT_A3_PANEL_FLASH_CRC_ERROR_POS)

#define BIT_A3_PANEL_FW_BLD_ERROR_POS		0x01UL

/* EEPROM Index */
#define DELIVERY_ASSEMBLY					0xC1U
#define SERIAL_NUMBER						0xC3U
#define PRODUCTION_PHASE_BYTE               0xC5U 
#define CORE_ASSEMBLY						0xC7U
#define SOFT_PART_NUMBER					0xC9U
#define MAIN_CALI_PART_NUMBER				0xCBU
#define DISPLAY_IDENTIFICATION				0xCDU
   
/* EEPROM Address Offset*/
#define ADDR_DELIVERY_ASSEMBLY				0x00000000U
#define ADDR_SERIAL_NUMBER					0x00000020U
#define ADDR_DISPLAY_IDENTIFICATION			0x00000040U
#define ADDR_PRODUCTION_PHASE_BYTE			0x00000060U
#define ADDR_RESERVED0						0x00000080U
#define ADDR_RESERVED1						0x000000A0U
#define ADDR_DERATINGDATA_LIMITED_TEMP		0x000000C0U
#define ADDR_DERATINGDATA_DUTY_TABLE		0x000000E0U

#endif
