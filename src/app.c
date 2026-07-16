#include "app.h"
#include "mcc_generated_files/system/pins.h"
#include "systick/systick.h"

// Button state tracking
typedef struct {
    bool last_state;
    bool debounced_state;
    uint32_t last_debounce_time;
    bool was_pressed;
} app_btn_t;

// Debounce interval in milliseconds
#define APP_BUTTON_DEBOUNCE_MS  20

static app_btn_t sw1_btn = {0};
static app_btn_t sw2_btn = {0};

static bool read_raw_sw1(void) { return (SW1_GetValue() == 0); } // active low
static bool read_raw_sw2(void) { return (SW2_GetValue() == 0); }

void APP_Buttons_Init(void)
{
    // SW1_SetDigitalInput(); // Initialization done in MCC
    // SW2_SetDigitalInput(); // Initialization done in MCC
    sw1_btn = (app_btn_t){0};
    sw2_btn = (app_btn_t){0};
}

static void debounce_button(app_btn_t* btn, bool raw_state)
{
    uint32_t now = SysTick_GetTickMs();
    if (raw_state != btn->last_state) {
        btn->last_debounce_time = now;
    }
    if ((now - btn->last_debounce_time) > APP_BUTTON_DEBOUNCE_MS) {
        // stable for debounce interval
        if (btn->debounced_state == false && raw_state == true) {
            btn->was_pressed = true; // set on rising edge (pressed)
        }
        btn->debounced_state = raw_state;
    }
    btn->last_state = raw_state;
}

// Call this periodically (e.g. from main loop)
void APP_Buttons_Task(void)
{
    debounce_button(&sw1_btn, read_raw_sw1());
    debounce_button(&sw2_btn, read_raw_sw2());
}

bool APP_Button_IsPressed(APP_BTN_ID btn_id)
{
    if (btn_id == APP_BTN_SW1) {
        return sw1_btn.debounced_state;
    } else if (btn_id == APP_BTN_SW2) {
        return sw2_btn.debounced_state;
    }
    return false;
}

bool APP_Button_WasPressed(APP_BTN_ID btn_id)
{
    if (btn_id == APP_BTN_SW1 && sw1_btn.was_pressed) {
        sw1_btn.was_pressed = false;
        return true;
    } else if (btn_id == APP_BTN_SW2 && sw2_btn.was_pressed) {
        sw2_btn.was_pressed = false;
        return true;
    }
    return false;
}
