/*
 * gpio.c
 *
 *  Created on: 2022. 9. 2.
 *      Author: baram
 */


#include "gpio.h"
#include "cli.h"
#include "i2c_exp.h"

#ifdef _USE_HW_GPIO
#include "driver/gpio.h"


typedef enum
{
  GPIO_HW,
  GPIO_I2C_EXP,
} GpioType_t;


typedef struct
{
  GpioType_t    type;
  uint32_t      pin;
  uint8_t       mode;
  bool          init_value;
  const char   *p_name;
} gpio_tbl_t;


static const gpio_tbl_t gpio_tbl[GPIO_MAX_CH] =
    {
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_POWER_BTN,     _DEF_INPUT,   _DEF_HIGH, "PWR_BTN"},      // 0. PWR_BTN      
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_CHARGE_FLAG,   _DEF_INPUT,   _DEF_HIGH, "CHARGE_FLAG"},  // 1. CHARGE_FLAG     
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_TOUCH_OUT,     _DEF_INPUT,   _DEF_HIGH, "TOUCH_OUT"},    // 2. TOUCH_OUT 
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_I2S_SD_MODE,   _DEF_OUTPUT,  _DEF_HIGH, "I2S_SD"},       // 3. I2S_SD   
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_LCD_TS_RST,    _DEF_OUTPUT,  _DEF_HIGH, "LCD_TS_RST"},   // 4. LCD_TS_RST      
      {GPIO_I2C_EXP, (uint32_t)I2C_REG_LCD_BL,        _DEF_OUTPUT,  _DEF_LOW , "LCD_BL"},       // 5. LCD_BL  
      {GPIO_HW,      (uint32_t)GPIO_NUM_0,            _DEF_OUTPUT,  _DEF_HIGH, "LCD_TS_INT"},   // 6. LCD_TS_INT  
    };

static uint8_t gpio_data[GPIO_MAX_CH];
static bool is_init = false;

#ifdef _USE_HW_CLI
static void cliGpio(cli_args_t *args);
#endif



bool gpioInit(void)
{
  bool ret = true;



  if (i2cExpIsInit() != true)
    return false;

  is_init = ret;

  for (int i=0; i<GPIO_MAX_CH; i++)
  {
    gpioPinMode(i, gpio_tbl[i].mode);
    gpioPinWrite(i, gpio_tbl[i].init_value);
  }

#ifdef _USE_HW_CLI
  cliAdd("gpio", cliGpio);
#endif

  return ret;
}

bool gpioPinMode(uint8_t ch, uint8_t mode)
{
  bool ret = true;


  if (ch >= GPIO_MAX_CH)
  {
    return false;
  }

  if (gpio_tbl[ch].type == GPIO_I2C_EXP)
  {    
    return true;
  }

  gpio_reset_pin(gpio_tbl[ch].pin);
  
  switch(mode)
  {
    case _DEF_INPUT:
      gpio_pullup_dis(gpio_tbl[ch].pin);
      gpio_pulldown_dis(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_INPUT_PULLUP:
      gpio_pullup_en(gpio_tbl[ch].pin);
      gpio_pulldown_dis(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_INPUT_PULLDOWN:
      gpio_pullup_dis(gpio_tbl[ch].pin);
      gpio_pulldown_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_OUTPUT:
      gpio_pullup_dis(gpio_tbl[ch].pin);
      gpio_pulldown_dis(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;

    case _DEF_OUTPUT_PULLUP:
      gpio_pullup_en(gpio_tbl[ch].pin);
      gpio_pulldown_dis(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;

    case _DEF_OUTPUT_PULLDOWN:
      gpio_pullup_dis(gpio_tbl[ch].pin);
      gpio_pulldown_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;
  }

  return ret;
}

void gpioPinWrite(uint8_t ch, uint8_t value)
{
  if (ch >= GPIO_MAX_CH || is_init != true)
  {
    return;
  }

  if (gpio_tbl[ch].type == GPIO_I2C_EXP)
  {
    i2cExpWrite(gpio_tbl[ch].pin, &value, 1);
  }
  else
  {
    gpio_set_level(gpio_tbl[ch].pin, value);
  }

  gpio_data[ch] = value;
}

uint8_t gpioPinRead(uint8_t ch)
{
  uint8_t ret;

  if (ch >= GPIO_MAX_CH || is_init != true)
  {
    return false;
  }

  if (gpio_tbl[ch].type == GPIO_I2C_EXP)
  {
    ret = 0;
    i2cExpRead(gpio_tbl[ch].pin, &ret, 1);
  }
  else
  {
    ret = gpio_get_level(gpio_tbl[ch].pin);
  }

  gpio_data[ch] = ret;
  return ret;
}

void gpioPinToggle(uint8_t ch)
{
  if (ch >= GPIO_MAX_CH || is_init != true)
  {
    return;
  }

  gpio_data[ch] = !gpio_data[ch];

  gpioPinWrite(gpio_tbl[ch].pin, gpio_data[ch]);
}





#ifdef _USE_HW_CLI
void cliGpio(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("is init : %d\n", is_init);
    for (int i=0; i<GPIO_MAX_CH; i++)
    {
      cliPrintf("%d %-16s - %d\n", i, gpio_tbl[i].p_name, gpioPinRead(i));
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<GPIO_MAX_CH; i++)
      {
        cliPrintf("%d", gpioPinRead(i));
      }
      cliPrintf("\n");
      delay(100);
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "read") == true)
  {
    uint8_t ch;

    ch = (uint8_t)args->getData(1);

    while(cliKeepLoop())
    {
      cliPrintf("gpio read %d : %d\n", ch, gpioPinRead(ch));
      delay(100);
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    uint8_t ch;
    uint8_t data;

    ch   = (uint8_t)args->getData(1);
    data = (uint8_t)args->getData(2);

    gpioPinWrite(ch, data);

    cliPrintf("gpio write %d : %d\n", ch, data);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("gpio info\n");
    cliPrintf("gpio show\n");
    cliPrintf("gpio read ch[0~%d]\n", GPIO_MAX_CH-1);
    cliPrintf("gpio write ch[0~%d] 0:1\n", GPIO_MAX_CH-1);
  }
}
#endif


#endif