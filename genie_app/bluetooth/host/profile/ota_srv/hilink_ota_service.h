/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_SERVICE_H_
#define _AIS_SERVICE_H_

#include <misc/byteorder.h>
#include <gatt.h>
#include "ota_common.h"



#define OTA_SERVICE_UUID           0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA0, 0xFD, 0x00,0x00

//#define OTA_READ_UUID              0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA3, 0xFD, 0x00,0x00

#define OTA_AUTH_UUID              0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA4, 0xFD, 0x00,0x00

#define OTA_STATUS_UUID            0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA6, 0xFD, 0x00,0x00

#define OTA_VERSION_UUID           0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA7, 0xFD, 0x00,0x00

#define OTA_CHECKSUM_UUID          0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA8, 0xFD, 0x00,0x00

#define HILINK_SERVICE                 BT_UUID_DECLARE_128(OTA_SERVICE_UUID)
//#define HILINK_READ_UUID               BT_UUID_DECLARE_128(OTA_READ_UUID)
#define HILINK_OTA_AUTH_UUID           BT_UUID_DECLARE_128(OTA_AUTH_UUID)
#define HILINK_OTA_STATUS_UUID         BT_UUID_DECLARE_128(OTA_STATUS_UUID)
#define HILINK_OTA_VERSION_UUID        BT_UUID_DECLARE_128(OTA_VERSION_UUID)
#define HILINK_OTA_CHECKSUM_UUID       BT_UUID_DECLARE_128(OTA_CHECKSUM_UUID)

#define CUSTOM_VERSION 0x0101


/* Get local WORD from external 2 BYTE, Little-Endian format */
#define LE_EXTRN2WORD(p) (((*(p)) << 8) + ((*((p)+1)) & 0xff))

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
    //uint32_t crc;       // CRC must not be 0x0000 or 0xFFFF.
    uint32_t uid;       // User-defined Image Identification bytes.
    // User-defined Image Version Number - default logic uses simple a '!=' comparison to start an OAD.
    uint16_t ver;
    uint16_t rom_ver;     // Rom ver.
    uint16_t custom_ver;
    uint16_t len;        // Image length in 4-byte blocks (i.e. HAL_FLASH_WORD_SIZE blocks).
} ota_hdr_custom_para_t;


typedef struct
{
/*
    uint16_t otaBlkNum;
    uint16_t otaBlkTot;
    uint16_t otaFileLen;
*/
    uint8_t ota_firmware_type;
    uint8_t update_comp : 1;
    uint8_t update_start : 1;
} ota_config_t;

struct otas_env_tag
{
    struct bt_conn *p_conn;
    k_timer_t timer;

    ota_config_t config_para;

    ota_hdr_para_t img_hdr;
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
void ota_init(uint32_t p_offset);
void ota_mutex_init(void);
#endif

