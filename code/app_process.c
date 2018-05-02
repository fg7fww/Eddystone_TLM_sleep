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
 * app_process.c
 * - Application task handler definition and support processes
 * ----------------------------------------------------------------------------
 * $Revision: 1.37 $
 * $Date: 2017/12/04 22:53:36 $
 * ------------------------------------------------------------------------- */

#include "../include/app.h"

/* Parameters for RC Oscillator period measurements */
volatile uint16_t sample_cnt = 0;
uint32_t rc_period_sum = 0;
volatile int loop_cnt = 0;
#if (RC_OSC_UPDATE)
uint32_t rc_period_prev = 0;
#endif

const struct ke_task_desc TASK_DESC_APP = {
    NULL, &appm_default_handler,
    appm_state, APPM_STATE_MAX,
    APP_IDX_MAX
};

/* State and event handler definition */
const struct ke_msg_handler appm_default_state[] =
{
    /* Note: Put the default handler on top as this is used for handling any
     *       messages without a defined handler */
    { KE_MSG_DEFAULT_HANDLER, (ke_msg_func_t) Msg_Handler },
    BLE_MESSAGE_HANDLER_LIST,
    BASS_MESSAGE_HANDLER_LIST,
    CS_MESSAGE_HANDLER_LIST,
    APP_MESSAGE_HANDLER_LIST
};

/* Use the state and event handler definition for all states. */
const struct ke_state_handler appm_default_handler
    = KE_STATE_HANDLER(appm_default_state);

/* Defines a place holder for all task instance's state */
ke_state_t appm_state[APP_IDX_MAX];

