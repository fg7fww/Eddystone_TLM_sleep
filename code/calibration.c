/* ----------------------------------------------------------------------------
 * Copyright (c) 2017 Semiconductor Components Industries, LLC (d/b/a
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
 * calibration.c
 * - board calibration
 * ----------------------------------------------------------------------------
 * $Revision: 1.16 $
 * $Date: 2017/11/27 22:07:44 $
 * ------------------------------------------------------------------------- */

#include "calibration.h"

#if (CALIB_RECORD == MANU_CALIB)

/* ----------------------------------------------------------------------------
 * Function      : void Load_Trim_Values_And_Calibrate_MANU_CALIB(void)
 * ----------------------------------------------------------------------------
 * Description   : Load trim values and calibrate the module to target voltages
 *                 using pre-loaded calibration records
 * Inputs        : Target voltages defined in cabliration.h
 * Outputs       : None
 * Assumptions   : Calibration records were loaded in NVR4 during manufacturing
 * ------------------------------------------------------------------------- */
uint8_t Load_Trim_Values_And_Calibrate_MANU_CALIB(void)
{
    uint8_t error_code;

    error_code = VOLTAGES_CALIB_NO_ERROR;

    /* -------------- Load pre-calculated VCC trim value -------------- */
    if (Sys_Power_DCDCCalibratedConfig(VCC_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VCC_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDRF trim value ------------- */
    if (Sys_Power_VDDRFCalibratedConfig(VDDRF_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDRF_CALIB_ERROR;
    }

#ifdef POWER_AMPLIFIER_ON

    /* ------------- Load pre-calculated VDDPA trim value ------------- */
    if (Sys_Power_VDDPACalibratedConfig(VDDPA_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDPA_CALIB_ERROR;
    }
#endif /* POWER_AMPLIFIER_ON */

    /* -------------- Load pre-calculated VDDC trim value ------------- */
    if (Sys_Power_VDDCCalibratedConfig(VDDC_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDC_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDM trim value -------------- */
    if (Sys_Power_VDDMCalibratedConfig(VDDM_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }

    return(error_code);
}

#elif (CALIB_RECORD == SUPPLEMENTAL_CALIB)

/* ----------------------------------------------------------------------------
 * Function      : void Load_Trim_Values_And_Calibrate_SUPPLEMENTAL_CALIB(void)
 * ----------------------------------------------------------------------------
 * Description   : Load trim values and calibrate the module to target voltages
 *                 using pre-loaded calibration records
 * Inputs        : Target voltages defined in app.h
 * Outputs       : None
 * Assumptions   : Calibration records were loaded in NVR3 by the
 *                 supplemental_calibate application
 * ------------------------------------------------------------------------- */
uint8_t Load_Trim_Values_And_Calibrate_SUPPLEMENTAL_CALIB(void)
{
    uint8_t* vcc_trim_ptr;
    uint8_t* vddrf_trim_ptr;
    uint8_t* vddpa_trim_ptr;
    uint8_t* vddc_trim_ptr;
    uint8_t* vddm_trim_ptr;

    uint16_t* vcc_target_ptr;
    uint16_t* vddrf_target_ptr;
    uint16_t* vddpa_target_ptr;
    uint16_t* vddc_target_ptr;
    uint16_t* vddm_target_ptr;

    uint8_t error_code;

    error_code = VOLTAGES_CALIB_NO_ERROR;

    /* ------------- Load pre-calculated VCC trim value --------------- */
    vcc_trim_ptr = (uint8_t *) (0x00081074);
    vcc_target_ptr = (uint16_t *) (0x00081076);

    if ((uint16_t) (*vcc_target_ptr) == VCC_TARGET)
    {
        /* Load the trim register, making sure the existing data in VCC_CTRL
         * is not over-written */
        ACS_VCC_CTRL->VTRIM_BYTE = (ACS_VCC_CTRL->VTRIM_BYTE &
                                    ~ACS_VCC_CTRL_VTRIM_Mask) |
                                   (*vcc_trim_ptr);
    }
    else
    {
        error_code |= VCC_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDRF trim value ------------- */
    vddrf_trim_ptr = (uint8_t *) (0x00081064);
    vddrf_target_ptr = (uint16_t *) (0x00081066);

    if ((uint16_t) (*vddrf_target_ptr) == VDDRF_TARGET)
    {
        /* Load the trim register, making sure the existing data in VDDRF_CTRL
         * is not over-written */
        ACS_VDDRF_CTRL->VTRIM_BYTE = (ACS_VDDRF_CTRL->VTRIM_BYTE &
                                      ~ACS_VDDRF_CTRL_VTRIM_Mask) |
                                     (*vddrf_trim_ptr);
    }
    else
    {
        error_code |= VDDRF_CALIB_ERROR;
    }

#ifdef POWER_AMPLIFIER_ON

    /* ------------- Load pre-calculated VDDPA trim value ------------- */
    vddpa_trim_ptr = (uint8_t *) (0x00081068);
    vddpa_target_ptr = (uint16_t *) (0x0008106A);

    if ((uint16_t) (*vddpa_target_ptr) == VDDPA_TARGET)
    {
        /* Load the trim register, making sure the existing data in VDDPA_CTRL
         * is not over-written */
        ACS_VDDPA_CTRL->VTRIM_BYTE = (ACS_VDDPA_CTRL->VTRIM_BYTE &
                                      ~ACS_VDDPA_CTRL_VTRIM_Mask) |
                                     (*vddpa_trim_ptr);
    }
    else
    {
        error_code |= VDDPA_CALIB_ERROR;
    }
#endif /* POWER_AMPLIFIER_ON */

    /* ------------- Load pre-calculated VDDC trim value -------------- */
    vddc_trim_ptr = (uint8_t *) (0x0008106C);
    vddc_target_ptr = (uint16_t *) (0x0008106E);

    if ((uint16_t) (*vddc_target_ptr) == VDDC_TARGET)
    {
        /* Load the trim register, making sure the existing data in VDDC_CTRL
         * is not over-written */
        ACS_VDDC_CTRL->VTRIM_BYTE = (ACS_VDDC_CTRL->VTRIM_BYTE &
                                     ~ACS_VDDC_CTRL_VTRIM_Mask) |
                                    (*vddc_trim_ptr);
    }
    else
    {
        error_code |= VDDC_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDM trim value -------------- */
    vddm_trim_ptr = (uint8_t *) (0x00081070);
    vddm_target_ptr = (uint16_t *) (0x00081072);

    if ((uint16_t) (*vddm_target_ptr) == VDDM_TARGET)
    {
        /* Load the trim register, making sure the existing data in VDDM_CTRL
         * is not over-written */
        ACS_VDDM_CTRL->VTRIM_BYTE = (ACS_VDDM_CTRL->VTRIM_BYTE &
                                     ~ACS_VDDM_CTRL_VTRIM_Mask) |
                                    (*vddm_trim_ptr);
    }
    else
    {
        error_code |= VDDM_CALIB_ERROR;
    }

    return(error_code);
}

#elif (CALIB_RECORD == USER_CALIB)

/* ----------------------------------------------------------------------------
 * Function      : void Calculate_Trim_Values_And_Calibrate(void)
 * ----------------------------------------------------------------------------
 * Description   : Calculate trim values and calibrate the module
 *                 to target voltages
 * Inputs        : Target values defined in app.h
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
uint8_t Calculate_Trim_Values_And_Calibrate(void)
{
    /* Initialize variables */

    CalSetting cal_values = {0};

    uint16_t *init_ptr;

    unsigned int result;

    uint8_t error_code;

    uint32_t vbg_calibration_value = 0;

    /* Refresh the watchdog timer */
    Sys_Watchdog_Refresh();

    error_code = VOLTAGES_CALIB_NO_ERROR;

    /* Initialize cal_values struct to 0xffff. The for loop is configured as
     * follows: Start:     a pointer to the start of the cal_values struct
     *          End:       The memory address at the end of the cal_values
     *                     struct, calculated by adding the size of cal_values
     *                     to the base address of cal_values. The division by
     *                     two is because the sizeof operator returns the size
     *                     in 8 bits while the pointer we're using to increment
     *                     is 16 bits.
     *          Increment: Increment the pointer by one, which amounts to 16
     *                     bits since we're using a 16 bit pointer. */
    for (init_ptr = (uint16_t *) (&cal_values);
         init_ptr < ((uint16_t *) (sizeof(cal_values) / 2 +
                                   (uint16_t *) &cal_values));
         init_ptr++)
    {
        *init_ptr = 0xffff;
    }

    /* Initializes the system state */
    Calibrate_Power_Initialize();

    /* Current System State: SYSCLK = RFOSC/3 = 16 MHz, SLOWCLK = 1 MHz,
     * ADC is configured for low frequency mode, sample rate is 100 Hz,
     * all power supply settings are at default values */

    /* ECC tracks errors in the flash memory. The system is initialized by
     * clearing any past errors found. */
    FLASH->ECC_STATUS = FLASH_ECC_UNCOR_ERROR_CNT_CLEAR |
                        FLASH_ECC_COR_ERROR_CNT_CLEAR;

    /* Read the VBG calibration trim data from NVR4 */
    Sys_ReadNVR4(MANU_INFO_BANDGAP, 1, (unsigned int *) &vbg_calibration_value);

    /* If the data from NVR4 pertains to calibrating to 0.75V (our target)
     * then load the trim data to the BG_CTRL register. If not, load the
     * default 0.75 trim data into the BG_CTRL register */
    if ((vbg_calibration_value >> (MANU_CAL_INFO_TARGET_POS)) == 75)
    {
        ACS_BG_CTRL->VTRIM_BYTE = (ACS_BG_CTRL->VTRIM_BYTE &
                                   ~ACS_BG_CTRL_VTRIM_Mask) |
                                  (uint8_t) (vbg_calibration_value &
                                             ACS_BG_CTRL_VTRIM_Mask);
    }
    else
    {
        ACS_BG_CTRL->VTRIM_BYTE = (ACS_BG_CTRL->VTRIM_BYTE &
                                   ~ACS_BG_CTRL_VTRIM_Mask) |
                                  BG_TRIM_0P750V;
    }

    /* VDDRF and VDDPA are the power supplies that will be calibrated to the
     * values specified by the corresponding targets. */

    /* Before calibrating VDDRF, we must set VCC to a value high enough to
     * support our desired VDDRF. This is because VCC is the supply for VDDRF.
     * VCC = max(VDDRF, VDDM, VDDC) + 50mV margin*/
    ACS_VCC_CTRL->VTRIM_BYTE = VCC_TRIM_BYTE_FOR_VDDRF_CALIB;

    /* ----------------------- Calibrate VDDRF ------------------------ */
    result = Calibrate_Power_VDDRF(0,
                                   (uint32_t *) &ADC->DATA_TRIM_CH[0],
                                   ADC_VDDRF_TARGET);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDRF_CALIB_ERROR;
    }
    else /* No error occurred, save the calibration data */
    {
        /* Save the VDDRF target value in the cal_values struct */
        cal_values.VDDRF_CAL_TARGET = ADC_VDDRF_TARGET;

        /* Save the VDDRF trim value in the cal_values struct */
        cal_values.VDDRF_CAL_TRIM_VALUE = ACS_VDDRF_CTRL->VTRIM_BYTE;
    }

#ifdef POWER_AMPLIFIER_ON

    /* ----------------------  Calibrate VDDPA  ----------------------- */
    result = Calibrate_Power_VDDPA(0, (uint32_t *) &ADC->DATA_TRIM_CH[0],
                                   ADC_VDDPA_TARGET);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDPA_CALIB_ERROR;
    }
    else /* No error occurred, save the calibration data */
    {
        /* Save the VDDPA target value in the cal_values struct */
        cal_values.VDDPA_CAL_TARGET = ADC_VDDPA_TARGET;

        /* Save the VDDPA trim value in the cal_values struct */
        cal_values.VDDPA_CAL_TRIM_VALUE = ACS_VDDPA_CTRL->VTRIM_BYTE;
    }
#endif

    /* ------------------------ Calibrate VDDC ------------------------ */
    result = Calibrate_Power_VDDC(0, (uint32_t *) &ADC->DATA_TRIM_CH[0],
                                  ADC_VDDC_TARGET);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDC_CALIB_ERROR;
    }
    else /* No error occurred, save the calibration data */
    {
        /* Save the VDDC target value in the cal_values struct */
        cal_values.VDDC_CAL_TARGET = ADC_VDDC_TARGET;

        /* Save the VDDC trim value in the cal_values struct */
        cal_values.VDDC_CAL_TRIM_VALUE = ACS_VDDC_CTRL->VTRIM_BYTE;
    }

    /* ------------------------ Calibrate VDDM ------------------------ */
    result = Calibrate_Power_VDDM(0, (uint32_t *) &ADC->DATA_TRIM_CH[0],
                                  ADC_VDDM_TARGET);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }
    else /* No error occurred, save the calibration data */
    {
        /* Save the VDDM target value in the cal_values struct */
        cal_values.VDDM_CAL_TARGET = ADC_VDDM_TARGET;

        /* Save the VDDM trim value in the cal_values struct */
        cal_values.VDDM_CAL_TRIM_VALUE = ACS_VDDM_CTRL->VTRIM_BYTE;
    }

    /* ------------------------ Calibrate DCDC ------------------------ */
    result = Calibrate_Power_DCDC(0, (uint32_t *) &ADC->DATA_TRIM_CH[0],
                                  ADC_DCDC_TARGET);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VCC_CALIB_ERROR;
    }
    else /* No error occurred, save the calibration data */
    {
        /* Save the DCDC target value in the cal_values struct */
        cal_values.DCDC_CAL_TARGET = ADC_DCDC_TARGET;

        /* Save the VDDM trim value in the cal_values struct */
        cal_values.DCDC_CAL_TRIM_VALUE = ACS_VCC_CTRL->VTRIM_BYTE;
    }

    /* Refresh the watchdog timer */
    Sys_Watchdog_Refresh();

    /* It may be desirable to set a breakpoint over here during testing and
     * check the errors array to make sure no calibration failure have occurred
     */

    return(error_code);
}

#endif /* CALIB_RECORD */
