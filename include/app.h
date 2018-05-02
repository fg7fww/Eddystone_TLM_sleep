/* ----------------------------------------------------------------------------
 * Copyright (c) 2015-2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 * This module is derived in part from example code provided by RivieraWaves
 * and as such the underlying code is the property of RivieraWaves [a member
 * of the CEVA, Inc. group of companies], together with additional code which
 * is the property of ON Semiconductor. The code (in whole or any part) may not
 * be redistributed in any form without prior written permission from
 * ON Semiconductor.
 *
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * app.h
 * - Main application header
 * ----------------------------------------------------------------------------
 * $Revision: 1.51 $
 * $Date: 2017/12/05 15:55:11 $
 * ------------------------------------------------------------------------- */

#ifndef APP_H
#define APP_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <rsl10.h>
#include <rsl10_ke.h>
#include <rsl10_ble.h>
#include <rsl10_profiles.h>
#include <rsl10_map_nvr.h>
#include <stdbool.h>
#include <rsl10_protocol.h>

#include "i2c.h"
#include "nct375.h"

#include "ble_std.h"
#include "ble_custom.h"
#include "ble_bass.h"
#include "calibration.h"

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Enable/disable buck converter
 * Options: VCC_BUCK_BITBAND or VCC_LDO_BITBAND */
#define VCC_BUCK_LDO_CTRL               VCC_BUCK_BITBAND

/* Minimum and maximum VBAT measurements */
#define VBAT_1P1V_MEASURED              0x1200
#define VBAT_1P4V_MEASURED              0x16CC

/* Maximum battery level */
#define BAT_LVL_MAX                     100

/* DIO number that is connected to LED of EVB */
#define LED_DIO                         6

/* DIO used for the I2C interface to interface the SI7042 sensor */
#define I2C_SDA_DIO_NUM                 12 /* 0 */
#define I2C_SCL_DIO_NUM                 11 /* 1 */
#define I2C_GND_DIO_NUM                 10 /* 2 */
#define I2C_PWR_DIO_NUM                 8  /* 4 */

#define UART_CFG_SYS_CLK                SystemCoreClock
#define UART_BAUD_RATE                  115200
#define UART_TX_DIO_NUM                 0
#define UART_RX_DIO_NUM                 1

#define SPI_SCLK_DIO_NUM                2
#define SPI_MOSI_DIO_NUM                3
#define SPI_CS_DIO_NUM                  4
#define SPI_MISO_DIO_NUM                7

/* NCT375 I2C commands */
#define NCT375_CMD_GET_TEMPERATURE					(uint8_t[]){0x00}
#define NCT375_CMD_GET_TEMPERATURE_ONE_SHOT			(uint8_t[]){0x04}

/* DIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_DIO                    12

/* Set timer to 200 ms (20 times the 10 ms kernel timer resolution) */
#define TIMER_200MS_SETTING             20

/* Configure RF 48 MHz XTAL divided clock frequency in Hz
 * Options: 8, 12, 16, 24, 48 */
#define RFCLK_FREQ                      8000000

