#include <stdint.h>

#define OTA_HEADER_BACKUP_FADDR         (0x07F000) //HAL_PARTITION_STATIC_PARA

#define PARTITION_STACK_CPU_ADDR                  (0x1F00)

/*********************************************************************
 * TYPEDEFS
 */
// The Image Header will not be encrypted, but it will be included in a Signature.

#define STATIC_PARTITION_HEADER_CODE    0xABCDEF01
typedef struct
{
    uint32_t    hearder_code;
}platform_static_header_s;
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

typedef enum  {
    OTA_TERMI_FINISH = 0,
    OTA_TERMI_BREAKPOINT,
} ota_terminate_type_e;

#define BLOCK_SIZE          0X10
#define READ_BLOCK_SIZE     0X20


enum
{
    SSTATUS_SECT_NORMAL = 0,
    SSTATUS_SECT_ERASED,
    SSTATUS_SECT_ABNORMAL,
    SSTATUS_SECT_UNKOWN ,
    SSTATUS_SECT_DIFF_ROM_VER ,
    SSTATUS_SECT_BREAKPOINT,
};

uint8_t bim_select_sec(void);