/* ----------------------------------------------------------------------------
 * Function      : void Sleep_Mode_Configure(
                           struct sleep_mode_env_tag *sleep_mode_env)
 * ----------------------------------------------------------------------------
 * Description   : Configure the sleep mode
 * Inputs        : Pre-defined parameters and configurations
 *                 for the sleep mode
 * Outputs       : sleep_mode_env   - Parameters and configurations
 *                                    for the sleep mode
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Sleep_Mode_Configure(struct sleep_mode_env_tag *sleep_mode_env)
{
    struct sleep_mode_init_env_tag sleep_mode_init_env;

    /* Set the clock source for RTC */
    sleep_mode_init_env.rtc_ctrl = RTC_CLK_SRC;

    /* if RTC clock source is XTAL 32 kHz oscillator */
    if (RTC_CLK_SRC == RTC_CLK_SRC_XTAL32K)
    {
        /* Enable XTAL32K oscillator amplitude control */
        ACS_XTAL32K_CTRL->EN_AMPL_CTRL_ALIAS = XTAL32K_AMPL_CTRL_ENABLE_BITBAND;

        /* Set XTAL32K load capacitance
         * 0x00: 0 pF internal capacitor */
        ACS_XTAL32K_CTRL->CLOAD_TRIM_BYTE = (uint8_t) (0x00);

        /* Enable XTAL32K oscillator */
        ACS_XTAL32K_CTRL->ENABLE_ALIAS = XTAL32K_ENABLE_BITBAND;

        /* Wait for XTAL32K oscillator to be ready */
        while (ACS_XTAL32K_CTRL->READY_ALIAS != XTAL32K_OK_BITBAND);

        LowPowerClock_Source_Set(0);
    }
    /* else: if RTC clock source is RC 32 kHz oscillator */
    else if (RTC_CLK_SRC == RTC_CLK_SRC_RC_OSC)
    {
        /* Start the RC oscillator */
        Sys_Clocks_Osc32kHz(RC_OSC_ENABLE | RC_OSC_NOM);

        /* Read the OSC_32K calibration trim data from NVR4 */
        unsigned int osc_calibration_value = 0;
        Sys_ReadNVR4(MANU_INFO_OSC_32K, 1, (unsigned
                                            int *) &osc_calibration_value);

        /* Use calibrated value for RC clock */
        if (osc_calibration_value != 0xFFFFFFFF)
        {
            ACS_RCOSC_CTRL->FTRIM_32K_BYTE = (uint8_t) (osc_calibration_value);
        }

        LowPowerClock_Source_Set(1);

        /* In us, for typical RCOSC until measurement is obtained. */
        RTCCLK_Period_Value_Set(RCCLK_PERIOD_VALUE);

        /* Set-up the Audiosink block for frequency measurement */
        Sys_Audiosink_ResetCounters();
        Sys_Audiosink_InputClock(0, AUDIOSINK_CLK_SRC_STANDBYCLK);
        Sys_Audiosink_Config(AUDIO_SINK_PERIODS_16, 0, 0);

        /* Enable interrupts */
        NVIC_ClearPendingIRQ(AUDIOSINK_PERIOD_IRQn);
        NVIC_EnableIRQ(AUDIOSINK_PERIOD_IRQn);

        /* Start period counter to start period measurement */
        AUDIOSINK_CTRL->PERIOD_CNT_START_ALIAS = 1;
    }
    /* else: if RTC clock source is external oscillator */
    else
    {
        DIO->CFG[EXT_LOW_POWER_CLK_GPIO_NUM] = (DIO_2X_DRIVE     |
                                                DIO_LPF_DISABLE  |
                                                DIO_NO_PULL      |
                                                DIO_MODE_INPUT);

        LowPowerClock_Source_Set(1);

        /* Clock period in us for external clock */
        RTCCLK_Period_Value_Set(EXT_LOW_POWER_CLK_PERIOD_VALUE);
    }

    /* Set delay and wake-up sources, use
     *    WAKEUP_DELAY_[ 1 | 2 | 4 | ... | 128],
     *    WAKEUP_DCDC_OVERLOAD_[ENABLE | DISABLE],
     *    WAKEUP_WAKEUP_PAD_[RISING | FALLING],
     *    WAKEUP_DIO*_[RISING | FALLING],
     *    WAKEUP_DIO*_[ENABLE | DISABLE] */
    sleep_mode_init_env.wakeup_cfg = WAKEUP_DELAY_32          |
                                     WAKEUP_WAKEUP_PAD_RISING |
                                     WAKEUP_DIO3_DISABLE      |
                                     WAKEUP_DIO2_DISABLE      |
                                     WAKEUP_DIO1_DISABLE      |
                                     WAKEUP_DIO0_DISABLE;

    /* Set wake-up control/status registers, use
     *    PADS_RETENTION_[ENABLE | DISABLE],
     *    BOOT_FLASH_APP_REBOOT_[ENABLE | DISABLE],
     *    BOOT_[CUSTOM | FLASH_XTAL_*],
     *    WAKEUP_DCDC_OVERLOAD_CLEAR,
     *    WAKEUP_PAD_EVENT_CLEAR,
     *    WAKEUP_RTC_ALARM_CLEAR,
     *    WAKEUP_BB_TIMER_CLEAR,
     *    WAKEUP_DIO3_EVENT_CLEAR,
     *    WAKEUP_DIO2_EVENT_CLEAR,
     *    WAKEUP_DIO1_EVENT_CLEAR],
     *    WAKEUP_DIO0_EVENT_CLEAR */
    sleep_mode_env->wakeup_ctrl = PADS_RETENTION_ENABLE         |
                                  BOOT_FLASH_APP_REBOOT_DISABLE |
                                  BOOT_CUSTOM                   |
                                  WAKEUP_DCDC_OVERLOAD_CLEAR    |
                                  WAKEUP_PAD_EVENT_CLEAR        |
                                  WAKEUP_RTC_ALARM_CLEAR        |
                                  WAKEUP_BB_TIMER_CLEAR         |
                                  WAKEUP_DIO3_EVENT_CLEAR       |
                                  WAKEUP_DIO2_EVENT_CLEAR       |
                                  WAKEUP_DIO1_EVENT_CLEAR       |
                                  WAKEUP_DIO0_EVENT_CLEAR;

    /* Set wake-up application start address (LSB must be set) */
    sleep_mode_init_env.app_addr =
        (uint32_t) (&Wakeup_From_Sleep_Application_asm) | 1;

#if defined(CFG_LIGHT_STACK)

    /* Set wake-up restore address */
    sleep_mode_init_env.wakeup_addr = (uint32_t) (DRAM0_TOP + 1 -
                                                  POWER_MODE_WAKEUP_INFO_SIZE);

    /* Configure memory retention */
    sleep_mode_env->mem_power_cfg = (DRAM0_POWER_ENABLE |
                                     BB_DRAM0_POWER_ENABLE);

    /* Configure memory at wake-up (PROM must be part of this) */
    sleep_mode_init_env.mem_power_cfg_wakeup = (PROM_POWER_ENABLE  |
                                                DRAM0_POWER_ENABLE |
                                                BB_DRAM0_POWER_ENABLE);
