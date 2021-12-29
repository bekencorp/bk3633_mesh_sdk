/**
 ****************************************************************************************
 *
 * @file flash.c
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2009-2021
 *
 *
 ****************************************************************************************
 */


#include "stdint.h"
#include "hal/soc/wdg.h"
#include "aos/kernel.h"
#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "flash_pub.h"
#include "ll.h"

extern wdg_dev_t wdg;

#define SECTOR_SIZE 0x1000 /* 4 K/sector */
#define BLOCK1_SIZE 0x8000
#define BLOCK2_SIZE 0x10000

extern void  _app_data_flash_end;
extern const uint16_t stack_support_flash;
extern const hal_logic_partition_t hal_partitions_4M[];
extern const hal_logic_partition_t hal_partitions_8M[];

struct flash_context
{
    aos_mutex_t lock;
    int         initialized;
};
static struct flash_context g_flash_context;

void hal_flash_init()
    {
    if (g_flash_context.initialized)
    {
        return;
    }
    if (0 == aos_mutex_new(&g_flash_context.lock))
    {
        g_flash_context.initialized = 1;
    }
}

static void hal_flash_lock()
{
    if (g_flash_context.initialized)
    {
        aos_mutex_lock(&g_flash_context.lock, AOS_WAIT_FOREVER);
    }
}

static void hal_flash_unlock()
{
    if (g_flash_context.initialized)
    {
        aos_mutex_unlock(&g_flash_context.lock);
    }
}

static PROTECT_TYPE secure_sector_switch(uint32_t addr)
{
    if(addr <= 0x3FFFF)
    {
        return FLASH_PROTECT_NONE;
    }
    else if(addr <= 0x5FFFF)
    {
        return FLASH_PROTECT_SEC_64;
    }
    else if(addr <= 0x6FFFF)
    {
        return FLASH_PROTECT_SEC_96;
    }
    else if(addr <= 0x77FFF)
    {
        return FLASH_PROTECT_SEC_112;
    }
    else if(addr <= 0x7BFFF)
    {
        return FLASH_PROTECT_SEC_120;
    }
    else if(addr <= 0x7DFFF)
    {
        return FLASH_PROTECT_SEC_124;
    }
    else
    {
        return FLASH_PROTECT_SEC_126;
    }
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    hal_logic_partition_t *logic_partition;

    if(stack_support_flash == 8)
    	logic_partition = (hal_logic_partition_t *)&hal_partitions_8M[ in_partition ];
    else
    	logic_partition = (hal_logic_partition_t *)&hal_partitions_4M[ in_partition ];

    return logic_partition;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    hal_logic_partition_t *partition_info;
    uint32_t status;
    DD_HANDLE flash_hdl;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (in_partition == CONFIG_AOS_KV_PTN) {
        if (off_set >= CONFIG_AOS_KV_PTN_SIZE) {
            in_partition = CONFIG_AOS_KV_SECOND_PTN;
            off_set -= CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( in_partition );

    if(size + off_set > partition_info->partition_length) {
        return -1;
    }

    start_addr = (partition_info->partition_start_addr + off_set) & (~0xFFF);
    end_addr = ((partition_info->partition_start_addr + off_set + size  - 1) & (~0xFFF)) + 0x1000;
os_printf("%s, start 0x%x, end 0x%x append 0x%x\r\n",__func__, start_addr, end_addr, ((uint32_t)&_app_data_flash_end|(SECTOR_SIZE-1)));
#ifdef CONFIG_OTA_DUAL_SWITCH
    uint32_t base_addr = *((volatile uint32_t *)0x400000);
    base_addr = base_addr * 0x22 / 0x20;
    if((start_addr <= ((uint32_t)&_app_data_flash_end|(SECTOR_SIZE-1)) && start_addr >= base_addr) ||
        (end_addr > base_addr && start_addr <= base_addr))
    {
    	os_printf("Not allowed to erase code area, start_addr 0x%x! base 0x%x \r\n", start_addr, base_addr);
        return -1;
    }
#else
    if(start_addr <= ((uint32_t)&_app_data_flash_end|(SECTOR_SIZE-1)))
    {
    	os_printf("Not allowed to erase code area, start_addr 0x%x!\r\n", start_addr);
        return -1;
    }
#endif

    PROTECT_TYPE sec = secure_sector_switch(start_addr);
    hal_flash_secure_sector(sec);

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);

    addr = start_addr;
    uint32_t erase_size, cmd;
    while(addr < end_addr)
    {
        if(addr%BLOCK1_SIZE || end_addr - addr < BLOCK1_SIZE)
        {
            cmd = CMD_FLASH_ERASE_SECTOR;
            erase_size = SECTOR_SIZE;
        }
        else if(end_addr - addr >= BLOCK2_SIZE && !addr % BLOCK2_SIZE)
        {
            cmd = CMD_FLASH_ERASE_BLOCK2;
            erase_size = BLOCK2_SIZE;
        }
        else
        {
            cmd = CMD_FLASH_ERASE_BLOCK1;
            erase_size = BLOCK1_SIZE;
        }

        //hal_wdg_reload(&wdg);
        hal_aon_wdt_feed();
        hal_flash_lock();
        ddev_control(flash_hdl, cmd, (void *)&addr);
        hal_flash_unlock();
        // printf("%s, addr 0x%x, erase_size 0x%x\n", __func__, addr, erase_size);
        addr += erase_size;
    }


    //hal_wdg_reload(&wdg);
    hal_aon_wdt_feed();
	ddev_close(flash_hdl);

    hal_flash_secure_sector(FLASH_PROTECT_ALL);
    return 0;
}
                        
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len, BOOL flash_secure_sector)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
	uint32_t status;
    DD_HANDLE flash_hdl;
    uint32_t par_buf_len = in_buf_len;
    uint8_t *par_in_buf = (uint8_t *)in_buf;
