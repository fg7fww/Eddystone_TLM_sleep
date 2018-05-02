/* ----------------------------------------------------------------------------
 * Copyright (c) 2015-2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * app_init.c
 * - Application initialization
 * ----------------------------------------------------------------------------
 * $Revision: 1.50 $
 * $Date: 2017/12/05 16:52:39 $
 * ------------------------------------------------------------------------- */

#include "../include/app.h"

/* Application Environment Structure */
struct app_env_tag app_env;

/* Sleep Mode Environment Structure */
struct sleep_mode_env_tag sleep_mode_env;

/* ----------------------------------------------------------------------------
 * Function      : void App_Initialize(void)
 * ----------------------------------------------------------------------------
 * Description   : Initialize the system for proper application execution
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void App_Initialize(void) {
	/* Customized parameters for the LLD SLEEP module
	 * respect to OSC wake-up timings in us */
	struct lld_sleep_params_t desired_lld_sleep_params;

	/* Mask all interrupts */
	__set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

	/* Disable all interrupts and clear any pending interrupts */
	Sys_NVIC_DisableAllInt();
	Sys_NVIC_ClearAllPendingInt();

	/* Test DIO12 to pause the program to make it easy to re-flash */
	DIO->CFG[RECOVERY_DIO] = DIO_MODE_INPUT | DIO_WEAK_PULL_UP |
	DIO_LPF_DISABLE | DIO_6X_DRIVE;
	while (DIO_DATA->ALIAS[RECOVERY_DIO] == 0)
		;

	/* Calibrate the board */
#if (CALIB_RECORD == MANU_CALIB)
	if (Load_Trim_Values_And_Calibrate_MANU_CALIB() !=
	VOLTAGES_CALIB_NO_ERROR) {
		/* Hold here to notify error(s) in voltage calibrations */
		while (true) {
			Sys_Watchdog_Refresh();
		}
	}
#elif (CALIB_RECORD == SUPPLEMENTAL_CALIB)
	if (Load_Trim_Values_And_Calibrate_SUPPLEMENTAL_CALIB() !=
			VOLTAGES_CALIB_NO_ERROR)
	{
		/* Hold here to notify error(s) in voltage calibrations */
		while (true)
		{
			Sys_Watchdog_Refresh();
		}
	}
#elif (CALIB_RECORD == USER_CALIB)
	if (Calculate_Trim_Values_And_Calibrate() !=
			VOLTAGES_CALIB_NO_ERROR)
	{
		/* Hold here to notify error(s) in voltage calibrations */
		while (true)
		{
			Sys_Watchdog_Refresh();
		}
	}
#endif /* CALIB_RECORD */

	/* Configure the current trim settings for VDDA */
#ifdef POWER_AMPLIFIER_ON
	ACS_VDDA_CP_CTRL->PTRIM_BYTE = VDDA_PTRIM_16MA_BYTE;
#endif

	/* Configure the current trim settings for VCC, VDDA */
	ACS_VCC_CTRL->ICH_TRIM_BYTE = VCC_ICHTRIM_80MA_BYTE;

	/* Start 48 MHz XTAL oscillator */
	ACS_VDDRF_CTRL->ENABLE_ALIAS = VDDRF_ENABLE_BITBAND;
	ACS_VDDRF_CTRL->CLAMP_ALIAS = VDDRF_DISABLE_HIZ_BITBAND;

	/* Wait until VDDRF supply has powered up */
	while (ACS_VDDRF_CTRL->READY_ALIAS != VDDRF_READY_BITBAND)
		;

	/* Configure VDDPA */
#ifdef POWER_AMPLIFIER_ON

	/* Enable power amplifier */
	ACS_VDDPA_CTRL->ENABLE_ALIAS = VDDPA_ENABLE_BITBAND;
	ACS_VDDPA_CTRL->VDDPA_SW_CTRL_ALIAS = VDDPA_SW_HIZ_BITBAND;
#else

	/* Disable power amplifier */
	ACS_VDDPA_CTRL->ENABLE_ALIAS = VDDPA_DISABLE_BITBAND;
	ACS_VDDPA_CTRL->VDDPA_SW_CTRL_ALIAS = VDDPA_SW_VDDRF_BITBAND;
