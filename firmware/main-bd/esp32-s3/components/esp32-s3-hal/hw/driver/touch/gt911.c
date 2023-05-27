#include "touch/gt911.h"
#include "i2c.h"
#include "cli.h"
#include "cli_gui.h"


#ifdef _USE_HW_GT911


#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);


#define GT911_TOUCH_WIDTH    480
#define GT911_TOUCH_HEIGTH   480



static void cliCmd(cli_args_t *args);
static bool readRegs(uint16_t reg_addr, void *p_data, uint32_t length);
static bool writeRegs(uint16_t reg_addr, void *p_data, uint32_t length);
static void gt911Thread(void* arg);


static uint8_t i2c_ch   = _DEF_I2C1;
static uint8_t i2c_addr = 0x14; 
static bool is_init = false;
static bool is_detected = false;
static SemaphoreHandle_t mutex_lock = NULL;





bool gt911Init(void)
{
  bool ret = false;


  if (mutex_lock == NULL)
  {
    mutex_lock = xSemaphoreCreateMutex();
  }

  if (i2cIsBegin(i2c_ch) == true)
    ret = true;
  else
    ret = i2cBegin(i2c_ch, 400);


  if (ret == true && i2cIsDeviceReady(i2c_ch, i2c_addr))
  {    
    is_detected = true;

    xTaskCreate(gt911Thread, "gt911Thread", _HW_DEF_RTOS_THREAD_MEM_GT911, NULL, _HW_DEF_RTOS_THREAD_PRI_GT911, NULL);      
  }
  else
  {
    ret = false;
  }

  logPrintf("[%s] gt911Init()\n", ret ? "OK":"NG");

  cliAdd("gt911", cliCmd);

  i2c_addr = 0x5D; 


  return ret;
}

bool gt911InitRegs(void)
{
  uint8_t config_tbl[] = 
  {
    0x00,                             // 0x8047 Config_Version
    (GT911_TOUCH_WIDTH>>0) & 0xFF,    // 0x8048 X Output Max L
    (GT911_TOUCH_WIDTH>>8) & 0xFF,    // 0x8049 X Output Max H
    (GT911_TOUCH_HEIGTH>>8) & 0xFF,   // 0x804A Y Output Max L
    (GT911_TOUCH_HEIGTH>>0) & 0xFF,   // 0x804B Y Output Max H
    0x05, // 0x804C Touch Number
    0x75, // 0x804D Module_Switch1
    0x01, // 0x804E Module_Switch2
    0x01, // 0x804F Shake_Count
    0x0F, // 0x8050 Filter
    0x24, // 0x8051 Large_Touch
	  0x0F, // 0x8052 Noise_Reduction
    0x64, // 0x8053 Screen_Touch_Level
    0x3C, // 0x8054 Screen_Leave_Level
    0x03, // 0x8055 Low_Power_Control
    0x05, // 0x8056 Refresh_Rate
    0x00, // 0x8057 x_threshold
    0x00, // 0x8058 y_threshold
    0x00, // 0x8059 X_Speed_Limit
    0x02, // 0x805A Y_Speed_Limit
    0x00, // 0x805B Space of border top
    0x00, // 0x805C Space of border left    
    0x00, // 0x805D
    0x16, // 0x805E
    0x19, // 0x805F

    0x1C,0x14,0x8C,0x0E,0x0E,0x24,0x00,0x31,0x0D,0x00,0x00,0x00,0x83,0x33,0x1D,0x00, // 0x8060
    0x41,0x00,0x00,0x3C,0x0A,0x14,0x08,0x0A,0x00,0x2B,0x1C,0x3C,0x94,0xD5,0x03,0x08, // 0x8070
    0x00,0x00,0x04,0x93,0x1E,0x00,0x82,0x23,0x00,0x74,0x29,0x00,0x69,0x2F,0x00,0x5F, // 0x8080
    0x37,0x00,0x5F,0x20,0x40,0x60,0x00,0xF0,0x40,0x30,0x55,0x50,0x27,0x00,0x00,0x00, // 0x8090
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0x80A0
    0x00,0x14,0x19,0x00,0x00,0x50,0x50,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12, // 0x80B0
	  0x14,0x16,0x18,0x1A,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0x80C0
    0x00,0x00,0x00,0x00,0x00,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x24,0x26,0x28,0x29,0x2A, // 0x80D0
    0x1C,0x18,0x16,0x14,0x13,0x12,0x10,0x0F,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0x00, // 0x80E0
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      // 0x80F0
    0x3C, // 0x80FF Config_Chksum
    0x01, // 0x8100 Config_Fresh
  };
  uint8_t check_sum = 0;
  uint8_t data;
  uint8_t CTP_CFG_GT911[184] =  {
      0x61,0xE0,0x01,0xE0,0x01,0x01,0x05,0x00,0x01,0xC8,
      0x28,0x0F,0x50,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x85,0x25,0x0A,
      0xEA,0xEC,0xB5,0x06,0x00,0x00,0x00,0x20,0x21,0x11,
      0x00,0x01,0x00,0x0F,0x00,0x2A,0x64,0x32,0x19,0x50,
      0x32,0xDC,0xFA,0x94,0x55,0x02,0x08,0x00,0x00,0x04,
      0x80,0xDE,0x00,0x80,0xE4,0x00,0x80,0xEA,0x00,0x7F,
      0xF0,0x00,0x7F,0xF6,0x00,0x7F,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x14,0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,
      0x04,0x02,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x21,0x20,0x1F,
      0x1E,0x1D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,//0x24,0x00,
  };


  uint8_t pid_str[4];
  readRegs(0x8140, pid_str, 4);
  pid_str[4] = 0;
  logPrintf("GT%s", pid_str);

  // for (int i=0; i<sizeof(test); i++)
  // {
  //   writeRegs(0x8047 + i, &test[i], 1);
  // }
  check_sum = 0;  
  for (int i=0; i<sizeof(CTP_CFG_GT911); i++)
  {
    check_sum += CTP_CFG_GT911[i];
  }

  check_sum = (~check_sum) + 1;

  uint8_t temp[2];
  temp[0] = check_sum;
  temp[1] = 1;

  writeRegs(0x8047, CTP_CFG_GT911, sizeof(CTP_CFG_GT911));
  writeRegs(0x80FF, temp, 2);


  // check_sum = 0;  
  // for (int i=0; i<sizeof(config_tbl)-2; i++)
  // {
  //   check_sum += config_tbl[i];
  //   writeRegs(0x8047 + i, &config_tbl[i], 1);
  // }
  // check_sum = (~check_sum) + 1;
  // data = 1;  
  // writeRegs(0x80FF, &check_sum, 1);
  // writeRegs(0x8100, &data, 1);

  return true;
}

