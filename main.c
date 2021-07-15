///////////////////////////////////// Include HAL Files /////////////////////////////////////
#include "hal_types.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_led.h"
#include "hal_uart.h"

halUARTCfg_t UARTCfg;

int main(void)
{
  UARTCfg.baudRate = HAL_UART_BR_115200;
  HalLedInit();
  //HalUARTIsrDMA();
  
  // Init Serial
  //HalUARTInit();

  // Configure for BaudRate 115200
  //HalUARTOpen(HAL_UART_PORT_1, &UARTCfg);
  
  return 0;
}