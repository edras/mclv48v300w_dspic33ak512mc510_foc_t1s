/* ************************************************************************** */
/** mqtt_topics_hal.h

  @Company
    Microchip Technology Inc.

  @File Name
    mqtt_topics_hal.h

  @Summary
    Platform-specific MQTT topic definitions for dsPIC33AK FOC drive.
 */
/* ************************************************************************** */

#ifndef _MQTT_TOPICS_HAL_H
#define _MQTT_TOPICS_HAL_H

#include "hal.h"

void MQTT_init_topics_hal(void);
bool is_remote_control(void);

#endif /* _MQTT_TOPICS_HAL_H */

/* *****************************************************************************
 End of File
 */
