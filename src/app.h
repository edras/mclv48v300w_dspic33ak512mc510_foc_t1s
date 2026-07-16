#ifndef APP_H
#define APP_H

#include <stdbool.h>

typedef enum {
    APP_BTN_NONE = 0,
    APP_BTN_SW1,
    APP_BTN_SW2
} APP_BTN_ID;

void APP_Buttons_Init(void);
void APP_Buttons_Task(void);
bool APP_Button_IsPressed(APP_BTN_ID btn_id);
bool APP_Button_WasPressed(APP_BTN_ID btn_id);

#endif // APP_H
