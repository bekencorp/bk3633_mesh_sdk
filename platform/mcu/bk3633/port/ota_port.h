#ifndef _OTA_PORT_H
#define _OTA_PORT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "static_partition.h"

#define OTA_CRC32_INIT_PARA     0xEDB88320
//jx debug
#define OTA_PAGE_SIZE            2048 //256


#ifdef CONFIG_OTA_DUAL_SWITCH
#define DUAL_SWITCH_MAGIC_NUM       0xaabbccdd
#endif

typedef struct
{
    uint32_t crc_table[256];
    uint32_t crc;
} CRC32_Context;

typedef struct
{
    uint32_t magic_num;
    // Secure OAD uses the Signature for image validation instead of calculating a CRC, but the use
    // of CRC==CRC-Shadow for quick boot-up determination of a validated image is still used.
    uint32_t dst_adr;
    uint32_t block_size;
    uint32_t src_adr;
    uint32_t crc;       // CRC must not be 0x0000 or 0xFFFF.
    // User-defined Image Version Number - default logic uses simple a '!=' comparison to start an OAD.
    uint16_t ver;
    uint16_t rom_ver;

    uint32_t size;
    uint16_t termi_type;
    uint16_t break_point;
}__attribute__((packed)) ota_img_hdr_t;

void CRC32_Init(CRC32_Context* contex);
uint32_t CRC32_Update(CRC32_Context* contex, uint8_t *f_data, uint32_t len);
//jx debug
//uint32_t crc32(uint32_t crc, const uint8_t *buf, int size);
uint32_t CRC_Update_api(uint32_t * crc_table, uint32_t crc, uint8_t *f_data, uint32_t len);
#endif  //_OTA_PORT_H