#else

    /* Set wake-up restore address */
    sleep_mode_init_env.wakeup_addr = (uint32_t) (DRAM2_TOP + 1 -
                                                  POWER_MODE_WAKEUP_INFO_SIZE);

    /* Configure memory retention */
    sleep_mode_env->mem_power_cfg = (DRAM0_POWER_ENABLE |
                                     DRAM1_POWER_ENABLE |
                                     DRAM2_POWER_ENABLE |
                                     BB_DRAM0_POWER_ENABLE);

    /* Configure memory at wake-up (PROM must be part of this) */
    sleep_mode_init_env.mem_power_cfg_wakeup = (PROM_POWER_ENABLE  |
                                                DRAM0_POWER_ENABLE |
                                                DRAM1_POWER_ENABLE |
                                                DRAM2_POWER_ENABLE |
                                                BB_DRAM0_POWER_ENABLE);
#endif

    /* Set DMA channel used to save/restore RF registers
     * in each sleep/wake-up cycle */
    sleep_mode_init_env.DMA_channel_RF = DMA_CHAN_SLP_WK_RF_REGS_COPY;

    /* Perform initializations required for sleep mode */
#ifdef APP_SLEEP_2MBPS_SUPPORT
    Sys_PowerModes_Sleep_Init_2Mbps(&sleep_mode_init_env);
#else
    Sys_PowerModes_Sleep_Init(&sleep_mode_init_env);
#endif
}

/* ----------------------------------------------------------------------------
 * Function      : void Wakeup_From_Sleep_Application(void)
 * ----------------------------------------------------------------------------
 * Description   : Restore system states from retention RAM and continue
 *                 application from flash
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Wakeup_From_Sleep_Application(void)
{
    /* Execute steps required to wake-up the system from sleep mode */
#ifdef APP_SLEEP_2MBPS_SUPPORT
    Sys_PowerModes_Wakeup_2Mbps();
#else
    Sys_PowerModes_Wakeup();
#endif

    /* The system is awake from this point, continue application from flash */
    Continue_Application();
}

/* ----------------------------------------------------------------------------
 * Function      : void Continue_Application(void)
 * ----------------------------------------------------------------------------
 * Description   : Restore application states, wait until BLE is awake and
 *                 go to the main loop
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Continue_Application(void)
{
    /* Lower drive strength (required when VDDO > 2.7)*/
    DIO->PAD_CFG = PAD_LOW_DRIVE;

    /* Turn LED on */
    Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_1);

    /* Turn off pad retention */
    ACS_WAKEUP_CTRL->PADS_RETENTION_EN_BYTE = PADS_RETENTION_DISABLE_BYTE;

    /* Configure clock dividers */
    CLK->DIV_CFG0 = SLOWCLK_PRESCALE_VALUE | BBCLK_PRESCALE_VALUE |
                    USRCLK_PRESCALE_1;
    CLK_DIV_CFG2->DCCLK_BYTE = DCCLK_BYTE_VALUE;

    /* Update Flash timing */
    FLASH->DELAY_CTRL = DEFAULT_READ_MARGIN | FLASH_DELAY_VALUE;

    /* Switch to RF clock */
    CLK_SYS_CFG->SYSCLK_SRC_SEL_BYTE = SYSCLK_CLKSRC_RFCLK_BYTE;

    /* Configure the baseband divider and force wake-up in case it is required
     * due to an early ACS wake-up condition (e.g. PAD, RTC) */
    BBIF->CTRL = BB_CLK_ENABLE | BBCLK_DIVIDER_VALUE | BB_WAKEUP;

    /* Mask all interrupts */
    __disable_irq();
    while (!(BLE_Is_Awake()))
    {
        SYS_WAIT_FOR_INTERRUPT;

        /* Process interrupt */
        __enable_irq();
        __disable_irq();
    }

    /* Update RCOSC period every 100 cycles of advertisement and sleep.
     * 100 cycles = approximately 4.5s with a connection interval of 20ms */
#if (RC_OSC_UPDATE)
    loop_cnt++;
    sample_cnt = RCCLK_FREQUENCY_SAMPLES - 1;

    if (loop_cnt == RC_OSC_UPDATE_INTERVAL)
    {
        /* Set-up the Audiosink block for frequency measurement */
        Sys_Audiosink_ResetCounters();
        Sys_Audiosink_InputClock(0, AUDIOSINK_CLK_SRC_STANDBYCLK);
        Sys_Audiosink_Config(AUDIO_SINK_PERIODS_16, 0, 0);

        /* Enable interrupts */
        NVIC_ClearPendingIRQ(AUDIOSINK_PERIOD_IRQn);
        NVIC_EnableIRQ(AUDIOSINK_PERIOD_IRQn);

        /* Start period counter to start period measurement */
        AUDIOSINK_CTRL->PERIOD_CNT_START_ALIAS = 1;
    }
