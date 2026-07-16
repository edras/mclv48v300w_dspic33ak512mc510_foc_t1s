#ifndef _SYSTICK_HAl
#define _SYSTICK_HAl

// Support a delay method based on system peripheral clock.
#ifndef FCY
    #include "mcc_generated_files/system/clock.h"
    #define FCY CLOCK_FastPeripheralFrequencyGet()
#endif
#include <libpic30.h>

#include "mcc_generated_files/system/interrupt.h"

#define ENTER_CRITICAL()   (INTERRUPT_GlobalDisable())
#define LEAVE_CRITICAL()   (INTERRUPT_GlobalEnable())

void SysTick_hal_Initialize(void (*CallbackHandler)(void));

#endif