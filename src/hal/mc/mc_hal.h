/**
 * @file mc_hal.h
 *
 * @brief Hardware Abstraction Layer for Motor Control.
 * 
 * This interface decouples the portable FOC algorithm (src/mc/) from 
 * hardware-specific peripherals (ADC, PWM). To port to a different board 
 * or device, only mc_hal.c needs to be re-implemented.
 */

#ifndef MC_HAL_H
#define MC_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mc/measure.h"
#include "mc/foc/svm.h"

/**
 * @brief Read all motor-related ADC inputs
 * @param pMotorInputs Pointer to measurement structure to populate
 */
void MC_HAL_MotorInputsRead(MCAPP_MEASURE_T *pMotorInputs);

/**
 * @brief Enable PWM outputs (remove override, PWM generators drive the pins)
 */
void MC_HAL_PWMEnableOutputs(void);

/**
 * @brief Disable PWM outputs (override all outputs LOW)
 */
void MC_HAL_PWMDisableOutputs(void);

/**
 * @brief Write duty cycle values to PWM registers
 * @param pPdc Pointer to duty cycle structure from SVM output
 */
void MC_HAL_PWMDutySet(MC_DUTYCYCLEOUT_T *pPdc);

/**
 * @brief Initialize hardware overcurrent protection (CMP3 + PWM Fault PCI)
 * 
 * Configures CMP3/DAC3 as the overcurrent comparator and wires it to
 * PWM Fault PCI on all three generators in cycle-by-cycle mode.
 * Call this AFTER MCC SYSTEM_Initialize() to override default PCI settings.
 */
void MC_HAL_OverCurrentProtectionInit(void);

#ifdef __cplusplus
}
#endif

#endif /* MC_HAL_H */
