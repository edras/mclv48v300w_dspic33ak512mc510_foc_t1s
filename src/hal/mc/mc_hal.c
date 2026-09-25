/**
 * @file mc_hal.c
 *
 * @brief Hardware Abstraction Layer for Motor Control - MCC Melody implementation.
 * 
 * This file maps the generic MC HAL interface to the dsPIC33AK512MC510 
 * peripherals configured by MCC Melody (ADC1/2/3, PWM PG1/PG2/PG3).
 */

#include <xc.h>
#include <stdint.h>

#include "mc_hal.h"

/* MCC Melody generated peripheral headers */
#include "mcc_generated_files/adc/adc1.h"
#include "mcc_generated_files/adc/adc2.h"
#include "mcc_generated_files/adc/adc3.h"
#include "mcc_generated_files/cmp/cmp3.h"

#include "mc/mc_calc_params.h"

void MC_HAL_MotorInputsRead(MCAPP_MEASURE_T *pMotorInputs)
{
    /* Phase currents: centered around mid-scale (2048), scaled to Q15 format */
    pMotorInputs->measureCurrent.Ia = (int16_t)((HALF_ADC_COUNT - (int16_t)ADC1_ConversionResultGet(ADC1_IA)) << 4);
    pMotorInputs->measureCurrent.Ib = (int16_t)((HALF_ADC_COUNT - (int16_t)ADC2_ConversionResultGet(ADC2_IB)) << 4);
    pMotorInputs->measureCurrent.Ibus = (int16_t)(((int16_t)ADC3_ConversionResultGet(ADC3_IBUS) - HALF_ADC_COUNT) << 4);

    /* Potentiometer */
    pMotorInputs->measurePot = (int16_t)ADC2_ConversionResultGet(ADC2_POT);

    /* DC bus voltage */
    pMotorInputs->measureVdc.count = (int16_t)ADC3_ConversionResultGet(ADC3_VBUS);
}

void MC_HAL_PWMEnableOutputs(void)
{
    /* Set duty cycles to zero before enabling */
    PG1DCbits.DC = 0;
    PG2DCbits.DC = 0;
    PG3DCbits.DC = 0;

    /* Remove override - PWM generators control the output pins */
    PG3IOCON2bits.OVRENH = 0;
    PG3IOCON2bits.OVRENL = 0;
    PG2IOCON2bits.OVRENH = 0;
    PG2IOCON2bits.OVRENL = 0;
    PG1IOCON2bits.OVRENH = 0;
    PG1IOCON2bits.OVRENL = 0;
}

void MC_HAL_PWMDisableOutputs(void)
{
    /* Set duty cycles to zero */
    PG1DCbits.DC = 0;
    PG2DCbits.DC = 0;
    PG3DCbits.DC = 0;

    /* Override data = LOW on all outputs */
    PG3IOCON2bits.OVRDAT = 0;
    PG2IOCON2bits.OVRDAT = 0;
    PG1IOCON2bits.OVRDAT = 0;

    /* Activate override - OVRDAT controls the output pins */
    PG3IOCON2bits.OVRENH = 1;
    PG3IOCON2bits.OVRENL = 1;
    PG2IOCON2bits.OVRENH = 1;
    PG2IOCON2bits.OVRENL = 1;
    PG1IOCON2bits.OVRENH = 1;
    PG1IOCON2bits.OVRENL = 1;
}

void MC_HAL_PWMDutySet(MC_DUTYCYCLEOUT_T *pPdc)
{
    /* Clamp duty cycles */
    if (pPdc->dutycycle1 < MIN_DUTY) pPdc->dutycycle1 = MIN_DUTY;
    if (pPdc->dutycycle2 < MIN_DUTY) pPdc->dutycycle2 = MIN_DUTY;
    if (pPdc->dutycycle3 < MIN_DUTY) pPdc->dutycycle3 = MIN_DUTY;
    if (pPdc->dutycycle1 > MAX_DUTY) pPdc->dutycycle1 = MAX_DUTY;
    if (pPdc->dutycycle2 > MAX_DUTY) pPdc->dutycycle2 = MAX_DUTY;
    if (pPdc->dutycycle3 > MAX_DUTY) pPdc->dutycycle3 = MAX_DUTY;

    /* Write to PWM duty cycle registers */
    PG1DCbits.DC = (uint32_t)(pPdc->dutycycle1);
    PG2DCbits.DC = (uint32_t)(pPdc->dutycycle2);
    PG3DCbits.DC = (uint32_t)(pPdc->dutycycle3);
}

void MC_HAL_OverCurrentProtectionInit(void)
{
    /* Apply device-specific DAC calibration from flash (MCC doesn't call this) */
    CMP3_Calibrate();

    /* Leading-edge blanking: MCC enables CBE but leaves TMCB = 0.
     * TMCB value = desired_time * Fp, where Fp is the peripheral clock.
     * At Fp = 200 MHz: TMCB = 100 gives 500 ns blanking. Adjust as needed. */
    DAC3CONbits.TMCB = 100;

    /* Configure PCI1 for cycle-by-cycle fault behavior (MCC only sets PCI2 source).
     * 0x03001000: ACP[26:24]=3 (latched), TERM[14:12]=1 (auto-terminate at EOC) */
    PG1CONbits.ON = 0;
    PG2CONbits.ON = 0;
    PG3CONbits.ON = 0;

    PG1F1PCI1 = 0x03001000;
    PG2F1PCI1 = 0x03001000;
    PG3F1PCI1 = 0x03001000;

    PG1CONbits.ON = 1;
    PG2CONbits.ON = 1;
    PG3CONbits.ON = 1;
}
