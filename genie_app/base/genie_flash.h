
#ifndef _GENIE_FLASH_H_
#define _GENIE_FLASH_H_

#include <bluetooth.h>

#include <api/mesh.h>
#include "net.h"
#include "flash.h"

#if 0   //next version
#define GENIE_FLASH_PARTITION_BACKUP   HAL_PARTITION_PARAMETER_1
#else
#define GENIE_FLASH_PARTITION_SEQ   HAL_PARTITION_PARAMETER_1
#endif
#define GENIE_FLASH_PARTITION_SYSTEM   HAL_PARTITION_PARAMETER_3
#define GENIE_FLASH_PARTITION_USERDATA HAL_PARTITION_CUSTOM_1
#define GENIE_FLASH_PARTITION_RECYCLE  HAL_PARTITION_CUSTOM_2

typedef enum {
    GENIE_FLASH_INDEX_START = 0,
    GFI_MESH_PARA,
    GFI_MESH_TRITUPLE,
    GFI_MESH_SADDR,
    GFI_MESH_GROUP,
    GFI_MESH_SEQ,
    GFI_MESH_RESET_CNT,
    GFI_MESH_RESET_FLAG,
    GFI_MESH_POWERUP,
} E_GENIE_FLASH_INDEX;

typedef struct
{
    uint8_t flag;
    uint8_t align;
    uint16_t index;
    uint16_t length;
    uint16_t crc;
} S_FLASH_HEADER;

typedef enum
{
    GENIE_FLASH_SUCCESS = 0,
    GENIE_FLASH_INIT_FAIL,
    GENIE_FLASH_EARSE_FAIL,
    GENIE_FLASH_DATA_INVALID,
    GENIE_FLASH_SEARCH_NONE,
    GENIE_FLASH_RECYCLE_FAIL,
    GENIE_FLASH_ERASE_FAIL,
    GENIE_FLASH_READ_FAIL,
    GENIE_FLASH_WRITE_FAIL,
    GENIE_FLASH_DELETE_FAIL,
    GENIE_FLASH_INVALID_INDEX,
    GENIE_FLASH_SIZE_ERROR,
    GENIE_FLASH_CHECK_CRC_FAIL,
} E_GENIE_FLASH_ERRCODE;

E_GENIE_FLASH_ERRCODE genie_flash_init(void);

E_GENIE_FLASH_ERRCODE genie_flash_read_reliable(uint16_t index, uint8_t *p_buff, uint16_t *p_size);
E_GENIE_FLASH_ERRCODE genie_flash_write_reliable(uint16_t index, uint8_t *p_buff, uint16_t size);
E_GENIE_FLASH_ERRCODE genie_flash_delete_reliable(uint16_t index);
E_GENIE_FLASH_ERRCODE genie_flash_read_userdata(uint16_t index, uint8_t *p_buff, uint16_t *p_size);
E_GENIE_FLASH_ERRCODE genie_flash_write_userdata(uint16_t index, uint8_t *p_buff, uint16_t size);
E_GENIE_FLASH_ERRCODE genie_flash_delete_userdata(uint16_t index);

E_GENIE_FLASH_ERRCODE genie_flash_write_trituple(uint32_t pid, uint8_t *p_mac,  uint8_t *p_key);
E_GENIE_FLASH_ERRCODE genie_flash_read_trituple(uint32_t *p_pid, uint8_t *p_mac,  uint8_t *p_key);
E_GENIE_FLASH_ERRCODE genie_flash_write_sub(uint16_t *p_sub);
E_GENIE_FLASH_ERRCODE genie_flash_read_sub(uint16_t *p_sub);
E_GENIE_FLASH_ERRCODE genie_flash_write_para(struct bt_mesh_net *mesh_data);
E_GENIE_FLASH_ERRCODE genie_flash_read_para(struct bt_mesh_net *mesh_data);
E_GENIE_FLASH_ERRCODE genie_flash_write_addr(uint16_t addr);
E_GENIE_FLASH_ERRCODE genie_flash_read_addr(uint16_t *p_addr);
E_GENIE_FLASH_ERRCODE genie_flash_write_seq(uint32_t seq);
E_GENIE_FLASH_ERRCODE genie_flash_read_seq(uint32_t *seq);

E_GENIE_FLASH_ERRCODE genie_flash_reset_system(void);
E_GENIE_FLASH_ERRCODE genie_flash_reset_userdata(void);

#endif/*--_GENIE_FLASH_H_--*/
