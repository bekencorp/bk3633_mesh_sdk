#include "bim_updataImage.h"
#include "bim_icu.h"
#include "bim_uart2.h"
#include "bim_flash.h"
#include "bim_wdt.h"
#include <string.h>

ota_img_hdr_t hdr_ota_img;

int make_crc32_table(void);
uint32_t make_crc32(uint32_t crc,unsigned char *string,uint32_t size);

uint32_t bim_get_psec_backup_header(void)
{
    platform_static_header_s verify;
    uint32_t ret = flash_read((uint8_t *)&verify, OTA_HEADER_BACKUP_FADDR, sizeof(platform_static_header_s));

    if(verify.hearder_code == STATIC_PARTITION_HEADER_CODE)
    {
        ret = flash_read((uint8_t *)&hdr_ota_img, OTA_HEADER_BACKUP_FADDR + sizeof(platform_static_header_s), sizeof(ota_img_hdr_t));
        //bim_printf("hdr_ota_imghdr_ota_img.magic_num = 0x%x\n",hdr_ota_img.magic_num);
        //bim_printf("hdr_ota_imghdr_ota_img.dst_adr = 0x%x\n",hdr_ota_img.dst_adr);
        //bim_printf("hdr_ota_imghdr_ota_img.src_adr = 0x%x\n",hdr_ota_img.src_adr);
        //bim_printf("hdr_ota_imghdr_ota_img.crc = 0x%x\n ",hdr_ota_img.crc);
        //bim_printf("hdr_ota_imghdr_ota_img.ver = 0x%x\n",hdr_ota_img.ver);
        //bim_printf("hdr_ota_img.size = 0x%x\n",hdr_ota_img.size);
        //bim_printf("hdr_ota_img.termi_type = %d\n",hdr_ota_img.termi_type);
        //bim_printf("hdr_ota_img.break_point = 0x%x\n",hdr_ota_img.break_point);
        return 0;
    }
    else
    {
        // bim_printf("Static Partition Vacant\n");
        return 1;
    }

}

uint32_t calc_backup_sec_crc(uint32_t addr)
{
    uint8_t data[BLOCK_SIZE];
    uint32_t read_addr = addr;
    uint32_t calcuCrc = 0xffffffff;
    uint32_t crc_size = hdr_ota_img.size;
    uint16_t read_size;

    //bim_printf("cal_crc from 0x%x, len 0x%x\n",addr, crc_size);

    make_crc32_table();

    while(crc_size > 0)
    {
        read_size = (crc_size > BLOCK_SIZE)?BLOCK_SIZE:crc_size;
        flash_read(data, read_addr, BLOCK_SIZE);
        
        calcuCrc = make_crc32(calcuCrc, data, read_size);

        crc_size -= read_size;
        read_addr += read_size;
        memset(data, 0, BLOCK_SIZE);
    }

    return calcuCrc;
}

uint8_t bim_check_backup_sec_status(void)//NOT WRITE INFO TO FLASH
{
    uint32_t ret = bim_get_psec_backup_header();

    if(ret)
    {
        // bim_printf("No OTA Info\r\n");
        return SSTATUS_SECT_ABNORMAL;
    }

    if((hdr_ota_img.magic_num == 0xffff) || (hdr_ota_img.ver == 0xffff)  )
    {
        //bim_printf("no header data\r\n");
        return SSTATUS_SECT_ERASED;
    }
    else if(hdr_ota_img.magic_num == 0)
    {
        bim_printf("Already update\r\n");
        return SSTATUS_SECT_ERASED;
    }

    if(hdr_ota_img.termi_type != OTA_TERMI_FINISH)
    {
        //bim_printf("update not finished\r\n");
        return SSTATUS_SECT_BREAKPOINT;
    }

    if(hdr_ota_img.dst_adr < PARTITION_STACK_CPU_ADDR)
    {
        //Can not erase Boot section
        return SSTATUS_SECT_ABNORMAL;
    }

    uint32_t calcuCrc = calc_backup_sec_crc(hdr_ota_img.src_adr);

    if(hdr_ota_img.crc != calcuCrc || !hdr_ota_img.size)
    {
        //bim_printf("crc fail 0x%x, calcuCrc 0x%x\r\n", hdr_ota_img.crc, calcuCrc);
        return SSTATUS_SECT_ABNORMAL;
    }

    return SSTATUS_SECT_NORMAL;
}

