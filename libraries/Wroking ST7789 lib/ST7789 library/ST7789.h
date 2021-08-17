#ifndef ST7789
#define ST7789 1

//#include <stdint.h>
#include "hal_types.h"
#ifndef bool
#define bool uint8
#endif

#define chip 2541


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
   

#define ST_CMD_DELAY      0x80    // special signifier for command lists

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_PTLAR      0x30
#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1      0xDA
#define ST77XX_RDID2      0xDB
#define ST77XX_RDID3      0xDC
#define ST77XX_RDID4      0xDD

// Some ready-made 16-bit ('565') color settings:
#define   ST7789_BLACK   0x0000
#define   ST7789_BLUE    0x001F
#define   ST7789_RED     0xF800
#define   ST7789_GREEN   0x07E0
#define   ST7789_CYAN    0x07FF
#define   ST7789_MAGENTA 0xF81F
#define   ST7789_YELLOW  0xFFE0
#define   ST7789_WHITE   0xFFFF

/*#ifndef ST7789_SPI_XFER
#define ST7789_SPI_XFER(x) SPI_XFER(ST7789, x)
#endif*/


// SCREEN INITIALIZATION ***************************************************

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 0

//*************************** User Functions ***************************//
void tft_init(void);

void drawPixel(uint16 x, uint16 y, uint16 color);
void drawHLine(uint8 x, uint8 y, uint8 w, uint16 color);
void drawVLine(uint8 x, uint8 y, uint8 h, uint16 color);
void fillRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color);
void fillScreen(uint16 color);
void setRotation(uint8 m);
void invertDisplay(bool i);
void pushColor(uint16 color);

//************************* Non User Functions *************************//
void startWrite(void);
void endWrite(void);
void displayInit(uint16 *addr);
void writeCommand(uint8 cmd);
void setAddrWindow(uint16 x, uint16 y, uint16 w, uint16 h);
void ST7789_SPI_XFER(uint8 byte);
void usleep(uint32 us);

#endif