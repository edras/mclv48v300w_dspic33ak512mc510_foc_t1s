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
 * This file implements X2Cscope_Init for the dsPIC33AK512_T1S project.
 * Communication is over TCP/IP via the 10BASE-T1S interface (see X2CscopeComm.c).
 */
#include "X2CscopeComm.h"
#include "X2Cscope.h"

/* Build date/time stamp — readable via the X2Cscope "Get Device Info" service.
 * Must be const and global: X2Cscope_InitialiseEx stores a pointer to it. */
const compilationDate_t compilationDate = {__DATE__, __TIME__};

/* Scope data buffer — size controlled by X2CSCOPE_BUFFER_SIZE (default 5000 bytes). */
int8_t X2CscopeArray[X2CSCOPE_BUFFER_SIZE];

void X2Cscope_Init(void)
{
    X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
        sendSerial,                 /* send one byte over TCP        */
        receiveSerial,              /* receive one byte from TCP     */
        isReceiveDataAvailable,     /* RX data ready flag            */
        isSendReady,                /* TX always ready (TCP buffer)  */
        flushSerial,                /* flush: sends TCP tx buffer    */
        (void*)X2CscopeArray,       /* scope data buffer             */
        X2CSCOPE_BUFFER_SIZE,       /* scope buffer size             */
        X2CSCOPE_APP_VERSION,       /* app version identifier        */
        compilationDate             /* build timestamp               */
    );
    X2Cscope_InitialiseEx(&config);
    X2CscopeComm_PostInit(); /* start TCP server, see X2CscopeComm.c */
}
