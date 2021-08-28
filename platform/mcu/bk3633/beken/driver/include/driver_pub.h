/**
****************************************************************************************
*
* @file driver_pub.h
*
* @brief driver public definations
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef _DRIVER_PUB_H_
#define _DRIVER_PUB_H_

#include "typedef.h"

#define UART_CMD_MAGIC               (0xC124000)
#define GPIO_CMD_MAGIC               (0xcaa0000)
#define ICU_CMD_MAGIC                (0xe220000)
#define PWM_CMD_MAGIC                (0xe230000)
#define FLASH_CMD_MAGIC              (0xe240000)
#define SPI_CMD_MAGIC                (0xe250000)
#define I2S_CMD_MAGIC                (0xe280000)
#define TIMER_CMD_MAGIC              (0xe290000)
#define WDT_CMD_MAGIC                (0xe330000)
#define AON_WDT_CMD_MAGIC            (0xe340000)

#define DRIVER_ENABLE                   1
#define DRIVER_DISABLE                  0

typedef enum
{
    DRIV_SUCCESS = 0,
    DRIV_FAIL = 1,
    DRIV_WRONG_PARA,
} DRIVER_CTRL_RES;

#endif // _DRIVER_PUB_H_
// eof