void gt911Thread(void* arg)
{
  while(1)
  {
    //gt911InitRegs();

    is_init = true;
    while(1)
    {
      delay(10);
    }
  }
}

bool readRegs(uint16_t reg_addr, void *p_data, uint32_t length)
{
  bool ret;

  lock();
  ret = i2cRead16Bytes(i2c_ch, i2c_addr, reg_addr, p_data, length, 10);
  unLock();

  return ret;
}

bool writeRegs(uint16_t reg_addr, void *p_data, uint32_t length)
{
  bool ret;

  lock();
  ret = i2cWrite16Bytes(i2c_ch, i2c_addr, reg_addr, p_data, length, 10);
  unLock();

  return ret;
}

uint16_t gt911GetWidth(void)
{
  return GT911_TOUCH_WIDTH;
}

uint16_t gt911GetHeight(void)
{
  return GT911_TOUCH_HEIGTH;
}

bool gt911GetInfo(gt911_info_t *p_info)
{
  bool ret;
  uint8_t buf[14];

  if (is_init == false)
  {
    p_info->count = 0;
    return false;
  }

  // ret = readRegs(0x00, buf, 14);
  // if (ret == true)
  // {
  //   p_info->gest_id = buf[FT6236_REG_GEST_ID];
  //   p_info->count   = buf[FT6236_REG_TD_STATUS] & 0x0F;
  //   if (p_info->count <= 2)
  //   {
  //     for (int i=0; i<p_info->count; i++)
  //     {
  //       uint16_t x;
  //       uint16_t y;

  //       p_info->point[i].id     = (buf[FT6236_REG_P_YH     + (6*i)] & 0xF0) >> 4;
  //       p_info->point[i].event  = (buf[FT6236_REG_P_XH     + (6*i)] & 0xC0) >> 6;
  //       p_info->point[i].weight = (buf[FT6236_REG_P_WEIGHT + (6*i)] & 0xFF) >> 0;
  //       p_info->point[i].area   = (buf[FT6236_REG_P_MISC   + (6*i)] & 0xFF) >> 4;

  //       x  = (buf[FT6236_REG_P_XH + (6*i)] & 0x0F) << 8;
  //       x |= (buf[FT6236_REG_P_XL + (6*i)] & 0xFF) << 0;
  //       y  = (buf[FT6236_REG_P_YH + (6*i)] & 0x0F) << 8;
  //       y |= (buf[FT6236_REG_P_YL + (6*i)] & 0xFF) << 0;

  //       p_info->point[i].x = y;
  //       p_info->point[i].y = GT911_TOUCH_HEIGTH - x; 
  //     }
  //   }
  //   else
  //   {
  //     ret = false;
  //   }
  // }

  return ret;
}

