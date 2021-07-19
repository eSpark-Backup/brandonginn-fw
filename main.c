#include "hal_types.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_led.h"
#include "hal_uart.h"

#include "hal_i2c.h"


halUARTCfg_t UARTCfg;
i2cClock_t clock_t = i2cClock_123KHZ;


int main( void )
{
  uint8 buf[3] = {'a', 'a', 'a'};
  UARTCfg.baudRate = HAL_UART_BR_115200;
  
  // Init Serial
  HalUARTInit();

  // Configure for BaudRate 115200
  HalUARTOpen(HAL_UART_PORT_1, &UARTCfg);
  
  HalI2CInit(0x42, clock_t);
  
  HalI2CWrite(3, buf);


  
  
  return 0;
}
