#include "i2c_device.h"




bool i2cDeviceInit(void)
{
  return true;
}



void i2cReadCallback(uint8_t addr, uint8_t *p_data)
{
  switch(addr)
  {
    case 0x00:
      break;
    
    default:
      *p_data = 0x00;
      break;
  }
}

void i2cWriteCallback(uint8_t addr, uint8_t *p_data)
{
  switch(addr)
  {
    case 0x00:
      gpioPinWrite(_PIN_GPIO_LCD_BL, p_data[0] > 0 ? 1:0);
      break;
    
    default:
      *p_data = 0x00;
      break;
  }
}
