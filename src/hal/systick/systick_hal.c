#include "../../systick/systick.h"
#include "systick_hal.h"

#include "mcc_generated_files/timer/tmr1.h"

void SysTick_hal_Initialize(void (*CallbackHandler)(void))
{
    // Initialize the hardware timer for systick
    Timer1.TimeoutCallbackRegister(CallbackHandler);
    Timer1.Start();
}

void SysTick_DelayMs(uint32_t milliseconds)
{
    __delay_ms(milliseconds);
}

void SysTick_DelayUs(uint32_t microseconds)
{
    __delay_us(microseconds);
}