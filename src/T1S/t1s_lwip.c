/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#include "t1s_lwip.h"
#include "tc6-lwip.h"
#include "tc6-stub.h"
#include "T1S/t1s_lwip_hal.h"
#include <string.h>
#include <stdio.h>

#include "lwip/apps/lwiperf.h"
#include "udp_perf_client.h"

#define FIRMWARE_VERSION            TC6_LIB_VER_STRING

#ifndef T1S_BOARD_INSTANCE
#define T1S_BOARD_INSTANCE          (1)
#endif
#define T1S_PLCA_ENABLE             (true)
#define T1S_PLCA_NODE_ID            (T1S_BOARD_INSTANCE + 1)
#define T1S_PLCA_NODE_COUNT         (8)
#define T1S_PLCA_BURST_COUNT        (0)
#define T1S_PLCA_BURST_TIMER        (0x80)
#define MAC_PROMISCUOUS_MODE        (false)
#define MAC_TX_CUT_THROUGH          (false)
#define MAC_RX_CUT_THROUGH          (false)
#define DELAY_BEACON_CHECK          (1000)

#define ESC_CLEAR_TERMINAL          "\033[2J"
#define ESC_CURSOR_X1Y1             "\033[1;1H"
#define ESC_HIDE_CURSOR             "\033[?25l"
#define ESC_CLEAR_LINE              "\033[2K"
#define ESC_RESETCOLOR              "\033[0m"
#define ESC_GREEN                   "\033[0;32m"
#define ESC_RED                     "\033[0;31m"
#define ESC_YELLOW                  "\033[1;33m"
#define ESC_BLUE                    "\033[0;36m"

bool debug = true;
bool bridge = false;

typedef struct {
    uint32_t nextCycle;
    uint32_t nextBeaconCheck;
    uint32_t nextLed;
    int8_t idxLwIp;
    bool button1;
    bool lastBeaconState;
} MainLocal_t;


static MainLocal_t m;
static const uint8_t m_ip[] = {192, 168, 0, (100 + T1S_BOARD_INSTANCE)};
char t1s_msg[100];

/**
 * Internal function prototypes
 */
void T1S_OnPlcaStatus(int8_t idx, bool success, bool plcaStatus);
void T1S_CheckPlcaStatus(void);
void T1S_OnIperfResult(void *arg, enum lwiperf_report_type report_type, const ip_addr_t *local_addr, u16_t local_port,
        const ip_addr_t *remote_addr, u16_t remote_port, u32_t bytes_transferred, u32_t ms_duration,
        u32_t bandwidth_kbitpsec);

/**
 * 
 */

void T1S_print_message(char *message) 
{
    if (bridge || debug)
    {
        T1S_TERMINAL_Write("%s", message);
        //while (!T1S_TERMINAL_Done());
    }
}

void T1S_execute(void) 
{
    if (!T1S_available()) return;

    T1S_CheckPlcaStatus();

    iperf_service();

    TC6LwIP_Service();
}

void T1S_print_menu(void) {
    T1S_print_message("\r\n=== Available Keys ===\r\n");
    T1S_print_message(" m - print this menu\r\n");
    T1S_print_message(" r - soft reset\r\n");
    T1S_print_message(" c - clear screen\r\n");
    T1S_print_message("======================\r\n");
}

void T1S_CheckPlcaStatus(void) {
    uint32_t now = T1S_GetTickCountMs();
    if (now > m.nextBeaconCheck) {
        m.nextBeaconCheck = now + DELAY_BEACON_CHECK;
        if (!TC6LwIP_GetPlcaStatus(m.idxLwIp, T1S_OnPlcaStatus)) {
            T1S_print_message(ESC_RED "GetPlcaStatus failed" ESC_RESETCOLOR "\r\n");
        }
    }
}

void T1S_OnPlcaStatus(int8_t idx, bool success, bool plcaStatus) {
    if (success) {
        if (plcaStatus != m.lastBeaconState) {
            m.lastBeaconState = plcaStatus;
            if (plcaStatus) {
                T1S_print_message(ESC_GREEN "PLCA Mode active" ESC_RESETCOLOR "\r\n");
            }
            else {
                T1S_print_message(ESC_RED "CSMA/CD fallback" ESC_RESETCOLOR "\r\n");
            }
        }
        m.lastBeaconState = plcaStatus;
    }
    else {
        T1S_print_message(ESC_RED "PLCA status register read failed" ESC_RESETCOLOR "\r\n");
    }
}

void T1S_send_cmd(char* buffer) {
    switch (buffer[0]) {
        case 'M':
        case 'm':
            T1S_print_menu();
            break;
        case 'R':
        case 'r':
            T1S_reset();
            while (true);
            break;
        case 'C':
        case 'c':
            T1S_print_message(ESC_CLEAR_TERMINAL ESC_CURSOR_X1Y1 ESC_HIDE_CURSOR);
            break;
        case 'Q':
        case 'q':
            ;
        default:
            sprintf(t1s_msg, "Unknown key='%c'(0x%X)\r\n", buffer[0], buffer[0]);
            T1S_print_message(t1s_msg);
            break;
    }
}

void T1S_OnIperfResult(void *arg, enum lwiperf_report_type report_type,
        const ip_addr_t *local_addr, u16_t local_port,
        const ip_addr_t *remote_addr, u16_t remote_port,
        u32_t bytes_transferred, u32_t ms_duration,
        u32_t bandwidth_kbitpsec) {
    sprintf(t1s_msg, "iperf result [Bytes=%ld Duration=%ld ms Bandwidth=%ld kbit/s]\r\n",
            bytes_transferred, ms_duration, bandwidth_kbitpsec);
    T1S_print_message(t1s_msg);
}

bool T1S_init(void)
{
    if(!T1S_AVAILABLE) return false;
     
    memset(&m, 0, sizeof(m));
    m.idxLwIp = TC6LwIP_Init(m_ip, T1S_PLCA_ENABLE, T1S_PLCA_NODE_ID, T1S_PLCA_NODE_COUNT,
            T1S_PLCA_BURST_COUNT, T1S_PLCA_BURST_TIMER, MAC_PROMISCUOUS_MODE,
            MAC_TX_CUT_THROUGH, MAC_RX_CUT_THROUGH);

    if (m.idxLwIp < 0) {
        T1S_print_message(ESC_RED "Failed to initialize TC6 lwIP Driver" ESC_RESETCOLOR "\r\n");
        return false;
    }

    T1S_print_message("T1S Firmware " FIRMWARE_VERSION " (" __DATE__ " " __TIME__ ") ===\r\n");

    /* iperf */
    iperf_init();

    m.nextBeaconCheck = DELAY_BEACON_CHECK;

    return true;
}

bool T1S_available(void) {
    return !(m.idxLwIp < 0);
}

void T1S_set_bridge(bool state) {
    bridge = state;
}

void T1S_set_debug(bool state) {
    debug = state;
}



/* *****************************************************************************
 End of File
 */
