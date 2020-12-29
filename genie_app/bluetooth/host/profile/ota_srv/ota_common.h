/**
 ****************************************************************************************
 *
 * @file ota_common.h
 *
 * @brief Header file - OTA Profile Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _OTA_COMMON_H_
#define _OTA_COMMON_H_

#include "ota_module.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define OTA_APP_PART_UID                    (0x42424242)
#define OTA_APP_STACK_UID                   (0x53535353)

// The Image is transporte in 16-byte blocks in order to avoid using blob operations.

#define OTA_BLOCK_SIZE        16
#define HAL_FLASH_WORD_SIZE   (sizeof(uint32_t))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define READ16B(x)         (uint16_t)(((uint8_t *)x)[0] | ((uint8_t *)x)[1] << 8)
#define READ32B(x)          ((uint32_t)READ16B((uint16_t *)x) | (uint32_t)(READ16B((uint16_t *)x + 1) << 16))


typedef enum
{
    OTA_FM_TYPE_NONE = 0,
    OTA_FM_TYPE_APP,
    OTA_FM_TYPE_STACK,

}oad_firmware_type_e;


#endif