#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (in_partition == CONFIG_AOS_KV_PTN) {
        if ((*off_set) >= CONFIG_AOS_KV_PTN_SIZE) {
            in_partition = CONFIG_AOS_KV_SECOND_PTN;
            *off_set = (*off_set) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > partition_info->partition_length) {
        os_printf("%s, L, Invalid params.\n", __func__, __LINE__);
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *off_set;

#ifdef CONFIG_OTA_DUAL_SWITCH
    uint32_t base_addr = *((volatile uint32_t *)0x400000);
    base_addr = base_addr * 0x22 / 0x20;
    if((start_addr <= ((uint32_t)&_app_data_flash_end|(SECTOR_SIZE-1)) && start_addr >= base_addr) ||
        (start_addr + in_buf_len  > base_addr && start_addr <= base_addr))
    {
    	os_printf("Not allowed to write code area, start_addr 0x%x! base 0x%x \r\n", start_addr, base_addr);
        return -1;
    }
#else
    if(start_addr <= (uint32_t)&_app_data_flash_end)
    {
    	os_printf("Not allowed to write code area, start_addr 0x%x.\r\n", start_addr);
        return -1;
    }
#endif
    do {
        //gpio_triger(0x07);
        if(flash_secure_sector == true)
        {
            PROTECT_TYPE sec = secure_sector_switch(start_addr);
            hal_flash_secure_sector(sec);
        }

        flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
        //hal_wdg_reload(&wdg);
        hal_aon_wdt_feed();
        hal_flash_lock();
        // printf("%s, par_in_buf %p, par_buf_len %d, len %d, in_buf_len %d start_addr 0x%x\n",\
        //        __func__, par_in_buf, par_buf_len,\
        //        (par_buf_len > HAL_FLASH_SEGMENTED_LEN) ? (HAL_FLASH_SEGMENTED_LEN) : par_buf_len, in_buf_len, start_addr);
        ddev_write(flash_hdl, par_in_buf, (par_buf_len > HAL_FLASH_SEGMENTED_LEN) ? (HAL_FLASH_SEGMENTED_LEN) : par_buf_len, start_addr);
        hal_flash_unlock();
        //hal_wdg_reload(&wdg);
        hal_aon_wdt_feed();
        ddev_close(flash_hdl);

        if(flash_secure_sector == true)
        {
            hal_flash_secure_sector(FLASH_PROTECT_ALL);
        }
        //gpio_triger(0x07);

        if (par_buf_len < HAL_FLASH_SEGMENTED_LEN) {
            break;
        }

        par_in_buf += HAL_FLASH_SEGMENTED_LEN;
        start_addr += HAL_FLASH_SEGMENTED_LEN;
        par_buf_len -= HAL_FLASH_SEGMENTED_LEN;//(par_buf_len > HAL_FLASH_SEGMENTED_LEN) ? (par_buf_len - HAL_FLASH_SEGMENTED_LEN) : par_buf_len;

        k_sleep(3);
    } while (true);

    *off_set += in_buf_len;
    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, int32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
	uint32_t status;
    DD_HANDLE flash_hdl;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (in_partition == CONFIG_AOS_KV_PTN) {
        if ((*off_set) >= CONFIG_AOS_KV_PTN_SIZE) {
            in_partition = CONFIG_AOS_KV_SECOND_PTN;
            *off_set = (*off_set) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > partition_info->partition_length) {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *off_set;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    //hal_wdg_reload(&wdg);
    hal_aon_wdt_feed();
    hal_flash_lock();
    if (ddev_read(flash_hdl, out_buf, out_buf_len, start_addr) != 0) {
        return -1;
    }
    hal_flash_unlock();
    //hal_wdg_reload(&wdg);
    hal_aon_wdt_feed();
	ddev_close(flash_hdl);

    *off_set += out_buf_len;
    return 0;
}

int32_t hal_flash_secure_sector(PROTECT_TYPE sector_type)
{
	DD_HANDLE flash_hdl;
    UINT32 status;
	PROTECT_TYPE param = sector_type;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&param);
    ddev_close(flash_hdl);
    return 0;
}

