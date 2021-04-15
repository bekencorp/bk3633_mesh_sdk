/**
 ****************************************************************************************
 *
 * @file flash_pub.h
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2021
 *
 *
 ****************************************************************************************
 */

#ifndef _FLASH_PUB_H
#define _FLASH_PUB_H

#include "driver_pub.h"

#define FLASH_DEV_NAME                ("flash")

enum
{
    CMD_FLASH_SET_CLK = FLASH_CMD_MAGIC + 1,
    CMD_FLASH_SET_DCO,
    CMD_FLASH_SET_DPLL,

    CMD_FLASH_WRITE_ENABLE,
    CMD_FLASH_WRITE_DISABLE,

    CMD_FLASH_READ_SR,
    CMD_FLASH_WRITE_SR,

    CMD_FLASH_SET_QWFR,
    CMD_FLASH_CLR_QWFR,

    CMD_FLASH_SET_WSR,
    CMD_FLASH_GET_ID,
    CMD_FLASH_ERASE_SECTOR,         //4K
    CMD_FLASH_ERASE_BLOCK1,         //32K
    CMD_FLASH_ERASE_BLOCK2,         //64K
	CMD_FLASH_SET_HPM,
    CMD_FLASH_SET_PROTECT
};

typedef enum
{
    //ALL sec 128 
    FLASH_PROTECT_NONE,
    FLASH_PROTECT_SEC_126,            //504K     0x7DFFF
    FLASH_PROTECT_SEC_124,            //496K     0x7BFFF
    FLASH_PROTECT_SEC_120,            //480K     0x77FFF
    FLASH_PROTECT_SEC_112,            //448K     0x6FFFF
    FLASH_PROTECT_SEC_96,             //384K     0x5FFFF
    FLASH_PROTECT_SEC_64,            //256K      0x3FFFF
    FLASH_PROTECT_ALL,               //512K      0x7FFFF
} PROTECT_TYPE;

typedef enum
{
    FLASH_XTX_16M_SR_WRITE_DISABLE,
    FLASH_XTX_16M_SR_WRITE_ENABLE
} XTX_FLASH_MODE;

typedef struct
{
    UINT8 byte;
    UINT16 value;
} flash_sr_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void flash_init(void);
extern void flash_exit(void);
extern UINT8 flash_get_line_mode(void);
extern void flash_set_line_mode(UINT8 );

#endif //_FLASH_PUB_H
