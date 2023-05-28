#include "ap.h"
#include "thread/power.h"
#include "thread/i2c_dev.h"






void apInit(void)
{
  logBoot(false);
}

void apMain(void)
{
  uint32_t pre_time;
  uint16_t led_time = 500;
 

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= led_time)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }    

    if (powerIsOff() == true)
    {
      led_time = 100;
    }

    powerUpdate();
    i2cDevUpdate();
 }
}


