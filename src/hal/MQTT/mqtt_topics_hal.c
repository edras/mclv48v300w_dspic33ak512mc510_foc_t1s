/* ************************************************************************** */
/** mqtt_topics_hal.c

  @Company
    Microchip Technology Inc.

  @File Name
    mqtt_topics_hal.c

  @Summary
    MQTT topics for dsPIC33AK512MC510 Smart Motor (predictive maintenance).

  @Description
    Registers the same MQTT topics as the aiot-bluewedge smart_motor project
    so the dsPIC board behaves identically on the T1S MQTT network.
    Topics: smart_motor/speed, smart_motor/torque, smart_motor/on, etc.
 */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_topics_hal.h"
#include "MQTT/mqtt_topics.h"
#include "mc/mc_app.h"
#include "app.h"

/* ---- Application state for remote control ---- */

static bool remote_control = false;
static uint8_t aiml_mode = 0;
static uint8_t aiml_result = 0;
static int16_t aiml_status = 0;

bool is_remote_control(void)       { return remote_control; }
void set_remote_control(bool state) { remote_control = state; }
uint8_t get_aiml_mode(void)        { return aiml_mode; }
uint8_t get_aiml_result(void)      { return aiml_result; }
int16_t get_aiml_status(void)      { return aiml_status; }

/* ---- Publish callbacks ---- */

static void get_vcc_str(char *data)
{
    sprintf(data, "%.2f", mcApp.controlScheme.vdc);
}

static void get_board_name_str(char *data)
{
    sprintf(data, "%s", BOARD_LABEL);
}

static void get_switch_str(char *data)
{
    sprintf(data, "%s", APP_Button_IsPressed(APP_BTN_SW1) ? "true" : "false");
}

static void get_speed_str(char *data)
{
    sprintf(data, "%4.0f", mcApp.controlScheme.estimatorInterface.speedMech.RPM);
}

static void get_torque_str(char *data)
{
    sprintf(data, "%1.4f", mcApp.controlScheme.idq.q);
}

static void get_pot_str(char *data)
{
    sprintf(data, "%.0f", mcApp.targetSpeed);
}

static void get_failure_str(char *data)
{
    sprintf(data, "%d", aiml_result);
}

static void get_aiml_status_str(char *data)
{
    sprintf(data, "%d", aiml_status);
}

/* ---- Subscribe callbacks ---- */

static void handle_led(char *data)
{
    if (data == NULL) return;
    if (strcmp(data, "true") == 0)
        LED1_SetHigh();
    else
        LED1_SetLow();
}

static void handle_on(char *data)
{
    if (data == NULL) return;
    if (is_remote_control())
    {
        bool running = (mcApp.runCmd == 1);
        bool requested = (strcmp(data, "true") == 0);
        if (requested != running)
        {
            mcApp.runCmdBuffer = requested ? 1 : 0;
        }
    }
}

static void handle_speed_sp(char *data)
{
    if (data == NULL) return;
    if (is_remote_control())
    {
        mcApp.targetSpeed = (float)atof(data);
    }
}

static void handle_rctrl(char *data)
{
    if (data == NULL) return;
    set_remote_control(strcmp(data, "true") == 0);
}

static void handle_aiml_mode(char *data)
{
    if (data == NULL) return;
    aiml_mode = (uint8_t)atoi(data);
}

/* ---- Topic registration ---- */

void MQTT_init_topics_hal(void)
{
    TopicItem topics[] = {
    //  nodename        topic_name        publish_cb            subscribe_cb       autoPublish
        {"smart_motor", "reserved",       NULL,                 NULL,              false},
        {"smart_motor", "vcc",            get_vcc_str,          NULL,              true},
        {"smart_motor", "board_name",     get_board_name_str,   NULL,              true},
        {"smart_motor", "switch",         get_switch_str,       NULL,              true},
        {"smart_motor", "led",            NULL,                 handle_led,        false},
        {"smart_motor", "on",             NULL,                 handle_on,         false},
        {"smart_motor", "speed",          get_speed_str,        NULL,              true},
        {"smart_motor", "speed_setpoint", NULL,                 handle_speed_sp,   false},
        {"smart_motor", "torque",         get_torque_str,       NULL,              true},
        {"smart_motor", "remote_control", NULL,                 handle_rctrl,      false},
        {"smart_motor", "failure",        get_failure_str,      NULL,              true},
        {"smart_motor", "pot",            get_pot_str,          NULL,              true},
        {"smart_motor", "aiml_mode",      NULL,                 handle_aiml_mode,  false},
        {"smart_motor", "aiml_status",    get_aiml_status_str,  NULL,              true},
    };

    for (unsigned int i = 0; i < sizeof(topics) / sizeof(TopicItem); i++)
    {
        MQTT_insert_topic(topics[i]);
    }
}

/* *****************************************************************************
 End of File
 */
