/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_SERVICE_H_
#define _AIS_SERVICE_H_

#include <misc/byteorder.h>
#include <gatt.h>
#include "ota_common.h"


#define OTA_SERVICE_UUID_128  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
                                  0x00, 0x40, 0x51, 0x04, 0xC0, 0xFF, 0x00, 0xF0

#define OTA_IMG_IDENTIFY_UUID_128  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
                                  0x00, 0x40, 0x51, 0x04, 0xC1, 0xFF, 0x00, 0xF0

#define OTA_IMG_BLOCK_UUID_128  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
                                  0x00, 0x40, 0x51, 0x04, 0xC2, 0xFF, 0x00, 0xF0

#define BEKEN_OTA_SERVICE_UUID          BT_UUID_DECLARE_128(OTA_SERVICE_UUID_128)
#define BEKEN_OTA_IMG_IDENTIFY_UUID     BT_UUID_DECLARE_128(OTA_IMG_IDENTIFY_UUID_128)
#define BEKEN_OTA_IMG_BLOCK_UUID        BT_UUID_DECLARE_128(OTA_IMG_BLOCK_UUID_128)

#define  OTAS_FFC1_DATA_LEN  25
#define  OTAS_FFC2_DATA_LEN  25

#define OTA_INIT_CONN_INT_MIN                0x000C  /* N * 1.25 ms */
#define OTA_INIT_CONN_INT_MAX                0x000C  /* N * 1.25 ms */
#define OTA_INIT_CONN_LATENCY                0       /* 0       */
#define OTA_INIT_CONN_TO                     2000    /* 20 s */

typedef struct
{
    //uint32_t crc;       // CRC must not be 0x0000 or 0xFFFF.
    uint32_t uid;       // User-defined Image Identification bytes.
    // User-defined Image Version Number - default logic uses simple a '!=' comparison to start an OAD.
    uint16_t ver;
    uint16_t rom_ver;     // Rom ver.

    uint16_t len;        // Image length in 4-byte blocks (i.e. HAL_FLASH_WORD_SIZE blocks).
} ota_hdr_para_t;

typedef struct
{
    uint16_t otaBlkNum;
    uint16_t otaBlkTot;
    uint8_t ota_firmware_type;
    uint8_t update_comp : 1;
    uint8_t update_start : 1;
    uint8_t sys_power_on : 1;
} ota_config_t;

struct otas_env_tag
{
    struct bt_conn *p_conn;
    k_timer_t timer;

    ota_config_t config_para;

    ota_hdr_para_t img_hdr;

    struct bt_gatt_attr *fcc1_attr;
    struct bt_gatt_attr *fcc2_attr;

    uint8_t ffc1_value[OTAS_FFC1_DATA_LEN];
    uint8_t ffc2_value[OTAS_FFC2_DATA_LEN];

};

typedef struct
{
    // Secure OAD uses the Signature for image validation instead of calculating a CRC, but the use
    // of CRC==CRC-Shadow for quick boot-up determination of a validated image is still used.
    uint32_t crc;       // CRC must not be 0x0000 or 0xFFFF.
    // User-defined Image Version Number - default logic uses simple a '!=' comparison to start an OAD.
    uint16_t ver;

    uint16_t len;        // Image length in 4-byte blocks (i.e. HAL_FLASH_WORD_SIZE blocks). include beken hdr

    uint32_t  uid;       // User-defined Image Identification bytes.
    uint8_t  crc_status;     // cur image crc status
    uint8_t  sec_status;     // cur image sec status
    uint16_t  rom_ver;     // Rom ver.

}beken_image_hdr;

bool ota_update_complete_get(void);

#endif

