/******************************************************************************/
/** T1S HAL HEADER                                                             */
/*******************************************************************************
 T1S

  Company:
    Microchip Technology Inc.

  File Name:
    t1s_lwip_hal.h

  Summary:
    T1S module "click" board support - header file interface
 definition

  Description:
    Generic support for the T1S Lightweight IP (LWIP) module
 ******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "hal.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/i2c_host/i2c3.h"
#include "systick/systick.h"
#include "dmaspi.h"

#ifndef _T1s_LWIP_HAL_H   
#define _T1s_LWIP_HAL_H

#ifdef MIKROE_T1S

#define T1S_BOARD_INSTANCE   0
#define T1S_AVAILABLE        true

#define T1S_RST_Clear        XPRO1_RST_SetLow
#define T1S_RST_Set          XPRO1_RST_SetHigh
#define T1S_CS_Clear         XPRO1_CS_SetLow
#define T1S_CS_Set           XPRO1_CS_SetHigh

#define T1S_INT_GetValue     XPRO1_INT_GetValue

#define T1S_TERMINAL_Write(...)   printf(__VA_ARGS__)
#define T1S_TERMINAL_Done(...)    true

#define T1S_DelayMs          SysTick_DelayMs
#define T1S_GetTickCountMs   SysTick_GetTickMs  
#define T1S_GetTickCountUs   SysTick_GetTickUs

// SPI Definitions
#define T1S_SPI_Init         dmaSpiInit
#define T1S_SPI_WriteRead    dmaSpiStartTransaction
#define T1S_SPI_IsBusy       dmaSpiGetStatus
#define T1S_SPI_AVAILABLE    DMA_AVAILABLE

// I2C Definitions
#define T1S_I2C_WriteRead    I2C3_WriteRead
#define T1S_I2C_IsBusy       I2C3_IsBusy
#define T1S_I2C_ErrorGet     I2C3_ErrorGet

#define T1S_reset(...)        __asm__ volatile ("reset")

void T1S_EIC_CB_Register(void (* callback)(void));

#else

#define T1S_AVAILABLE        false
#define T1S_available(...)   false
#define T1S_init(...)        false

#define T1S_I2C_WriteRead(...)   0
#define T1S_I2C_IsBusy(...)  0

#endif

#endif /* _T1s_LWIP_HAL_H */

/* *****************************************************************************
 End of File
 */