/* Define clock divider and flash timings depending on RF clock frequency */
#if (RFCLK_FREQ == 8000000)
#define RF_CK_DIV_PRESCALE_VALUE        CK_DIV_1_6_PRESCALE_6_BYTE
#define SLOWCLK_PRESCALE_VALUE          SLOWCLK_PRESCALE_8
#define BBCLK_PRESCALE_VALUE            BBCLK_PRESCALE_1
#define DCCLK_PRESCALE_BYTE_VALUE       DCCLK_PRESCALE_2_BYTE
#define FLASH_DELAY_VALUE               FLASH_DELAY_FOR_SYSCLK_8MHZ
#define BBCLK_DIVIDER_VALUE             BBCLK_DIVIDER_8
#elif (RFCLK_FREQ == 12000000)
#define RF_CK_DIV_PRESCALE_VALUE        CK_DIV_1_6_PRESCALE_4_BYTE
#define SLOWCLK_PRESCALE_VALUE          SLOWCLK_PRESCALE_12
#define BBCLK_PRESCALE_VALUE            BBCLK_PRESCALE_1
#define DCCLK_PRESCALE_BYTE_VALUE       DCCLK_PRESCALE_3_BYTE
#define FLASH_DELAY_VALUE               FLASH_DELAY_FOR_SYSCLK_12MHZ
#define BBCLK_DIVIDER_VALUE             BBCLK_DIVIDER_12
#elif (RFCLK_FREQ == 16000000)
#define RF_CK_DIV_PRESCALE_VALUE        CK_DIV_1_6_PRESCALE_3_BYTE
#define SLOWCLK_PRESCALE_VALUE          SLOWCLK_PRESCALE_16
#define BBCLK_PRESCALE_VALUE            BBCLK_PRESCALE_2
#define DCCLK_PRESCALE_BYTE_VALUE       DCCLK_PRESCALE_4_BYTE
#define FLASH_DELAY_VALUE               FLASH_DELAY_FOR_SYSCLK_16MHZ
#define BBCLK_DIVIDER_VALUE             BBCLK_DIVIDER_8
#elif (RFCLK_FREQ == 24000000)
#define RF_CK_DIV_PRESCALE_VALUE        CK_DIV_1_6_PRESCALE_2_BYTE
#define SLOWCLK_PRESCALE_VALUE          SLOWCLK_PRESCALE_24
#define BBCLK_PRESCALE_VALUE            BBCLK_PRESCALE_3
#define DCCLK_PRESCALE_BYTE_VALUE       DCCLK_PRESCALE_6_BYTE
#define FLASH_DELAY_VALUE               FLASH_DELAY_FOR_SYSCLK_24MHZ
#define BBCLK_DIVIDER_VALUE             BBCLK_DIVIDER_8
#elif (RFCLK_FREQ == 48000000)
#define RF_CK_DIV_PRESCALE_VALUE        CK_DIV_1_6_PRESCALE_1_BYTE
#define SLOWCLK_PRESCALE_VALUE          SLOWCLK_PRESCALE_48
#define BBCLK_PRESCALE_VALUE            BBCLK_PRESCALE_6
#define DCCLK_PRESCALE_BYTE_VALUE       DCCLK_PRESCALE_12_BYTE
#define FLASH_DELAY_VALUE               FLASH_DELAY_FOR_SYSCLK_48MHZ
#define BBCLK_DIVIDER_VALUE             BBCLK_DIVIDER_8
#endif

/* Define DCCLK prescaler configuration depending on VCC configuration */
#if (VCC_BUCK_LDO_CTRL == VCC_BUCK_BITBAND)
#define DCCLK_BYTE_VALUE DCCLK_PRESCALE_BYTE_VALUE
#else
#define DCCLK_BYTE_VALUE DCCLK_DISABLE_BYTE
#endif

/* Low power clock for RTC
 * Options are:
 *                                      RTC_CLK_SRC_XTAL32K
 *                                      RTC_CLK_SRC_RC_OSC
 *                                      RTC_CLK_SRC_DIO[ 0 | 1 | 2 | 3 ]  */
#define RTC_CLK_SRC                     RTC_CLK_SRC_XTAL32K

/* Update options when RC_OSC is used for clock:
 * Enable  1
 * Disable 0 */
#define RC_OSC_UPDATE                   0

/* Update interval (in advertisement and sleep cycles) for RC oscillator period
 * measurement  */
#define RC_OSC_UPDATE_INTERVAL          100

/* The default value of RC clock period [us] */
#define RCCLK_PERIOD_VALUE              (float) (1000000.0 / 32768)

/* Number of frequency samples (in 16*) for calculating average frequency and
 * period for RC oscillator */
#define RCCLK_FREQUENCY_SAMPLES         1000

/* External low power clock DIO number */
#define EXT_LOW_POWER_CLK_GPIO_NUM      0

/* The default value of external clock frequency in Hz */
#define EXT_LOW_POWER_CLK_FREQ          100000

