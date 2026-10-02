#include "hal.h"

uint32_t get_mcu_uid(void)
{
    return *((volatile uint32_t *)SYSTEM_DeviceIdRegisterAddressGet());
}

void HAL_Init(void)
{
    SYSTEM_Initialize();
}