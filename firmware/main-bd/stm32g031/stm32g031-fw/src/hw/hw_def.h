#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V230521R1"
#define _DEF_BOARD_NAME           "STM32G031-FW"
#define _DEF_BOARD_VERSION        0x00




#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2
#define      HW_UART_CH_DEBUG       _DEF_UART1

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       2

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         6

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1


#define _PIN_GPIO_SPK_EN            0
#define _PIN_GPIO_PWR_EN            1
#define _PIN_GPIO_LCD_BL            2
#define _PIN_GPIO_TS_RST            3
#define _PIN_GPIO_CHARGE_FLAG       4
#define _PIN_GPIO_SPI_CS            5


#endif