/* ----------------------------------------------------------------------------
 * Copyright (c) 2016 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 * ----------------------------------------------------------------------------
 * app.c
 * - Main application file
 * ----------------------------------------------------------------------------
 * $Revision: 1.65 $
 * $Date: 2017/12/01 17:42:47 $
 * ------------------------------------------------------------------------- */

#include "include/app.h"

#ifdef ONE_SHOT_MODE
	OneShotMode_t OneShotMode[2] = {NCT375_ONEShot_StartSample,  NCT375_ONEShot_ReadSample};
	volatile int seq = 0;
#endif

int main() {

	App_Initialize();

	/* Power NCT375 */
#ifdef ONE_SHOT_MODE
    NCT375_ONEShot_ModeOn();
#else
    NCT375_PowerUp();
#endif

	/* Turn LED on */
	Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_1);

	/* Disable DIO4 and DIO5 to avoid current consumption on VDDO */
	Sys_DIO_Config(4, DIO_MODE_DISABLE | DIO_NO_PULL);
	Sys_DIO_Config(5, DIO_MODE_DISABLE | DIO_NO_PULL);

	/* Wait for 3 seconds to allow re-flashing directly after pressing RESET */
	Sys_Delay_ProgramROM(3 * SystemCoreClock);

	/* If the source clock is RC oscillator, measure and update its period */
	if (RTC_CLK_SRC == RTC_CLK_SRC_RC_OSC) {
		/* Start period counter to start period measurement */
		AUDIOSINK_CTRL->PERIOD_CNT_START_ALIAS = 1;

		/* Delay to make sure sampleCounter variable is incremented before
		 * moving onto the measurement loop */
		Sys_Delay_ProgramROM(0.001 * SystemCoreClock);
	}

	/* Main application loop */
	Main_Loop();
}

/* ----------------------------------------------------------------------------
 * Function      : Main_Loop(void)
 * ----------------------------------------------------------------------------
 * Description   : - Run the kernel scheduler
 *                 - Update the battery voltage when applicable
 *                 - Update custom service data when applicable
 *                 - Attempt to go to sleep mode if possible
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Main_Loop(void) {
	Sys_Watchdog_Refresh();

	if ((cs_env.sentSuccess == 1) && (app_env.sleep_cycles % 20 == 0)) {
		cs_env.sentSuccess = 0;
		cs_env.tx_value_changed = 1;
	}
	(app_env.sleep_cycles)++;

	if (ble_env.adv_count % 10 == 0) {
		/* Configure ADC channel 0 to measure VBAT/2 */
		Sys_ADC_Set_Config(
				ADC_VBAT_DIV2_NORMAL | ADC_CONTINUOUS | ADC_PRESCALE_6400);
		Sys_ADC_InputSelectConfig(0,
				(ADC_NEG_INPUT_GND | ADC_POS_INPUT_VBAT_DIV2));

		/* Wait for ADC to measure voltage */
		Sys_Delay_ProgramROM(8*6500);
		/* Read the battery level and update the average value */
		Measure_Battery_Level();
	}
	ble_env.adv_count++;
	ble_env.i2c_tx_buffer[0] = 0x00;

	/* Configure I2C Interface */
	I2C_Master_Init(0x80U);
	Sys_I2C_DIOConfig(DIO_6X_DRIVE | DIO_LPF_ENABLE | DIO_STRONG_PULL_UP,
			I2C_SCL_DIO_NUM, I2C_SDA_DIO_NUM);
#ifdef ONE_SHOT_MODE
	OneShotMode[seq]();
	seq=(seq+1)%2;
	NCT375_I2C_Delay();
#else
	I2C_WriteRead(0x48, ble_env.i2c_tx_buffer, 1, ble_env.i2c_rx_buffer, 2,
			NCT375_Received_Temperature);
#endif
	Advertising_Update();

	Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_0);
	while (true) {
		Kernel_Schedule();

		Sys_Watchdog_Refresh();

		Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_0);
		GLOBAL_INT_DISABLE();
		BLE_Power_Mode_Enter(&sleep_mode_env, POWER_MODE_SLEEP);
		GLOBAL_INT_RESTORE();

		/* Turn LED on */
		Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_1);
	}

	/* Wait for an interrupt before executing the scheduler again */
	SYS_WAIT_FOR_INTERRUPT;

}
