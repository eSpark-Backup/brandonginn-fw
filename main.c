#define TEMP_AMBIENT 0
#define TEMP_OBJ     1

#include "MLX90614.h"


int main( void )
{
  float temp;
  
  init_sensor();
  temp = get_temp(TEMP_OBJ);
  
  return 0;
}
