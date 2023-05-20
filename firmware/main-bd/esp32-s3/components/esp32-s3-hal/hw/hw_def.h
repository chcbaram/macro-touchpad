/*
 * hw_def.h
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */

#ifndef MAIN_HW_HW_DEF_H_
#define MAIN_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _DEF_FIRMWATRE_VERSION    "V230520R1"
#define _DEF_BOARD_NAME           "MACRO-TOUCHPAD"



#define _HW_DEF_RTOS_THREAD_PRI_CLI           5
#define _HW_DEF_RTOS_THREAD_PRI_BUTTON        5
#define _HW_DEF_RTOS_THREAD_PRI_SD            5
#define _HW_DEF_RTOS_THREAD_PRI_TE            5
#define _HW_DEF_RTOS_THREAD_PRI_FT6236        5
#define _HW_DEF_RTOS_THREAD_PRI_I2S           5
#define _HW_DEF_RTOS_THREAD_PRI_AUDIO         5
#define _HW_DEF_RTOS_THREAD_PRI_BT_HIDH       5


#define _HW_DEF_RTOS_THREAD_MEM_CLI           (8*1024)
#define _HW_DEF_RTOS_THREAD_MEM_BUTTON        (1*1024)
#define _HW_DEF_RTOS_THREAD_MEM_SD            (4*1024)
#define _HW_DEF_RTOS_THREAD_MEM_TE            (2*1024)
#define _HW_DEF_RTOS_THREAD_MEM_FT6236        (2*1024)
#define _HW_DEF_RTOS_THREAD_MEM_I2S           (4*1024)
#define _HW_DEF_RTOS_THREAD_MEM_AUDIO         (8*1024)
#define _HW_DEF_RTOS_THREAD_MEM_BT_HIDH       (6*1024)

#define _USE_HW_RTOS
#define _USE_HW_NVS
#define _USE_HW_CDC



#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_CLI_GUI
#define      HW_CLI_GUI_WIDTH       80
#define      HW_CLI_GUI_HEIGHT      24

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024






#endif /* MAIN_HW_HW_DEF_H_ */
