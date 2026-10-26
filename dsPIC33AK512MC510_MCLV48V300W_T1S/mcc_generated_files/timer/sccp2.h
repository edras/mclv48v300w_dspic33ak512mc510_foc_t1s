/**
 * SCCP2-TIMER Generated Driver Header File 
 * 
 * @file      sccp2.h
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver header file for the SCCP2-TIMER driver
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

#ifndef SCCP2_H
#define SCCP2_H

// Section: Included Files

#include <stddef.h>
#include <stdint.h>
#include <xc.h>
#include "timer_interface.h"

// Section: Data Type Definitions


/**
 * @ingroup  timerdriver
 * @brief    Structure object of type TIMER_INTERFACE with the custom name given by 
 *           the user in the Melody Driver User interface. The default name 
 *           e.g. Timer1 can be changed by the user in the TIMER user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct TIMER_INTERFACE AI_Timer;

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Initialize API
 */
#define AI_Timer_Initialize SCCP2_Timer_Initialize
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Deinitialize API
 */
#define AI_Timer_Deinitialize SCCP2_Timer_Deinitialize
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Tasks API
 */
#define AI_Timer_Tasks SCCP2_Timer_Tasks
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Start API
 */
#define AI_Timer_Start SCCP2_Timer_Start
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Stop API
 */
#define AI_Timer_Stop SCCP2_Timer_Stop

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_PeriodSet API
 */
#define AI_Timer_PeriodSet SCCP2_Timer_PeriodSet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_PeriodGet API
 */
#define AI_Timer_PeriodGet SCCP2_Timer_PeriodGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_CounterGet API
 */
#define AI_Timer_CounterGet SCCP2_Timer_CounterGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_Counter16BitGet API
 */
#define AI_Timer_Counter16BitGet SCCP2_Timer_Counter16BitGet
/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_InterruptPrioritySet API
 */
#define AI_Timer_InterruptPrioritySet SCCP2_Timer_InterruptPrioritySet

/**
 * @ingroup  timerdriver
 * @brief    This macro defines the Custom Name for \ref SCCP2_Timer_TimeoutCallbackRegister API
 */
#define AI_Timer_TimeoutCallbackRegister SCCP2_Timer_TimeoutCallbackRegister

// Section: Driver Interface Functions

/**
 * @ingroup  timerdriver
 * @brief    Initializes the SCCP2 module 
 * @return   none  
 */
void SCCP2_Timer_Initialize (void);

/**
 * @ingroup  timerdriver
 * @brief    Deinitializes the SCCP2 to POR values
 * @return   none  
 */
void SCCP2_Timer_Deinitialize(void);

/**
 * @ingroup  timerdriver
 * @brief    Starts the timer
 * @pre      \ref SCCP2_Timer_Initialize must be called
 * @return   none  
 */
void SCCP2_Timer_Start(void);

/**
 * @ingroup  timerdriver
 * @brief    Stops the timer
 * @pre      \ref SCCP2_Timer_Initialize must be called
 * @return   none  
 */
void SCCP2_Timer_Stop(void);

/**
 * @ingroup  timerdriver
 * @brief    Sets the SCCP2-Timer period count value
 * @pre      \ref SCCP2_Timer_Initialize must be called
 * @param[in]  count - period value
 * @return   none  
 */
void SCCP2_Timer_PeriodSet(uint32_t count);

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the SCCP2-Timer period count value
 * @pre      \ref SCCP2_Timer_Initialize must be called
 * @return   Period count value  
 */
inline static uint32_t SCCP2_Timer_PeriodGet(void)
{
        return CCP2PR;
}

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the SCCP2-Timer elapsed count value
 * @return   Elapsed count value of the timer  
 */
inline static uint32_t SCCP2_Timer_CounterGet(void)
{
    return (uint32_t)CCP2TMR;
}

/**
 * @ingroup  timerdriver
 * @brief    This inline function gets the SCCP2-Timer least significant 16 bit elapsed count value
 * @return   Least significant 16 bit elapsed count value of the timer  
 */
inline static uint16_t SCCP2_Timer_Counter16BitGet(void)
{
    return (uint16_t)CCP2TMR;
}

/**
 * @ingroup  timerdriver
 * @brief    Sets the Interrupt Priority Value 
 * @return   none  
 */
void SCCP2_Timer_InterruptPrioritySet(enum INTERRUPT_PRIORITY priority);
/**
 * @ingroup  timerdriver
 * @brief    Used in polling method of timeout event 
 * @pre      \ref SCCP2_Timer_Initialize must be called 
 * @return   none  
 */
void SCCP2_Timer_Tasks(void);


/**
 * @ingroup    timerdriver
 * @brief      This function can be used to override default callback and to define 
 *             custom callback for SCCP2 Timeout event.
 * @param[in]  handler - Address of the callback function.  
 * @return     none 
 */
void SCCP2_Timer_TimeoutCallbackRegister(void (*handler)(void));

/**
 * @ingroup  timerdriver
 * @brief    This is the default callback with weak attribute. The user can 
 *           override and implement the default callback without weak attribute 
 *           or can register a custom callback function using  \ref SCCP2_Timer_TimeoutCallbackRegister.
 * @return   none  
 */
void SCCP2_TimeoutCallback(void);

#endif //SCCP2_H

/**
 End of File
*/


