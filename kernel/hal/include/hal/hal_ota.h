/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_OTA_H
#define HAL_OTA_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "base.h"


typedef enum
{
	//APP
	HiLink_APP_CMD_BASE							= 0XA000,
    HiLink_APP_CMD_SWITCH 						= 0XA001,
    HiLink_APP_CMD_BRIGHTNESS 					= 0XA002,
    HiLink_APP_CMD_TEMP							= 0XA003,
    HiLink_APP_CMD_MODE							= 0XA004,
    HiLink_APP_CMD_SENSRO_GEAR					= 0XA005,
    HiLink_APP_CMD_CNN_NOTIFY					= 0XA006,
    HiLink_APP_CMD_TIME							= 0XA0F1,
    HiLink_APP_CMD_COUNTDOWN					= 0XA0F2,
    HiLink_APP_CMD_TOMATO_TIME					= 0XA0F3,
    HiLink_APP_CMD_UNIX_TIME					= 0XA0F4,
    HiLink_APP_CMD_OTA_AUTH						= 0XA0FF,
    HiLink_APP_CMD_GET_ALL_STATUS				= 0XAFFF,    

	//OTA
	HiLink_OTA_CMD_FILE_START					= 0XFF01,
	HiLink_OTA_CMD_FILE_CHECKSUM				= 0XFF02,
	HiLink_OTA_CMD_FILE_WRITE					= 0XFF03,
	HiLink_OTA_CMD_FILE_FINSHED					= 0XFF04,
	HiLink_OTA_CMD_FILE_RESET					= 0XFF05,
	HiLink_OTA_CMD_FILE_DEV_VERSION				= 0XFF06,
} HiLink_app_cmd_t;


enum ota_parti_e {
    OTA_PARTITION_DEFAULT = 0,
    OTA_PARTITION_APP,
    OTA_PARTITION_STACK,
};

typedef enum  {
    OTA_TERMI_FINISH = 0,
    OTA_TERMI_BREAKPOINT,
} ota_terminate_type_e;

typedef struct {
    uint32_t start_address; /* the address of the bin saved on flash. */
    uint32_t length;        /* file real length */
    uint8_t  version[8];
    uint8_t  type;          /* B:bootloader, P:boot_table, A:application, D: 8782 driver */
    uint8_t  upgrade_type;  /* u:upgrade */
    uint16_t crc;
    uint8_t  reserved[4];
} boot_table_t;

typedef struct
{
    uint16_t ver;
    uint16_t rom_ver;
    uint16_t size;
}ota_img_tag_t;

typedef struct hal_ota_module_s hal_ota_module_t;

typedef int hal_stat_t;

struct hal_ota_module_s {
    hal_module_base_t base;

    /* Link to HW */
    int (*init)(void *something);
    int (*deinit)(void *something);
    int (*ota_save)(uint8_t *in_buf , uint32_t in_buf_len);
    int (*ota_read)(volatile uint32_t *off_set, uint8_t *out_buf , uint32_t out_buf_len);
    int (*ota_set_boot)(void *something);
    int (*ota_rollback)(void *something);
    int (*ota_tag_check)(uint32_t ota_type, uint16_t ver, uint16_t rom_ver, uint32_t size);
    int (*ota_tag_get)(void *something);
    int (*ota_tag_init)(uint16_t ver, uint16_t rom_ver);
//jx debug
    int (*ota_app_checksum)(uint32_t checksum);
    void (*ota_app_mutex_init)(void);
};

/**
 * Arch register a new module before HAL startup
 */
void hal_ota_register_module(hal_ota_module_t *module);

/**
 * init ota partition
 *
 * @note   when ota start, maybe it need init something
 * @param  something  extra info for ota init
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_init(void *something);

/**
 * deinit ota partition
 *
 * @param  something  extra info for ota deinit
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_deinit(void *something);




/**
 * trigger checksum action push the temp buf to flash.
 *
 * @param  something  extra info for ota deinit
 *
 * @return  0 : On success,  1 : error occurred with any step
 */

hal_stat_t hal_ota_app_checksum(hal_ota_module_t *m, uint32_t app_checksum);



/**
 * Save data to an area on ota partition
 *
 * @param  m           Refer the ota module which will be used,default module will be used if value is NULL
 * @param  inbuf       point to the data buffer that will be written to flash
 * @param  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_save(hal_ota_module_t *m, uint8_t *in_buf , uint32_t in_buf_len);

/**
 * Read data from an area on ota Flash to data buffer in RAM
 *
 * @param  m            Refer the ota module which will be used,default module will be used if value is NULL
 * @param  off_set      Point to the start address that the data is read, and
 *                      point to the last unread address after this function is
 *                      returned, so you can call this function serval times without
 *                      update this start address.
 * @param  out_buf      Point to the data buffer that stores the data read from flash
 * @param  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set,
                        uint8_t *out_buf, uint32_t out_buf_len);

/**
 * Set boot options when ota reboot
 *
 * @param  m          Refer the ota module which will be used,default module will be used if value is NULL
 * @param  something  boot parms
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something);

/**
 * Check the validity of received tag information with local data
 * The parameter list could be altered according the implemet
 *
 * @param  m          Refer the ota module which will be used,default module will be used if value is NULL
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
hal_stat_t hal_ota_tag_check(hal_ota_module_t *m, uint32_t ota_type, uint16_t ver, uint16_t rom_ver, uint32_t size);

hal_stat_t hal_ota_tag_get(hal_ota_module_t *m, void *something);

/**
 * Get the default ota module
 *
 * @return  the first registered ota module ,which is the head of module list
 */
hal_ota_module_t *hal_ota_get_default_module(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_OTA_H */

