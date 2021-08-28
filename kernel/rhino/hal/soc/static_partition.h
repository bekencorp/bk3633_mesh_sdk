#ifndef _STATIC_PARTITION_H_
#define _STATIC_PARTITION_H_



#include <stdint.h>

#define STATIC_PARTITION_HEADER_CODE    0xABCDEF01

typedef enum
{
    STATIC_SECTION_START = 0,
    STATIC_SECTION_OTA,
    STATIC_SECTION_MAC,
    STATIC_SECTION_TRITUPLE,
    STATIC_SECTION_CHECKSUM,

    STATIC_SECTION_NONE = 0xff,

}static_section_t;

typedef struct
{
    uint32_t    hearder_code;
}platform_static_header_s;

typedef struct
{
    static_section_t sec_name;
    uint32_t sec_len;
    uint32_t start_offset;

}platform_static_partition_s;

int static_partition_read(static_section_t in_section, void *out_buf, uint32_t out_buf_len);
int static_partition_write(static_section_t in_section, void *in_buf, uint32_t write_len);
int static_partition_write_addr_head(static_section_t in_section);
#endif  //_STATIC_PARTITION_H_