#endif /* POWER_AMPLIFIER_ON */

	/* Enable/disable buck converter */
	ACS_VCC_CTRL->BUCK_ENABLE_ALIAS = VCC_BUCK_LDO_CTRL;

	/* Enable RF power switches */
	SYSCTRL_RF_POWER_CFG->RF_POWER_ALIAS = RF_POWER_ENABLE_BITBAND;

	/* Remove RF isolation */
	SYSCTRL_RF_ACCESS_CFG->RF_ACCESS_ALIAS = RF_ACCESS_ENABLE_BITBAND;

	/* Start the 48 MHz oscillator without changing the other register bits */
	RF->XTAL_CTRL = ((RF->XTAL_CTRL & ~XTAL_CTRL_DISABLE_OSCILLATOR) |
	XTAL_CTRL_REG_VALUE_SEL_INTERNAL);

	/* Enable the 48 MHz oscillator divider using the desired prescale value */
	RF_REG2F->CK_DIV_1_6_CK_DIV_1_6_BYTE = RF_CK_DIV_PRESCALE_VALUE;

	/* Wait until 48 MHz oscillator is started */
	while (RF_REG39->ANALOG_INFO_CLK_DIG_READY_ALIAS !=
	ANALOG_INFO_CLK_DIG_READY_BITBAND)
		;

	/* Switch to (divided 48 MHz) oscillator clock */
	Sys_Clocks_SystemClkConfig(JTCK_PRESCALE_1 |
	EXTCLK_PRESCALE_1 |
	SYSCLK_CLKSRC_RFCLK);

	/* Configure clock dividers */
	CLK->DIV_CFG0 = SLOWCLK_PRESCALE_VALUE | BBCLK_PRESCALE_VALUE |
	USRCLK_PRESCALE_1;
	CLK_DIV_CFG2->DCCLK_BYTE = DCCLK_BYTE_VALUE;

	/* - The baseband clock (master1) is a scaled down version of SYSCLK that
	 *   can be configured by setting the field BBCLK_PRESCALE of the
	 *   CLK_DIV_CFG0 control register: 8 MHz or 12 MHz
	 * - The internal baseband controller clock divider must be set according
	 *   to the baseband clock frequency in order to generate a 1 MHz clock */
	BBIF->CTRL = BB_CLK_ENABLE | BBCLK_DIVIDER_VALUE | BB_DEEP_SLEEP;

	/* Seed the random number generator */
	srand(1);

	/* Configure ADC channel 0 to measure VBAT/2 */
	Sys_ADC_Set_Config(ADC_VBAT_DIV2_NORMAL | ADC_NORMAL |
	ADC_PRESCALE_6400);
	Sys_ADC_InputSelectConfig(0, (ADC_NEG_INPUT_GND |
	ADC_POS_INPUT_VBAT_DIV2));

	/* Customized parameters for the LLD SLEEP module
	 * respect to OSC wake-up timings in us */
	desired_lld_sleep_params.twosc = TWOSC;
	BLE_LLD_Sleep_Params_Set(desired_lld_sleep_params);

	/* Initialize the baseband and BLE stack */
	BLE_Initialize();

	/* Set radio output power of RF */
	Sys_RFFE_SetTXPower(RF_TX_POWER_LEVEL);

	/* Trim RC oscillator to 3 MHz (required by Sys_PowerModes_Wakeup) */
	Sys_Clocks_OscRCCalibratedConfig(3000);

	/* Configure the sleep mode parameters and configurations */
	Sleep_Mode_Configure(&sleep_mode_env);

	/* BLE not in sleep mode and ready for normal operations */
	BLE_Is_Awake_Flag_Set();

	/* Initialize environment */
	App_Env_Initialize();

#ifdef VOLTAGES_CALIB_VERIFY

	/* Hold here to verify calibrated voltages */
	while (true)
	{
		Sys_Watchdog_Refresh();
	}
#endif

	/* Stop masking interrupts */
	__set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
	__set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/* ----------------------------------------------------------------------------
 * Function      : void App_Env_Initialize(void)
 * ----------------------------------------------------------------------------
 * Description   : Initialize application environment
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void App_Env_Initialize(void) {
	/* Reset the application manager environment */
	memset(&app_env, 0, sizeof(app_env));

	/* Create the application task handler */
	ke_task_create(TASK_APP, &TASK_DESC_APP);

	/* Configure I2C as Master, increase its interrupt priority to manage
	 * quicker the transferred data */
	I2C_Master_Init(0x80U);
	NVIC_SetPriority(I2C_IRQn, 2);

	/* Configure the DIOs for I2C */
	Sys_I2C_DIOConfig(DIO_6X_DRIVE | DIO_LPF_ENABLE | DIO_STRONG_PULL_UP,
			I2C_SCL_DIO_NUM, I2C_SDA_DIO_NUM);

	/* Configure the DIO used as ground and power pins for the SI7042 */
	Sys_DIO_Config(I2C_GND_DIO_NUM, DIO_MODE_GPIO_OUT_0);
	Sys_DIO_Config(I2C_PWR_DIO_NUM, DIO_MODE_GPIO_OUT_1);

	/* Initialize the custom service environment */
	CustomService_Env_Initialize();

	/* Initialize the battery service server environment */
	Bass_Env_Initialize();
}