uint8_t bim_check_up_mode(void)
{
    if(hdr_ota_img.magic_num == DUAL_SWITCH_MAGIC_NUM)
    {
        return OTA_DUAL_SWITCH;
    }
    else
    {
        return OTA_NORMAL;
    }
}

void bim_erase_image_sec(void)
{
    //uint8_t sector_cnt;
    //bim_printf("erase_image start \r\n");
    flash_erase(hdr_ota_img.dst_adr, hdr_ota_img.block_size);
    //bim_printf("erase_image end \r\n");
}

void bim_clear_magic_num(void)
{
    uint8_t blank_magic[4] = {0};
    flash_write(blank_magic, OTA_HEADER_BACKUP_FADDR + sizeof(platform_static_header_s), 4);
}

void bim_updata_backup_to_image_sec(void)
{
    uint8_t data[READ_BLOCK_SIZE];
    uint32_t read_addr = hdr_ota_img.src_adr;
    uint32_t read_end_addr = read_addr + hdr_ota_img.size;
    uint32_t write_addr = hdr_ota_img.dst_adr;

    bim_printf("Copy Start\n");
    while(read_addr < read_end_addr)
    {
        flash_read(data,read_addr,READ_BLOCK_SIZE);
        flash_write(data,write_addr,READ_BLOCK_SIZE);
        write_addr += READ_BLOCK_SIZE;
        read_addr += READ_BLOCK_SIZE;
    }

    bim_printf("update end\n");
}

uint32_t bim_select_sec(void)
{
    uint8_t bsec_status, up_mode;
    uint32_t addr_crc;
    
    bsec_status = bim_check_backup_sec_status();
    bim_printf("bsec_status = 0x%x \n", bsec_status);
    up_mode = bim_check_up_mode();
    bim_printf("up_mode = 0x%x \n", up_mode);
    
    switch(bsec_status)
    {
        case SSTATUS_SECT_NORMAL: // 1:I NORMAL ,B NORMAL,updata B -> I,RUN I
        {
            if(up_mode == OTA_DUAL_SWITCH)
            {
                //flash_wp_ALL();
                addr_crc = hdr_ota_img.src_adr;
                return (addr_crc*0x20/0x22);
            }
            else
            {
                flash_wp_none();
                bim_erase_image_sec();
                bim_updata_backup_to_image_sec();
                bim_clear_magic_num();
                flash_wp_ALL();
                return PARTITION_APP_CPU_ADDR;
            }

        }
        case SSTATUS_SECT_ERASED://://3:I NORMAL,B ERASED,RUN I
        case SSTATUS_SECT_BREAKPOINT:
        case SSTATUS_SECT_ABNORMAL:
        case SSTATUS_SECT_DIFF_ROM_VER:////4:I DIFF_ROM,B ERASED,NOT HAPPEN
        default:
        {
            if(up_mode == OTA_DUAL_SWITCH)
            {
                //flash_wp_ALL();
                addr_crc = hdr_ota_img.dst_adr;
                return (addr_crc*0x20/0x22);
            }
            else
            {
                //flash_wp_ALL();
                return PARTITION_APP_CPU_ADDR;
            }
        }

    }
}

uint32_t crc32_table[256];
int make_crc32_table(void)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        crc32_table[i] = c;
    }
    return 0;
}

uint32_t make_crc32(uint32_t crc,unsigned char *string,uint32_t size)
{
    while(size--)
    {
        crc = (crc >> 8)^(crc32_table[(crc^*string++)&0xff]);
    }
    return crc;
}
