/**
 * PINS Generated Driver Source File 
 * 
 * @file      pins.c
 *            
 * @ingroup   pinsdriver
 *            
 * @brief     This is the generated driver source file for PINS driver.
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

// Section: Includes
#include <xc.h>
#include <stddef.h>
#include "../pins.h"

// Section: File specific functions
static void (*XPRO1_INT_InterruptHandler)(void) = NULL;

/**
 * @ingroup  pinsdriver
 * @brief    Locks all the Peripheral Remapping registers and cannot be written.
 * @return   none  
 */
#define PINS_PPSLock()           (RPCONbits.IOLOCK = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Unlocks all the Peripheral Remapping registers and can be written.
 * @return   none  
 */
#define PINS_PPSUnlock()         (RPCONbits.IOLOCK = 0)

// Section: Driver Interface Function Definitions
void PINS_Initialize(void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000UL;
    LATB = 0x0000UL;
    LATC = 0x8000UL;
    LATD = 0x0000UL;
    LATE = 0x0000UL;
    LATF = 0x0000UL;
    LATG = 0x0000UL;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x3FDBUL;
    TRISB = 0xFFFEUL;
    TRISC = 0x7FE7UL;
    TRISD = 0xFFF0UL;
    TRISE = 0x0013UL;
    TRISF = 0x000FUL;
    TRISG = 0x0006UL;


    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPUA = 0x0000UL;
    CNPUB = 0x0000UL;
    CNPUC = 0x0000UL;
    CNPUD = 0x0000UL;
    CNPUE = 0x0000UL;
    CNPUF = 0x0000UL;
    CNPUG = 0x0000UL;
    CNPDA = 0x0000UL;
    CNPDB = 0x0000UL;
    CNPDC = 0x0000UL;
    CNPDD = 0x0000UL;
    CNPDE = 0x0000UL;
    CNPDF = 0x0000UL;
    CNPDG = 0x0000UL;


    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000UL;
    ODCB = 0x0000UL;
    ODCC = 0x0000UL;
    ODCD = 0x0000UL;
    ODCE = 0x0000UL;
    ODCF = 0x0000UL;
    ODCG = 0x0000UL;


    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x0FFFUL;
    ANSELB = 0xFFF7UL;
    ANSELC = 0x3F27UL;
    ANSELD = 0xFF80UL;
    ANSELE = 0x0001UL;
    ANSELF = 0x0001UL;

    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
      PINS_PPSUnlock(); // unlock PPS

        RPINR13bits.U1RXR = 0x002FUL; //RC14->UART1:U1RX;
        RPINR14bits.SDI1R = 0x0045UL; //RE4->SPI1:SDI1;
        RPOR11bits.RP48R = 0x0013UL;  //RC15->UART1:U1TX;
        RPOR3bits.RP16R = 0x0019UL;  //RA15->SPI1:SDO1;
        RPINR14bits.SCK1R = 0x000EUL;  //RA13->SPI1:SCK1IN;
        RPOR3bits.RP14R = 0x001AUL;  //RA13->SPI1:SCK1OUT;

      PINS_PPSLock(); // lock PPS

    /*******************************************************************************
    * Interrupt On Change: negative
    *******************************************************************************/
    CNEN1Bbits.CNEN1B3 = 1; //Pin : RB3; 

    /****************************************************************************
     * Interrupt On Change: flag
     ***************************************************************************/
    CNFBbits.CNFB3 = 0;    //Pin : XPRO1_INT

    /****************************************************************************
     * Interrupt On Change: config
     ***************************************************************************/
    CNCONBbits.CNSTYLE = 1; //Config for PORTB
    CNCONBbits.ON = 1; //Config for PORTB

    /* Initialize IOC Interrupt Handler*/
    XPRO1_INT_SetInterruptHandler(&XPRO1_INT_CallBack);

    /****************************************************************************
     * Interrupt On Change: Interrupt Enable
     ***************************************************************************/
    IFS3bits.CNBIF = 0; //Clear CNBI interrupt flag
    IEC3bits.CNBIE = 1; //Enable CNBI interrupt
}

void __attribute__ ((weak)) XPRO1_INT_CallBack(void)
{

}

void XPRO1_INT_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC3bits.CNBIE = 0; //Disable CNBI interrupt
    XPRO1_INT_InterruptHandler = InterruptHandler; 
    IEC3bits.CNBIE = 1; //Enable CNBI interrupt
}

/* Interrupt service function for the CNBI interrupt. */
/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ (( interrupt, no_auto_psv )) _CNBInterrupt (void)
{
    if(CNFBbits.CNFB3 == 1)
    {
        if(XPRO1_INT_InterruptHandler != NULL) 
        { 
            XPRO1_INT_InterruptHandler(); 
        }
        
        CNFBbits.CNFB3 = 0;  //Clear flag for Pin - XPRO1_INT
    }
    
    // Clear the flag
    IFS3bits.CNBIF = 0;
}