void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    uint8_t reg_data;

    cliPrintf("is_init     : %s\n", is_init ? "True" : "False");
    cliPrintf("is_detected : %s\n", is_detected ? "True" : "False");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "init"))
  {
    gt911InitRegs();
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "read"))
  {
    uint16_t addr;
    uint16_t len;
    uint8_t data;

    addr = args->getData(1);
    len  = args->getData(2);

    for (int i=0; i<len; i++)
    {
      if (readRegs(addr + i, &data, 1) == true)
      {
        cliPrintf("0x%04x : 0x%02X\n", addr + i, data);
      }
      else
      {
        cliPrintf("readRegs() Fail\n");
        break;
      }
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write"))
  {
    uint16_t addr;
    uint8_t  data;

    addr = args->getData(1);
    data = args->getData(2);


    if (writeRegs(addr, &data, 1) == true)
    {
      cliPrintf("0x%02x : 0x%02X\n", addr, data);
    }
    else
    {
      cliPrintf("writeRegs() Fail\n");
    }

    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "get") && args->isStr(1, "info"))
  {
    gt911_info_t info;
    uint32_t pre_time;
    uint32_t exe_time;

    while(cliKeepLoop())
    {
      pre_time = micros();
      if (gt911GetInfo(&info) == true)
      {
        exe_time = micros()-pre_time;

        cliPrintf("cnt : %d %3dus, g=%d ", info.count, exe_time, info.gest_id);

        for (int i=0; i<info.count; i++)
        {
          cliPrintf(" - ");
          cliPrintf("id=%d evt=%2d x=%3d y=%3d w=%3d a=%3d ", 
            info.point[i].id,      
            info.point[i].event,      
            info.point[i].x, 
            info.point[i].y, 
            info.point[i].weight, 
            info.point[i].area
            );
        }

        cliPrintf("\n");
      }
      else
      {
        cliPrintf("gt911GetInfo() Fail\n");
        break;
      }
      delay(10);
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "gui"))
  {
    gt911_info_t info;
    gt911_info_t info_pre;

    info.count = 0;
    info_pre.count = 0;
    cliGui()->initScreen(80, 24);

    while(cliKeepLoop())
    {
      cliGui()->drawBox(0, 0, 480/10 + 1, 480/20 + 1, "");

      if (gt911GetInfo(&info) == true)
      {
        uint16_t x;
        uint16_t y;

        for (int i=0; i<info_pre.count; i++)
        {
          if (info.point[i].x != info_pre.point[i].x || 
              info.point[i].y != info_pre.point[i].y ||
              info.count != info_pre.count)
          {
            x = info_pre.point[i].x/10;
            y = info_pre.point[i].y/20;          
            cliGui()->eraseBox(x, y, 6, 3);
            cliGui()->movePrintf(x+2, y+1, " ");
            cliGui()->movePrintf(x, y+3, "       ");
          }
        }
        for (int i=0; i<info.count; i++)
        {
          x = info.point[i].x/10;
          y = info.point[i].y/20;
          cliGui()->drawBox(x, y, 6, 3, "");
          cliGui()->movePrintf(x+2, y+1, "%d", info.point[i].id);
          cliGui()->movePrintf(x, y+3, "%3d:%3d", info.point[i].x, info.point[i].y);
        }
        info_pre = info;
      }
      delay(10);
    }

    cliGui()->closeScreen();    
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("gt911 info\n");
    cliPrintf("gt911 init\n");
    cliPrintf("gt911 read addr len[0~255]\n");
    cliPrintf("gt911 write addr data \n");
    cliPrintf("gt911 get info\n");
    cliPrintf("gt911 gui\n");
  }
}

#endif