/* The external clock should have 500 ppm accuracy otherwise
 * its period should be measured and set, if it has variation
 * over time, it should be measured dynamically and set periodically in us */
#define EXT_LOW_POWER_CLK_PERIOD_VALUE  (float) (1000000.0 / \
                                                 EXT_LOW_POWER_CLK_FREQ)

/* DMA channel used to save/restore BB registers in each sleep/wake-up cycle */
#define DMA_CHAN_SLP_WK_BB_REGS_COPY    0

/* DMA channel used to save/restore RF registers in each sleep/wake-up cycle */
#define DMA_CHAN_SLP_WK_RF_REGS_COPY    1

/* Customized parameters for the LLD SLEEP module
 * respect to OSC wake-up timings in us */
#define TWOSC                           1100

extern const struct ke_task_desc TASK_DESC_APP;

/* APP Task messages */
enum appm_msg
{
    APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),

    /* Timer used to have a tick periodically for application */
    APP_TEST_TIMER,
};

typedef void (*appm_add_svc_func_t)(void);

#define DEFINE_SERVICE_ADD_FUNCTION(func) (appm_add_svc_func_t) func

#define DEFINE_MESSAGE_HANDLER(message, handler) { message, \
                                                   (ke_msg_func_t) handler }

/* List of message handlers that are used by the different profiles/services */
#define APP_MESSAGE_HANDLER_LIST \


/* List of functions used to create the database */
#define SERVICE_ADD_FUNCTION_LIST                        \
    DEFINE_SERVICE_ADD_FUNCTION(Batt_ServiceAdd_Server), \
    DEFINE_SERVICE_ADD_FUNCTION(CustomService_ServiceAdd)

typedef void (*appm_enable_svc_func_t)(uint8_t);
#define DEFINE_SERVICE_ENABLE_FUNCTION(func) (appm_enable_svc_func_t) func

/* List of functions used to enable client services */
#define SERVICE_ENABLE_FUNCTION_LIST \
    DEFINE_SERVICE_ENABLE_FUNCTION(Batt_ServiceEnable_Server)

/* Length of custom service notification data [byte] (max: 20 bytes)*/
#define APP_CS_TX_VALUE_NOTF_LENGTH     5

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/
struct app_env_tag
{
    /* Battery service */
    uint8_t batt_lvl;
    uint32_t sum_batt_lvl;
    uint16_t num_batt_read;
    uint8_t send_batt_ntf;

    uint32_t sleep_cycles;

	/* Temperature value and CCCD */
	int16_t temperature;
    uint16_t temperature_cccd_value;

    /* I2C reception buffer */
    uint8_t i2c_rx_buffer[8];

    /* I2C reception buffer */
    uint8_t i2c_tx_buffer[8];

};

#ifdef ONE_SHOT_MODE
	typedef void (*OneShotMode_t)(void);
#endif

/* RC oscillator period measurement parameter */
extern volatile uint16_t sample_cnt;

/* Counter to track number of advertisement and sleep cycles */
extern volatile int loop_cnt;

/* Support for the application manager and the application environment */
extern struct app_env_tag app_env;

/* List of functions used to create the database */
extern const appm_add_svc_func_t appm_add_svc_func_list[];

/* Parameters and configurations for the sleep mode */
extern struct sleep_mode_env_tag sleep_mode_env;

/* ---------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/
extern void App_Initialize(void);

extern void App_Env_Initialize(void);

extern void Sleep_Mode_Configure(struct sleep_mode_env_tag *sleep_mode_env);

extern void Wakeup_From_Sleep_Application_asm(void);

extern void Wakeup_From_Sleep_Application(void)
                          __attribute__ ((section(".app_wakeup")));

extern void Continue_Application(void);

extern void Main_Loop(void);

extern void Measure_Battery_Level(void);

extern uint8_t Emulate_CS_Val_Notif_Change(uint8_t val_notif);

extern int Msg_Handler(ke_msg_id_t const msgid, void *param,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);

extern void AUDIOSINK_PERIOD_IRQHandler(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif /* APP_H */
