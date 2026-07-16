/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
 *
 * @skipline @version   PLIB Version 1.0.2
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

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

// Section: Device Pin Macros

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA12 GPIO Pin which has a custom name of SW1 to High
 * @pre      The RA12 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SW1_SetHigh()          (_LATA12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA12 GPIO Pin which has a custom name of SW1 to Low
 * @pre      The RA12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW1_SetLow()           (_LATA12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA12 GPIO Pin which has a custom name of SW1
 * @pre      The RA12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW1_Toggle()           (_LATA12 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA12 GPIO Pin which has a custom name of SW1
 * @param    none
 * @return   none  
 */
#define SW1_GetValue()         _RA12

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA12 GPIO Pin which has a custom name of SW1 as Input
 * @param    none
 * @return   none  
 */
#define SW1_SetDigitalInput()  (_TRISA12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA12 GPIO Pin which has a custom name of SW1 as Output
 * @param    none
 * @return   none  
 */
#define SW1_SetDigitalOutput() (_TRISA12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA14 GPIO Pin which has a custom name of XPRO1_CS to High
 * @pre      The RA14 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_SetHigh()          (_LATA14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA14 GPIO Pin which has a custom name of XPRO1_CS to Low
 * @pre      The RA14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_SetLow()           (_LATA14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA14 GPIO Pin which has a custom name of XPRO1_CS
 * @pre      The RA14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_Toggle()           (_LATA14 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA14 GPIO Pin which has a custom name of XPRO1_CS
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_GetValue()         _RA14

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA14 GPIO Pin which has a custom name of XPRO1_CS as Input
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_SetDigitalInput()  (_TRISA14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA14 GPIO Pin which has a custom name of XPRO1_CS as Output
 * @param    none
 * @return   none  
 */
#define XPRO1_CS_SetDigitalOutput() (_TRISA14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB3 GPIO Pin which has a custom name of XPRO1_INT to High
 * @pre      The RB3 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_SetHigh()          (_LATB3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB3 GPIO Pin which has a custom name of XPRO1_INT to Low
 * @pre      The RB3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_SetLow()           (_LATB3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB3 GPIO Pin which has a custom name of XPRO1_INT
 * @pre      The RB3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_Toggle()           (_LATB3 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB3 GPIO Pin which has a custom name of XPRO1_INT
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_GetValue()         _RB3

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB3 GPIO Pin which has a custom name of XPRO1_INT as Input
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_SetDigitalInput()  (_TRISB3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB3 GPIO Pin which has a custom name of XPRO1_INT as Output
 * @param    none
 * @return   none  
 */
#define XPRO1_INT_SetDigitalOutput() (_TRISB3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD4 GPIO Pin which has a custom name of HALL_A to High
 * @pre      The RD4 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define HALL_A_SetHigh()          (_LATD4 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD4 GPIO Pin which has a custom name of HALL_A to Low
 * @pre      The RD4 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_A_SetLow()           (_LATD4 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD4 GPIO Pin which has a custom name of HALL_A
 * @pre      The RD4 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_A_Toggle()           (_LATD4 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD4 GPIO Pin which has a custom name of HALL_A
 * @param    none
 * @return   none  
 */
#define HALL_A_GetValue()         _RD4

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD4 GPIO Pin which has a custom name of HALL_A as Input
 * @param    none
 * @return   none  
 */
#define HALL_A_SetDigitalInput()  (_TRISD4 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD4 GPIO Pin which has a custom name of HALL_A as Output
 * @param    none
 * @return   none  
 */
#define HALL_A_SetDigitalOutput() (_TRISD4 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of HALL_B to High
 * @pre      The RD5 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define HALL_B_SetHigh()          (_LATD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of HALL_B to Low
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_B_SetLow()           (_LATD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD5 GPIO Pin which has a custom name of HALL_B
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_B_Toggle()           (_LATD5 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD5 GPIO Pin which has a custom name of HALL_B
 * @param    none
 * @return   none  
 */
#define HALL_B_GetValue()         _RD5

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of HALL_B as Input
 * @param    none
 * @return   none  
 */
#define HALL_B_SetDigitalInput()  (_TRISD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of HALL_B as Output
 * @param    none
 * @return   none  
 */
#define HALL_B_SetDigitalOutput() (_TRISD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD6 GPIO Pin which has a custom name of HALL_C to High
 * @pre      The RD6 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define HALL_C_SetHigh()          (_LATD6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD6 GPIO Pin which has a custom name of HALL_C to Low
 * @pre      The RD6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_C_SetLow()           (_LATD6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD6 GPIO Pin which has a custom name of HALL_C
 * @pre      The RD6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define HALL_C_Toggle()           (_LATD6 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD6 GPIO Pin which has a custom name of HALL_C
 * @param    none
 * @return   none  
 */
#define HALL_C_GetValue()         _RD6

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD6 GPIO Pin which has a custom name of HALL_C as Input
 * @param    none
 * @return   none  
 */
#define HALL_C_SetDigitalInput()  (_TRISD6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD6 GPIO Pin which has a custom name of HALL_C as Output
 * @param    none
 * @return   none  
 */
#define HALL_C_SetDigitalOutput() (_TRISD6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE1 GPIO Pin which has a custom name of SW2 to High
 * @pre      The RE1 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SW2_SetHigh()          (_LATE1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE1 GPIO Pin which has a custom name of SW2 to Low
 * @pre      The RE1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW2_SetLow()           (_LATE1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE1 GPIO Pin which has a custom name of SW2
 * @pre      The RE1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW2_Toggle()           (_LATE1 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE1 GPIO Pin which has a custom name of SW2
 * @param    none
 * @return   none  
 */
#define SW2_GetValue()         _RE1

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE1 GPIO Pin which has a custom name of SW2 as Input
 * @param    none
 * @return   none  
 */
#define SW2_SetDigitalInput()  (_TRISE1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE1 GPIO Pin which has a custom name of SW2 as Output
 * @param    none
 * @return   none  
 */
#define SW2_SetDigitalOutput() (_TRISE1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE2 GPIO Pin which has a custom name of LED1 to High
 * @pre      The RE2 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED1_SetHigh()          (_LATE2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE2 GPIO Pin which has a custom name of LED1 to Low
 * @pre      The RE2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED1_SetLow()           (_LATE2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE2 GPIO Pin which has a custom name of LED1
 * @pre      The RE2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED1_Toggle()           (_LATE2 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE2 GPIO Pin which has a custom name of LED1
 * @param    none
 * @return   none  
 */
#define LED1_GetValue()         _RE2

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE2 GPIO Pin which has a custom name of LED1 as Input
 * @param    none
 * @return   none  
 */
#define LED1_SetDigitalInput()  (_TRISE2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE2 GPIO Pin which has a custom name of LED1 as Output
 * @param    none
 * @return   none  
 */
#define LED1_SetDigitalOutput() (_TRISE2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE3 GPIO Pin which has a custom name of LED2 to High
 * @pre      The RE3 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED2_SetHigh()          (_LATE3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE3 GPIO Pin which has a custom name of LED2 to Low
 * @pre      The RE3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED2_SetLow()           (_LATE3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE3 GPIO Pin which has a custom name of LED2
 * @pre      The RE3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED2_Toggle()           (_LATE3 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE3 GPIO Pin which has a custom name of LED2
 * @param    none
 * @return   none  
 */
#define LED2_GetValue()         _RE3

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE3 GPIO Pin which has a custom name of LED2 as Input
 * @param    none
 * @return   none  
 */
#define LED2_SetDigitalInput()  (_TRISE3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE3 GPIO Pin which has a custom name of LED2 as Output
 * @param    none
 * @return   none  
 */
#define LED2_SetDigitalOutput() (_TRISE3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RG0 GPIO Pin which has a custom name of XPRO1_RST to High
 * @pre      The RG0 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_SetHigh()          (_LATG0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RG0 GPIO Pin which has a custom name of XPRO1_RST to Low
 * @pre      The RG0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_SetLow()           (_LATG0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RG0 GPIO Pin which has a custom name of XPRO1_RST
 * @pre      The RG0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_Toggle()           (_LATG0 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RG0 GPIO Pin which has a custom name of XPRO1_RST
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_GetValue()         _RG0

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RG0 GPIO Pin which has a custom name of XPRO1_RST as Input
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_SetDigitalInput()  (_TRISG0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RG0 GPIO Pin which has a custom name of XPRO1_RST as Output
 * @param    none
 * @return   none  
 */
#define XPRO1_RST_SetDigitalOutput() (_TRISG0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);

/**
 * @ingroup  pinsdriver
 * @brief    This function is callback for XPRO1_INT Pin
 * @param    none
 * @return   none   
 */
void XPRO1_INT_CallBack(void);


/**
 * @ingroup    pinsdriver
 * @brief      This function assigns a function pointer with a callback address
 * @param[in]  InterruptHandler - Address of the callback function 
 * @return     none  
 */
void XPRO1_INT_SetInterruptHandler(void (* InterruptHandler)(void));


#endif
