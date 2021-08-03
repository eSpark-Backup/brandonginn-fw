#include "hal_i2c.h"

  static const int ram_access = 0x00;
  static const int T_obj = 0x07;
     char cmd[3] = { 0 };
 float tempC = 0.0;
  uint8 status = 0;
i2cClock_t clock = i2cClock_33KHZ;

int main( void )
{
  //HalI2CDisable();
  uint8 cmd[3] = {0};
  HalI2CInit(0x5A, clock);
    //i2c_->write(default_addr,cmd,1,true); i2c_->read(default_addr,cmd,3);
   
    
    
    // convert to meaningful units, still in kelvin - just normalized
    //return 0.02 * static_cast<float>((cmd[1]<<8)|cmd[0]);
    
    
    while(1)
    {
    uint8 reg = T_obj;
    // read the temperature data (kelvin)
    cmd[0] = ram_access | reg;
    status = HalI2CWrite(1, (uint8 *)cmd, FALSE);
    status = HalI2CRead(3, (uint8 *)cmd);
    
    //status = HalI2CWriteRead(1, (uint8 *)cmd, 3, (uint8 *)cmd);
    
    tempC = 0.02 * ((float)(((uint16)cmd[1]<<8)|((uint16)cmd[0]))) -273.15;
    }
  return 0;
}
