#define SPI_BAUD_M  0
#define SPI_BAUD_E  16

#include "ST7789.h"       // include ST7789 display driver

void initSysClock();
void initSPI();


int main( void )
{
    initSysClock();
    
    initSPI();
    
    tft_init();
    
    //invertDisplay(1);
   
    fillScreen(ST7789_RED);
    
    /*drawPixel(150, 240, ST7789_GREEN);
    drawPixel(150, 241, ST7789_GREEN);
    drawPixel(150, 242, ST7789_GREEN);
    drawPixel(150, 243, ST7789_GREEN);
    drawPixel(150, 244, ST7789_GREEN);
    drawPixel(150, 245, ST7789_GREEN);
    drawPixel(150, 246, ST7789_GREEN);
    drawPixel(150, 247, ST7789_GREEN);
    drawPixel(150, 248, ST7789_GREEN);
    drawPixel(150, 249, ST7789_GREEN);
    
    drawPixel(150, 240, ST7789_GREEN);
    drawPixel(151, 240, ST7789_GREEN);
    drawPixel(152, 240, ST7789_GREEN);
    drawPixel(153, 240, ST7789_GREEN);
    drawPixel(154, 240, ST7789_GREEN);
    
    fillRect(50,240, 40, 40, ST7789_GREEN);*/
  
  return 0;
}
 
void initSysClock()
{
        // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    while (CLKCONSTA & CLKCON_OSC);   // Wait until clock source has changed
}

void initSPI()
{
    // Configure USART1 for Alternative 1 => Port P1
    PERCFG = (PERCFG | 0x02);
    
    // Set pins 2, 3 and 5 as peripheral I/O and pin 4 as GPIO output.
    P1SEL = (P1SEL & ~BIT4) | BIT5 | BIT6 | BIT7;
    P1DIR |= BIT4;
    
        /***************************************************************************
     * Configure SPI
     */

    // Set USART to SPI mode and Master mode.
    uint8 config = 0;
    config = U1MODE;
    config |= U1SLAVE;
    U1CSR &= ~config;

    // Set:
    // - mantissa value
    // - exponent value
    // - clock phase to be centered on first edge of SCK period
    // - negative clock polarity (SCK low when idle)
    // - bit order for transfers to MSB first
    // - 2MHz Clock Rate
    U1BAUD = SPI_BAUD_M;
    U1GCR = ((U1GCR | U0GCR_ORDER | U0GCR_CPOL) & ~(U0GCR_BAUD_E  | U0GCR_CPHA))
        | SPI_BAUD_E;
    
}