#endif

    /* Stop masking interrupts */
    __enable_irq();

    /* Stop forcing baseband wake-up */
    BBIF->CTRL = BB_CLK_ENABLE | BBCLK_DIVIDER_VALUE | BB_DEEP_SLEEP;

    /* Main application loop */
    Main_Loop();
}

/* ----------------------------------------------------------------------------
 * Function      : void Measure_Battery_Level(void)
 * ----------------------------------------------------------------------------
 * Description   : - Read the battery level using ADC and calculate the
 *                 average value. If the average value changes, set the
 *                 notification flag for battery service
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Measure_Battery_Level(void)
{
    uint16_t level;

    /* Calculate the battery level */
    level = ADC->DATA_TRIM_CH[0];
    ble_env.batt_lvl = 2*level;
}

/* ----------------------------------------------------------------------------
 * Function      : uint8_t Emulate_CS_Val_Notif_Change(uint8_t val_notif)
 * ----------------------------------------------------------------------------
 * Description   : Emulate the change of custom service notification data
 * Inputs        : - val_notif  - value of custom service notification
 * Outputs       : return value - updated value of custom service notification
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
uint8_t Emulate_CS_Val_Notif_Change(uint8_t val_notif)
{
    val_notif++;
    if ((val_notif & 0x0F) == 0x0A)
    {
        val_notif = val_notif & 0xF0;
        val_notif = val_notif + 0x10;
        if ((val_notif & 0xF0) == 0xA0)
        {
            val_notif = val_notif & 0x0F;
        }
    }

    return(val_notif);
}

/* ----------------------------------------------------------------------------
 * Function      : int Msg_Handler(ke_msg_id_t const msg_id,
 *                                 void const *param,
 *                                 ke_task_id_t const dest_id,
 *                                 ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Handle any message received from kernel that doesn't have
 *                 a dedicated handler
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameter (unused)
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int Msg_Handler(ke_msg_id_t const msg_id, void *param,
                ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    return(KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : void AUDIOSINK_PERIOD_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Calculates the average period for every 16 measurements and
 *                 and then averages 1000 of those measurements before updating
 *                 the RC oscillator.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void AUDIOSINK_PERIOD_IRQHandler(void)
{
    /* Parameters for RC oscillator period measurements */
    uint32_t rc_freq;
    uint32_t rc_period_new = 0;

#if (RC_OSC_UPDATE)
    uint32_t rc_period_curr = 0;

    /* k = (1/1000) = 0.001 (scaled for accuracy) for a 16 measurements
     * averaging low pass filter */
    const uint16_t k = 1;

    /* Record period count value (scaled for accuracy) */
    rc_period_curr = Sys_Audiosink_PeriodCounter() * 100;
#endif

    /* Use long averaging method for the initial update for the RC oscillator */
    if (loop_cnt == 0)
    {
        rc_period_sum += Sys_Audiosink_PeriodCounter();
    }

    /* Reset period counter */
    AUDIOSINK->PERIOD_CNT = 0;

#if (RC_OSC_UPDATE)

    /* Start using the filter after rc_period_prev is initialized */
    if (rc_period_prev != 0)
    {
        /* Low pass filter for new period measurements */
        rc_period_new = (((k * rc_period_curr) + ((1000 - k) *
                                                  rc_period_prev)) + 50000)
                        / 1000;
        rc_period_prev = rc_period_new - 50;
    }
#endif

    sample_cnt++;

    /* Once sampleCounter is reset and measurement is obtained, update
     * RC oscillator period. */
    if (sample_cnt == RCCLK_FREQUENCY_SAMPLES)
    {
        if (loop_cnt == 0)
        {
            rc_period_new = (rc_period_sum + (sample_cnt >> 1)) / sample_cnt;

#if (RC_OSC_UPDATE)
            /* Initialize the 'previous' variable based on the average
             * calculated over the 1000 samples collected at the start 
             * to begin using the filter for continuous updates */
            rc_period_prev = (rc_period_new - 0.5) * 100;
#endif
        }
        else
        {
            rc_period_new /= 100;
        }

        /* Calculate RC oscillator frequency */
        rc_freq = (SystemCoreClock * (AUDIOSINK->CFG + 1)
                   + (rc_period_new >> 1)) / rc_period_new;

        /* Update RCCLK period value */
        RTCCLK_Period_Value_Set((float) 1000000.0 / rc_freq);

        /* Reset sampleCounter and loop_cnt */
        sample_cnt = 0;
        loop_cnt = 0;
    }
}
