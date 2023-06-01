#include "i2c_dev.h"


#define I2C_REG_VERSION         0x00    // RO
#define I2C_REG_POWER_BTN       0x01    // RO
#define I2C_REG_CHARGE_FLAG     0x02    // RO
#define I2C_REG_TOUCH_OUT       0x03    // RO
#define I2C_REG_I2S_SD_MODE     0x04    // RW
#define I2C_REG_LCD_TS_RST      0x05    // RW
#define I2C_REG_LCD_BL          0x06    // RW
#define I2C_REG_LCD_WR_CMD      0x07    // WO
#define I2C_REG_LCD_WR_DAT      0x08    // WO
#define I2C_REG_LCD_BL_PWM      0x09    // RW
#define I2C_REG_BAT_ADC_UP      0x0A    // WO
#define I2C_REG_BAT_ADC_L       0x0B    // RO
#define I2C_REG_BAT_ADC_H       0x0C    // RO



static uint16_t bat_adc = 0;
static bool     bat_is_connected = false;
static bool     bat_is_charging = false;
static uint32_t charge_pre_time;
static uint8_t  charge_pre = 0;
static uint8_t  charge_cnt = 0;



bool i2cDevInit(void)
{
  charge_pre_time = millis();
  return true;
}

void i2cDevUpdate(void)
{
  uint8_t charge_pin;

  charge_pin = gpioPinRead(_PIN_GPIO_CHARGE_FLAG);
  if (charge_pin != charge_pre)
  {
    if (charge_cnt < 255)
      charge_cnt++;
  }
  charge_pre = charge_pin;

  if (millis()-charge_pre_time >= 100)
  {
    charge_pre_time = millis();
    
    if (charge_cnt > 20)
      bat_is_connected = false;
    else
      bat_is_connected = true;

    charge_cnt = 0;
  }

  if (bat_is_connected && gpioPinRead(_PIN_GPIO_CHARGE_FLAG) == _DEF_LOW)
    bat_is_charging = true;
  else
    bat_is_charging = false;
}


bool lcdWriteCmd(uint8_t data)
{
  uint16_t tx_data;

  gpioPinWrite(_PIN_GPIO_SPI_CS, _DEF_LOW);

  tx_data = (0<<8) | data;
  spiSetBitWidth(_DEF_SPI1, 9);
  spiTransfer(_DEF_SPI1, (uint8_t *)&tx_data, NULL, 1, 10);

  gpioPinWrite(_PIN_GPIO_SPI_CS, _DEF_HIGH);

  return true;
}

bool lcdWriteData(uint8_t data)
{
  uint16_t tx_data;

  gpioPinWrite(_PIN_GPIO_SPI_CS, _DEF_LOW);

  tx_data = (1<<8) | data;
  spiSetBitWidth(_DEF_SPI1, 9);
  spiTransfer(_DEF_SPI1, (uint8_t *)&tx_data, NULL, 1, 10);

  gpioPinWrite(_PIN_GPIO_SPI_CS, _DEF_HIGH);

  return true;
}

void i2cReadCallback(uint8_t addr, uint8_t *p_data)
{
  switch(addr)
  {
    case I2C_REG_VERSION:
      *p_data =_DEF_BOARD_VERSION;
      break;

    case I2C_REG_POWER_BTN:
      *p_data = buttonGetPressed(_DEF_BUTTON1);
      break;

    case I2C_REG_CHARGE_FLAG:
      *p_data = (bat_is_connected<<1) | (bat_is_charging<<0);
      break;

    case I2C_REG_TOUCH_OUT:
      *p_data = buttonGetPressed(_DEF_BUTTON2);
      break;

    case I2C_REG_I2S_SD_MODE:
      *p_data = gpioPinRead(_PIN_GPIO_SPK_EN);
      break;

    case I2C_REG_LCD_TS_RST:
      *p_data = gpioPinRead(_PIN_GPIO_TS_RST);
      break;

    case I2C_REG_LCD_BL:
      *p_data = gpioPinRead(_PIN_GPIO_LCD_BL);
      break;

    case I2C_REG_LCD_BL_PWM:
      *p_data = pwmRead(_DEF_PWM1);
      break;

    case I2C_REG_BAT_ADC_L:
      *p_data = (bat_adc>>0) & 0xFF;
      break;

    case I2C_REG_BAT_ADC_H:
      *p_data = (bat_adc>>8) & 0xFF;
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
    case I2C_REG_I2S_SD_MODE:
      gpioPinWrite(_PIN_GPIO_SPK_EN, p_data[0] > 0 ? 1:0);
      break;

    case I2C_REG_LCD_TS_RST:
      gpioPinWrite(_PIN_GPIO_TS_RST, p_data[0] > 0 ? 1:0);
      break;

    case I2C_REG_LCD_BL:
      gpioPinWrite(_PIN_GPIO_LCD_BL, p_data[0] > 0 ? 1:0);
      break;
    
    case I2C_REG_LCD_WR_CMD:
      lcdWriteCmd(p_data[0]);
      break;

    case I2C_REG_LCD_WR_DAT:
      lcdWriteData(p_data[0]);
      break;

    case I2C_REG_LCD_BL_PWM:
      pwmWrite(_DEF_PWM1, p_data[0]);
      break;

    case I2C_REG_BAT_ADC_UP:
      if (p_data[0] > 0)
      {
        bat_adc = adcRead12(0);
      }
      break;

    default:
      *p_data = 0x00;
      break;
  }
}
