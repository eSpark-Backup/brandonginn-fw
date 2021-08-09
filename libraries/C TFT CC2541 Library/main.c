
/***********************************************************************************
  Filename:     spi_master_send.c

  Description:  This example uses a master to send data to a slave using SPI.

  Comments:     To execute this example, use the IAR project spi_slave_receive
                as the slave. The slave's code must be executed before executing
                the master's code, since the slave is clocked by the Master. The
                bytes sent are simply numbers 0x00 upto BUFFER_SIZE. Note that
                if BUFFER_SIZE is larger than 0xFF, then the element 0xFF in the
                array will have the value 0x00 and so on. When bytes up to
                BUFFER_SIZE are sent, the example will end and LED1 will be set
                on SmartRF05EB.

Configuration for each module:

CC2541EM, CC2543EM, CC2545EM: 
                Connect from each board:
                 - MISO:  P0_2  (PIN9 on Debug Connector P18)
                 - MOSI:  P0_3  (PIN11 on Debug Connector P18)
                 - SSN:   P0_4  (PIN13 on Debug Connector P18)
                 - SCK:   P0_5  (PIN15 on Debug Connector P18)
                 - GND:         (PIN20 on Debug Connector P18)

CC2544Dongle:
                No configuration for Dongle in this example. 
                The code illsutrate how to set up the interface for CC2544.
                There is not a CC2544 HW plattform available for running 
                this excact code.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include "spi_lcd.h"
                  
#define chip 2541
#define LCD LCD_ST7789


#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2544)
#include "ioCC2544.h"
#warning "The CC2544Dongle is not supported for this software example."
#warning "This code illustrate how to set up the SPI interface on the CC2544 device."
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/***********************************************************************************
* CONSTANTS
*/

unsigned short usColors[8] = {0xf800, 0x7e0, 0x1f, 0xffff, 0xffe0, 0x7ff, 0xf81f, 0x7bef};

// Define size of buffer and number of bytes to send
#define BUFFER_SIZE 1
/***********************************************************************************
* LOCAL VARIABLES
*/


/***********************************************************************************
* LOCAL FUNCTIONS
*/


/***********************************************************************************
* @fn          main
*
* @brief       Send data to a single slave using SPI in Master mode
*
* @param       void
*
* @return      void
*/
void main(void)
{
  
      // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    while (CLKCONSTA & CLKCON_OSC);   // Wait until clock source has changed
  
    int rc;
    rc = spilcdInit(LCD, 0, 0, 2000000, 0, 0, 0); // LCD type, flip 180, SPI Channel, D/C, RST, LED
    
    spilcdFill(usColors[2]);
    
    // end function with infinite loop (for debugging purposes). 
    while(1);
}


/***********************************************************************************
  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/