///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                              ST7789.c                             ////
////                                                                   ////
////               ST7789 display driver for CCS C compiler            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////               This is a free software with NO WARRANTY.           ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

/**************************************************************************/
/*!
    @brief  Call before issuing command(s) or data to display. Performs
            chip-select (if required). Required
            for all display types; not an SPI-specific function.
*/
/**************************************************************************/

#include "ST7789.h"

    
uint16
  _width,     ///< Display width as modified by current rotation
  _height,    ///< Display height as modified by current rotation
  _xstart,    ///< Internal framebuffer X offset
  _ystart,    ///< Internal framebuffer Y offset
  _colstart,  ///< Some displays need this changed to offset
  _rowstart,  ///< Some displays need this changed to offset
  rotation;   ///< Display rotation (0 thru 3)  
  
void startWrite(void) {
  /*#ifdef TFT_CS
    output_low(TFT_CS);
  #endif*/
  
  P1_4 = 0;
  
  
}

/**************************************************************************/
/*!
    @brief  Call after issuing command(s) or data to display. Performs
            chip-deselect (if required). Required
            for all display types; not an SPI-specific function.
*/
/**************************************************************************/
void endWrite(void) {
  /*#ifdef TFT_CS
    output_high(TFT_CS);
  #endif*/
  
  P1_4 = 0;
}

/**************************************************************************/
/*!
    @brief  Write a single command byte to the display. Chip-select and
            transaction must have been previously set -- this ONLY sets
            the device to COMMAND mode, issues the byte and then restores
            DATA mode. There is no corresponding explicit writeData()
            function -- just use ST7789_SPI_XFER().
    @param  cmd  8-bit command to write.
*/
/**************************************************************************/
void writeCommand(uint8 cmd) {
  //output_low(TFT_DC);
  
  // Drive DC Low
  P1_2 = 0;
  
  //ST7789_SPI_XFER(cmd);
  U1DBUF = cmd;
      
  // Check if byte is transmitted.
  while(!(U1CSR & U0CSR_TX_BYTE));

  // Clear transmit byte status.
  U1CSR &= ~U0CSR_TX_BYTE;
  
  //output_high(TFT_DC);
  
  // Drive DC High
  P1_2 = 1;
}

