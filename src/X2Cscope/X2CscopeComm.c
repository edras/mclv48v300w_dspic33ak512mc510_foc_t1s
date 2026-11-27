/*
Copyright (c) [2012-2020] Microchip Technology Inc.  
    All rights reserved.
    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
*/
/*
 * This file is licensed according to the BSD 3-clause license as follows:
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the "Linz Center of Mechatronics GmbH" and "LCM" nor
 *       the names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL "Linz Center of Mechatronics GmbH" BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * This file is part of X2Cscope firmware implementation.
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 */
/**
 * This file implements the X2C Lin protocol connection with MCC Peripheral drivers.
 */
#include "X2CscopeComm.h"
#include "T1S/tcp_server.h"

#include <stdint.h>

/* function prototypes */
void receiveTcpData(void* tcp_server_cfg, const uint8_t* data, uint16_t len);

#define TCP_RX_BUFSIZE 256

/* TCP server configuration */
TCP_Server x2cscope_tcp_server = {
    .port = 12666,
    .listen_pcb = NULL,
    .conn_pcb = NULL,
    .recv_fn = receiveTcpData
};

/* Circular buffer to receive data from TCP server */
typedef struct {
    uint8_t buf[TCP_RX_BUFSIZE];
    volatile uint16_t head, tail;
} TCP_CircularBuffer;

static TCP_CircularBuffer tcp_rx_buf = {0};

/* Transmit buffer for TCP server */
static uint8_t tcp_tx_buf[516] = {0}; // max LNET frame size with fill bytes
static uint16_t tx_head = 0;

void receiveTcpData(void* tcp_server_cfg, const uint8_t* data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uint16_t next_head = (tcp_rx_buf.head + 1) % TCP_RX_BUFSIZE;
        if (next_head != tcp_rx_buf.tail)
        {
            tcp_rx_buf.buf[tcp_rx_buf.head] = data[i];
            tcp_rx_buf.head = next_head;
        }
    }
}

/**
  @brief
    Called from X2Cscope_Init() after X2Cscope_InitialiseEx(), for steps that
    must follow library init (e.g. start a TCP server, enable the UART TX
    interrupt, start DMA transfers).
    Default: does nothing.
*/
void X2CscopeComm_PostInit(void)
{
    tcp_server_start(&x2cscope_tcp_server);
}

/** 
  @brief
    Puts the data to the hardware layer. (UART)
   @param[in] serial Serial interface object. (Not used)
   @param[in] data Data to send 
 */
void X2CscopeComm_SendByte(uint8_t data)
{
    tcp_tx_buf[tx_head++] = data;
}

/** 
  @brief
   Get serial data from hardware. Reset the hardware in case of error. (UART2)
  @param[in] serial Serial interface object. (Not used)
  @return
    Return with the received data
 */
uint8_t X2CscopeComm_ReceiveByte(void)
{
    if (tcp_rx_buf.head == tcp_rx_buf.tail)
    {
        return 0; // No data available
    }
    uint8_t data = tcp_rx_buf.buf[tcp_rx_buf.tail];
    tcp_rx_buf.tail = (tcp_rx_buf.tail + 1) % TCP_RX_BUFSIZE;
    return data;
}

/** 
  @brief  Check data availability (UART).
  @param[in] serial Serial interface object. (Not used)
  @return
    True -> Serial data ready to read.
    False -> No data.
 */
bool X2CscopeComm_IsByteAvailable()
{
    return (tcp_rx_buf.head != tcp_rx_buf.tail);
}

/** 
  @brief
   Check output buffer. (UART)
  @param[in] serial Serial interface object. (Not used)
  @return    
    True -> Transmit buffer is not full, at least one more character can be written.
    False -> Transmit buffer is full.
 */
bool X2CscopeComm_IsSendReady()
{
    return 1; // Always ready in TCP case
}

/**
  @brief
    Send the bytes collected by X2CscopeComm_SendByte(). Called when a frame is
    complete. Needed if the peripheral buffers TX data (e.g. CAN, TCP/IP).
    Default: does nothing.
*/
void X2CscopeComm_Flush(void)
{
    tcp_server_send(&x2cscope_tcp_server, tcp_tx_buf, tx_head);
    tx_head = 0;
}

/**
  @brief
    Reset the device when requested from the host.

    WARNING: when this function resets the device, the running application stops
    immediately and all outputs go to their reset state.
  @return
    true  -> the device is reset (now or shortly afterwards).
    false -> the device cannot be reset now for any safety reason.
    Default: the device is not reset; the host is told that the reset is not supported.
*/
bool X2CscopeComm_ResetDevice(void)
{
    // dsPIC30/33, PIC24 (XC16 / XC-DSC):
    __asm__ volatile ("reset");

    // PIC32M (XC32):
    // SYSKEY = 0x00000000; SYSKEY = 0xAA996655; SYSKEY = 0x556699AA;   // unlock
    // RSWRSTSET = 1; (void)RSWRST;                                     // software reset
    // while (1) {}

    // Cortex-M (CMSIS, e.g. SAM, PIC32C/CK/CM/CZ):
    // NVIC_SystemReset();

    return true;                // not reached on a direct reset
}
/* *****************************************************************************
 End of File
 */