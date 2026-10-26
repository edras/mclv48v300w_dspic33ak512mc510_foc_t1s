/**
 * SCCP2-TIMER Generated Driver Source File
 * 
 * @file      sccp2.c
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver source file for SCCP2-TIMER driver
 *
 * @version   PLIB Version 1.2.2
 *
 * @skipline  Device : dsPIC33AK512MC510
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

// Section: Included Files

#include <stddef.h> 
#include "../sccp2.h"
#include "../timer_interface.h"

// Section: File specific functions

static void (*SCCP2_TimeoutHandler)(void) = NULL;

// Section: Driver Interface

// Defines an object for TIMER_INTERFACE

const struct TIMER_INTERFACE AI_Timer = {
    .Initialize     = &SCCP2_Timer_Initialize,
    .Deinitialize   = &SCCP2_Timer_Deinitialize,
    .Start          = &SCCP2_Timer_Start,
    .Stop           = &SCCP2_Timer_Stop,
    .PeriodSet      = &SCCP2_Timer_PeriodSet,
    .PeriodGet	    = &SCCP2_Timer_PeriodGet,
    .CounterGet     = &SCCP2_Timer_CounterGet,
    .InterruptPrioritySet = &SCCP2_Timer_InterruptPrioritySet,
    .TimeoutCallbackRegister = &SCCP2_Timer_TimeoutCallbackRegister,
    .Tasks          = &SCCP2_Timer_Tasks,
};

// Section: Driver Interface Function Definitions

void SCCP2_Timer_Initialize(void)
{
    //MOD 16-Bit/32-Bit Timer; CCSEL disabled; T32 32 Bit; TMRPS 1:1; CLKSEL Clock Generator 13; TMRSYNC disabled; SIDL disabled; ON disabled; SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; RTRGEN disabled; OPSSRC Timer Interrupt Event; 
    CCP2CON1 = 0x120UL;
    //ASDG disabled; SSDG disabled; ASDGM disabled; PWMRSEN disabled; ICS ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; 
    CCP2CON2 = 0x0UL;
    //PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; PSSBDF Tri-state; POLBDF disabled; 
    CCP2CON3 = 0x0UL;
    //ICOV disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; RAWIP disabled; RBWIP disabled; TMRLWIP disabled; TMRHWIP disabled; PRLWIP disabled; 
    CCP2STAT = 0x0UL;
    //TMRL 0x0; TMRH 0x0; 
    CCP2TMR = 0x0UL;
    //PRL 38527; PRH 152; 
    CCP2PR = 0x98967FUL;
    //BUFL 0x0; BUFH 0x0; 
    CCP2BUF = 0x0UL;
    //CMPA 0x0; 
    CCP2RA = 0x0UL;
    //CMPB 0x0; 
    CCP2RB = 0x0UL;
    
    SCCP2_Timer_TimeoutCallbackRegister(&SCCP2_TimeoutCallback);


    CCP2CON1bits.ON = 1; //Enable Module
}

void SCCP2_Timer_Deinitialize(void)
{
    CCP2CON1bits.ON = 0;
    
    CCP2CON1 = 0x0UL; 
    CCP2CON2 = 0x1000000UL; 
    CCP2CON3 = 0x0UL; 
    CCP2STAT = 0x0UL; 
    CCP2TMR = 0x0UL; 
    CCP2PR = 0xFFFFFFFFUL; 
    CCP2BUF = 0x0UL; 
    CCP2RA = 0x0UL; 
    CCP2RB = 0x0UL; 
}

void SCCP2_Timer_Start(void)
{
    CCP2CON1bits.ON = 1;
}

void SCCP2_Timer_Stop(void)
{
    CCP2CON1bits.ON = 0;
}

void SCCP2_Timer_PeriodSet(uint32_t count)
{
    if(count > 0xFFFFU)
    {
        CCP2PR = count;
        CCP2CON1bits.T32 = 1;
    }
    else
    {
        CCP2PR = count;
        CCP2CON1bits.T32 = 0;
    }
}

void SCCP2_Timer_InterruptPrioritySet(enum INTERRUPT_PRIORITY priority)
{
    IPC6bits.CCT2IP = priority;
}

void SCCP2_Timer_TimeoutCallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP2_TimeoutHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP2_TimeoutCallback (void)
{ 

} 

void SCCP2_Timer_Tasks(void)
{
    if(IFS1bits.CCT2IF == 1)
    {
        if(NULL != SCCP2_TimeoutHandler)
        {
            (*SCCP2_TimeoutHandler)();
        }
        IFS1bits.CCT2IF = 0;
    }
}

/**
 End of File
*/