/**************************************************************************/
/*!
    @brief  Companion code to the initiliazation tables. Reads and issues
            a series of LCD commands stored in ROM byte array.
    @param  addr  Flash memory array with commands and data to send
*/
/**************************************************************************/
void displayInit(uint16 *addr){
  uint16  numCommands, numArgs;
  uint16 ms;
  startWrite();

  numCommands = *addr++;   // Number of commands to follow
  
  while(numCommands--) {                 // For each command...

    writeCommand(*addr++); // Read, issue command
    numArgs  = *addr++;    // Number of args to follow
    ms       = numArgs & ST_CMD_DELAY;   // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;            // Mask out delay bit
    
    while(numArgs--) {                   // For each argument...
      //ST7789_SPI_XFER(*addr++);   // Read, issue argument
      
      // transmit byte
      U1DBUF = *addr++;
      
      // Check if byte is transmitted.
      while(!(U1CSR & U0CSR_TX_BYTE));

      // Clear transmit byte status.
      U1CSR &= ~U0CSR_TX_BYTE;

    }

    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      usleep(ms * 1000);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
    @brief  Initialization code for ST7789 display
*/
/**************************************************************************/
void tft_init(void) {
  /*#ifdef TFT_RST
    output_high(TFT_RST);
    output_drive(TFT_RST);
    delay_ms(100);
    output_low(TFT_RST);
    delay_ms(100);
    output_high(TFT_RST);
    delay_ms(200);
  #endif

  #ifdef TFT_CS
    output_high(TFT_CS);
    output_drive(TFT_CS);
  #endif

  output_drive(TFT_DC);*/
  
    // P1_1 - LED
    // P1_2 - RS
    // P1_3 - RST
    P1SEL = P1SEL & ~(BIT3 | BIT2 | BIT1);
    P1DIR |= (BIT3 | BIT2 | BIT1);
    
    // Reset LCD
    P1_3 = 1;
    usleep(100000);
    P1_3 = 0;
    usleep(100000);
    P1_3 = 1;
    usleep(200000);
    
    // High CS
    P1_4 = 1;
    
    // Turn Backlight ON
    P1_1 = 1;
    
    uint16
  cmd_240x240[] =  {                // Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      150,                          //    150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      255,                          //     255 = 500 ms delay
    ST77XX_COLMOD , 1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_XSTART,        //     XSTART = 0
      (240+ST7789_240x240_XSTART)>>8,
      (240+ST7789_240x240_XSTART)&0xFF,  //     XEND = 240
    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_YSTART,             //     YSTART = 0
      (240+ST7789_240x240_YSTART)>>8,
      (240+ST7789_240x240_YSTART)&0xFFFF,  //     YEND = 320
    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
      10,
    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
    255 };                          //     255 = max (500 ms) delay

  displayInit(cmd_240x240);

  _colstart = ST7789_240x240_XSTART;
  _rowstart = ST7789_240x240_YSTART;
  _height   = 240;
  _width    = 240;
  setRotation(0);
}

/**************************************************************************/
/*!
  @brief  SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner x coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
/**************************************************************************/
void setAddrWindow(uint16 x, uint16 y, uint16 w, uint16 h) {
  x += _xstart;
  y += _ystart;

  writeCommand(ST77XX_CASET); // Column addr set
  ST7789_SPI_XFER(x >> 8);
  ST7789_SPI_XFER(x);
  ST7789_SPI_XFER(x >> 8);
  ST7789_SPI_XFER(x+w-1);

  writeCommand(ST77XX_RASET); // Row addr set
  ST7789_SPI_XFER(y >> 8);
  ST7789_SPI_XFER(y);
  ST7789_SPI_XFER(y >> 8);
  ST7789_SPI_XFER(y+h-1);

  writeCommand(ST77XX_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void setRotation(uint8 m) {
  uint8 madctl = 0;

  rotation = m & 3; // can't be higher than 3

  switch (rotation) {
   case 0:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
     _xstart = _colstart;
     _ystart = _rowstart;
     break;
   case 1:
     madctl  = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = _rowstart;
     _ystart = _colstart;
     break;
  case 2:
     madctl  = ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
   case 3:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
  }
  startWrite();
  writeCommand(ST77XX_MADCTL);
  //ST7789_SPI_XFER(madctl);
  
  // transmit byte
  U1DBUF = madctl;
      
  // Check if byte is transmitted.
  while(!(U1CSR & U0CSR_TX_BYTE));

  // Clear transmit byte status.
  U1CSR &= ~U0CSR_TX_BYTE;
  
  
  endWrite();
}

void drawPixel(uint16 x, uint16 y, uint16 color) {
  if((x < _width) && (y < _height)) {
    startWrite();
    setAddrWindow(x, y, 10, 10);
    ST7789_SPI_XFER(color >> 8);
    ST7789_SPI_XFER(color & 0xFF);

    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawHLine(uint8 x, uint8 y, uint8 w, uint16 color) {
  if( (x < _width) && (y < _height) && w) {   
    uint8 hi = color >> 8, lo = color;

    if((x >= _width) || (y >= _height))
      return;
    if((x + w - 1) >= _width)  
      w = _width  - x;
    startWrite();
    setAddrWindow(x, y, w, 1);
    while (w--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawVLine(uint8 x, uint8 y, uint8 h, uint16 color) {
  if( (x < _width) && (y < _height) && h) {  
    uint8 hi = color >> 8, lo = color;
    if((y + h - 1) >= _height) 
      h = _height - y;
    startWrite();
    setAddrWindow(x, y, 1, h);
    while (h--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color) {
  if(w && h) {                            // Nonzero width and height?  
    uint8 hi = color >> 8, lo = color;
    if((x >= _width) || (y >= _height))
      return;
    if((x + w - 1) >= _width)  
      w = _width  - x;
    if((y + h - 1) >= _height) 
      h = _height - y;
    startWrite();
    setAddrWindow(x, y, w, h);
    uint16 px = (uint16)w * h;
    while (px--) {
      ST7789_SPI_XFER(hi);
      ST7789_SPI_XFER(lo);
    }
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one color. Update in subclasses if desired!
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillScreen(uint16 color) {
    fillRect(0, 0, _width, _height, color);
}

/**************************************************************************/
/*!
    @brief  Invert the colors of the display (if supported by hardware).
            Self-contained, no transaction setup required.
    @param  i  true = inverted display, false = normal display.
*/
/**************************************************************************/
void invertDisplay(bool i) {
    startWrite();
    writeCommand(i ? ST77XX_INVON : ST77XX_INVOFF);
    endWrite();
}

/*!
    @brief  Essentially writePixel() with a transaction around it. I don't
            think this is in use by any of our code anymore (believe it was
            for some older BMP-reading examples), but is kept here in case
            any user code relies on it. Consider it DEPRECATED.
    @param  color  16-bit pixel color in '565' RGB format.
*/
void pushColor(uint16 color) {
    uint8 hi = color >> 8, lo = color;
    startWrite();
    ST7789_SPI_XFER(hi);
    ST7789_SPI_XFER(lo);
    endWrite();
}

void ST7789_SPI_XFER(uint8 byte)
{
    U1DBUF = byte;
      
    // Check if byte is transmitted.
    while(!(U1CSR & U0CSR_TX_BYTE));

    // Clear transmit byte status.
    U1CSR &= ~U0CSR_TX_BYTE;
}

void usleep(uint32 us)
{
	  // Delay for ms
      for(int i = 0; i < us; i++)
      {
          /* 32 NOPs == 1 usecs */
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); 
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
      asm("nop"); asm("nop");
      }
}

// end of code.
