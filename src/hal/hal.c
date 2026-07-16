#include "hal.h"

#include "mcc_generated_files/system/system.h"

void HAL_Init(void)
{
    SYSTEM_Initialize();
}