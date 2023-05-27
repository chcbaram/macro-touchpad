#include "power.h"






bool is_power_off = false;




bool powerInit(void)
{
  return true;
}

bool powerIsOff(void)
{
  return is_power_off;
}

void powerUpdate(void)
{
  enum 
  {
    POWER_INIT,
    POWER_CHECK,
    POWER_WAIT_OFF,
    POWER_OFF,
  };

  static uint8_t power_state = POWER_INIT;
  static uint32_t pre_time;


  switch(power_state)
  {
    case POWER_INIT:
      if (buttonGetPressed(0) == false)
      {
        power_state = POWER_CHECK;
      }
      break;

    case POWER_CHECK:
      if (buttonGetPressed(0) == true)
      {
        power_state = POWER_WAIT_OFF;
        pre_time = millis();
      }
      break;
    
    case POWER_WAIT_OFF:
      if (millis()-pre_time >= 2000)
      {
        is_power_off = true;
        power_state = POWER_OFF;
      }
      if (buttonGetPressed(0) == false)
      {
        power_state = POWER_CHECK;
      }      
      break;

    case POWER_OFF:
      gpioPinWrite(_PIN_GPIO_PWR_EN, _DEF_LOW);      
      break;
  }

}

