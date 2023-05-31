#include "ap.h"
#include "widgets/lv_demo_widgets.h"


static void cliThread(void *args);




void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);


  if (xTaskCreate(cliThread, "cliThread", _HW_DEF_RTOS_THREAD_MEM_CLI, NULL, _HW_DEF_RTOS_THREAD_PRI_CLI, NULL) != pdPASS)
  {
    logPrintf("[NG] cliThread()\n");   
  }  

  delay(500);
  logBoot(false);
}


void apMain(void)
{
  uint32_t pre_time;

  lvglInit();

  lv_demo_widgets();
  
  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 1000)
    {
      pre_time = millis();
    }
    delay(1);   

    lvglUpdate();
  }
}

void cliThread(void *args)
{
  while(1)
  {
    cliMain();
    delay(2);
  }
}
