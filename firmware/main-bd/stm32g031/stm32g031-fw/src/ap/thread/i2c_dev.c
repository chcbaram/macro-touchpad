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




bool i2cDevInit(void)
{
  return true;
}

void i2cDevUpdate(void)
{

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
      *p_data = gpioPinRead(_PIN_GPIO_CHARGE_FLAG);
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

    default:
      *p_data = 0x00;
      break;
  }
}
