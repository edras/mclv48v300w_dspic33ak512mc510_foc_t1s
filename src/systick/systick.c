/*------------------------------------------------------------------------------------------------*/
/* System Tick Handler                                                                            */
/* Copyright 2021, Microchip Technology Inc. and its subsidiaries.                                */
/*                                                                                                */
/* Redistribution and use in source and binary forms, with or without                             */
/* modification, are permitted provided that the following conditions are met:                    */
/*                                                                                                */
/* 1. Redistributions of source code must retain the above copyright notice, this                 */
/*    list of conditions and the following disclaimer.                                            */
/*                                                                                                */
/* 2. Redistributions in binary form must reproduce the above copyright notice,                   */
/*    this list of conditions and the following disclaimer in the documentation                   */
/*    and/or other materials provided with the distribution.                                      */
/*                                                                                                */
/* 3. Neither the name of the copyright holder nor the names of its                               */
/*    contributors may be used to endorse or promote products derived from                        */
/*    this software without specific prior written permission.                                    */
/*                                                                                                */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"                    */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE                      */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                 */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE                   */
/* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL                     */
/* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                     */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER                     */
/* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,                  */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE                  */
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/*------------------------------------------------------------------------------------------------*/

#include <string.h>
#include "systick.h"
#include "systick/systick_hal.h"


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      DEFINES AND LOCAL VARIABLES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/// @brief Defines the Systick object.
typedef struct
{
   volatile uint64_t tickCounter;
} SYSTICK_OBJECT;

static SYSTICK_OBJECT systick;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTION PROTOTYPES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static void SystickCallback(void);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                         PUBLIC FUNCTIONS                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void SysTick_Initialize(void)
{
    (void)memset(&systick, 0, sizeof(SYSTICK_OBJECT));
    SysTick_hal_Initialize(SystickCallback);
}

uint32_t SysTick_GetTickMs(void)
{
    volatile uint32_t now;

    ENTER_CRITICAL();
    now = (uint32_t)(systick.tickCounter / SYSTICK_TIMER_SCALER_MS);
    LEAVE_CRITICAL();

    return now;
}

uint64_t SysTick_GetTickUs(void)
{
    volatile uint64_t now;

    ENTER_CRITICAL();
    now = (systick.tickCounter * SYSTICK_TIMER_SCALER_US);
    LEAVE_CRITICAL();

    return now;
}

void SysTick_StartTimeOut (SYSTICK_TIMEOUT* timeout, uint32_t delay_ms)
{
    timeout->start = SysTick_GetTickMs();
    timeout->count = delay_ms;
}

void SysTick_ResetTimeOut (SYSTICK_TIMEOUT* timeout)
{
    timeout->start = SysTick_GetTickMs();
}

bool SysTick_IsTimeoutReached(SYSTICK_TIMEOUT* timeout)
{
    bool valTimeout  = true;
    if ((SysTick_GetTickMs() - timeout->start) < timeout->count)
    {
        valTimeout = false;
    }

    return valTimeout;

}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                  PRIVATE  FUNCTION IMPLEMENTATIONS                   */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static void SystickCallback(void)
{
    systick.tickCounter++;
}