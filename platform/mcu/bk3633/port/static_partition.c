
#include "hal/soc/soc.h"
#include "static_partition.h"
#include <misc/util.h>
#include <errno.h>
#include "common/log.h"

platform_static_partition_s bk3633_static_partition[] = 
{
    {STATIC_SECTION_OTA, 0x100, 0},
    {STATIC_SECTION_MAC, 0x100, 0x100},

};

static int static_partition_get_section(static_section_t in_section, uint32_t* offset, uint32_t* size)
{
    uint8_t i = 0;

    for(i = 0; i < ARRAY_SIZE(bk3633_static_partition); i++)
    {
        if(bk3633_static_partition[i].sec_name == in_section)
        {
            *offset = bk3633_static_partition[i].start_offset;
            *size = bk3633_static_partition[i].sec_len;
            return 0;
        }
    }

    return -1;
}

int static_partition_read(static_section_t in_section, void *out_buf, uint32_t out_buf_len)
{
    uint32_t offset = 0;
    uint32_t size = 0;
    platform_static_header_s verify;

    int ret = static_partition_get_section(in_section, &offset, &size);

    if(ret || out_buf_len > size - sizeof(platform_static_header_s))
    {
        LOG("No static part %d, %d\n", in_section, ret);
        return -1;
    }

    LOG("static_partition_read sec %d, offset 0x%x\n", in_section, offset);

    hal_flash_read( HAL_PARTITION_STATIC_PARA, &offset, &verify, sizeof(platform_static_header_s));
    if(verify.hearder_code == STATIC_PARTITION_HEADER_CODE)
    {
        ret = hal_flash_read( HAL_PARTITION_STATIC_PARA, &offset, out_buf, out_buf_len);
        return ret;
    }
    else
    {
        return -1;
    }

}

int static_partition_write(static_section_t in_section, void *in_buf, uint32_t write_len)
{
    uint32_t offset = 0, partition_start = 0;
    uint32_t size = 0;
    platform_static_header_s verify;
    verify.hearder_code = STATIC_PARTITION_HEADER_CODE;


    LOG("static_partition_write sec %d, len %d\n", in_section, write_len);

    int ret = static_partition_get_section(in_section, &offset, &size);

    if(ret || write_len > size)
    {
        return -1;
    }

    hal_logic_partition_t *partition_info = hal_flash_get_info(HAL_PARTITION_STATIC_PARA);
    uint8_t *temp_data = (uint8_t *)aos_malloc(partition_info->partition_length);

    if(temp_data == NULL)
    {
        return -1;
    }

    memset(temp_data, 0, partition_info->partition_length);

    ret = hal_flash_read( HAL_PARTITION_STATIC_PARA, &partition_start, temp_data, partition_info->partition_length);

    if(ret)
    {
        goto exit;
    }

    memset((uint8_t *)temp_data + offset, 0, size);
    memcpy((uint8_t *)temp_data + offset, &verify, sizeof(platform_static_header_s));
    memcpy((uint8_t *)temp_data + offset + sizeof(platform_static_header_s), in_buf, write_len);

    hal_flash_erase(HAL_PARTITION_STATIC_PARA, 0, partition_info->partition_length);

    partition_start = 0;
    ret = hal_flash_write( HAL_PARTITION_STATIC_PARA, &partition_start, temp_data, partition_info->partition_length);

exit:
    aos_free(temp_data);

    if(ret)
    {
        return -1;
    }
    else
    {
        return 0;
    }

}