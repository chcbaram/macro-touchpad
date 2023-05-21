/*
 * log.c
 *
 *  Created on: Nov 12, 2021
 *      Author: baram
 */




#include "log.h"
#include "uart.h"


#ifdef _USE_HW_LOG






static bool is_init = false;
static bool is_boot_log = true;
static bool is_enable = true;
static bool is_open = false;

static uint8_t  log_ch = LOG_CH;
static uint32_t log_baud = 115200;

static char print_buf[256];

#ifdef _USE_HW_RTOS
static SemaphoreHandle_t mutex_lock;
#endif



#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif





bool logInit(void)
{
  is_init = true;

  return true;
}

void logEnable(void)
{
  is_enable = true;
}

void logDisable(void)
{
  is_enable = false;
}

void logBoot(uint8_t enable)
{
  is_boot_log = enable;
}

bool logOpen(uint8_t ch, uint32_t baud)
{
  log_ch   = ch;
  log_baud = baud;
  is_open  = true;

  is_open = uartOpen(ch, baud);

  return is_open;
}


void logPrintf(const char *fmt, ...)
{
  va_list args;
  int len;

  if (is_init != true) return;


  va_start(args, fmt);
  len = vsnprintf(print_buf, 256, fmt, args);

  if (is_open == true && is_enable == true)
  {
    uartWrite(log_ch, (uint8_t *)print_buf, len);
  }
  va_end(args);
}

#endif