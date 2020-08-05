/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "genie_flash.h"
#include "flash.h"
#include <aos/aos.h>
#include "crc16.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_FLASH)
#include "common/log.h"

/***********************************************************************
reliable map
------------------------------------------------------------------------
inited_flag(4B) | reliable_data(3088B) | seq_base(4B) | seq_count(1000B)
------------------------------------------------------------------------

reliable data
------------------------------------------------------------------------
flag(1B) | align(1B) | index(2B) | len(2B) | CRC(2B) | data(nB)
------------------------------------------------------------------------
***********************************************************************/

#if 0   //next version
#define GENIE_FLASH_START_BACKUP 0
#define GENIE_FLASH_SIZE_BACKUP 0x1000

#define GENIE_FLASH_SIZE_SYSTEM 0x1000
#define GENIE_FLASH_START_INITED_FLAG 0
#define GENIE_FLASH_SIZE_INITED_FLAG 4
#define GENIE_FLASH_START_RELIABLE GENIE_FLASH_SIZE_INITED_FLAG
#define GENIE_FLASH_SIZE_RELIABLE (GENIE_FLASH_SIZE_SYSTEM-GENIE_FLASH_SIZE_SEQ_COUNT-GENIE_FLASH_SIZE_SEQ_BASE-GENIE_FLASH_SIZE_INITED_FLAG)
#define GENIE_FLASH_START_SEQ_BASE (GENIE_FLASH_START_RELIABLE+GENIE_FLASH_SIZE_RELIABLE)
#define GENIE_FLASH_SIZE_SEQ_BASE 4
#define GENIE_FLASH_START_SEQ_COUNT (GENIE_FLASH_START_SEQ_BASE+GENIE_FLASH_SIZE_SEQ_BASE)
#define GENIE_FLASH_SIZE_SEQ_COUNT 1000
#else
#define GENIE_FLASH_SIZE_SYSTEM 0x1000
#define GENIE_FLASH_START_INITED_FLAG 0
#define GENIE_FLASH_SIZE_INITED_FLAG 4
#define GENIE_FLASH_START_RELIABLE GENIE_FLASH_SIZE_INITED_FLAG
#define GENIE_FLASH_SIZE_RELIABLE (GENIE_FLASH_SIZE_SYSTEM-GENIE_FLASH_SIZE_INITED_FLAG)

#define GENIE_FLASH_SIZE_SEQ 0x1000
#define GENIE_FLASH_START_SEQ_BASE 0
#define GENIE_FLASH_SIZE_SEQ_BASE 4
#define GENIE_FLASH_START_SEQ_COUNT (GENIE_FLASH_START_SEQ_BASE+GENIE_FLASH_SIZE_SEQ_BASE)
#define GENIE_FLASH_SIZE_SEQ_COUNT (GENIE_FLASH_SIZE_SEQ-GENIE_FLASH_SIZE_SEQ_BASE)
#endif

#define GENIE_FLASH_START_USERDATA 0
#define GENIE_FLASH_SIZE_USERDATA 0x1000
                                                
#define GENIE_FLASH_START_RECYCLE 0
#define GENIE_FLASH_SIZE_RECYCLE 0x1000

#define GENIE_FLASH_FLAG_INITED 0xA5A501A8
#define GENIE_FLASH_FLAG_UNSED   0xFF
#define GENIE_FLASH_FLAG_WRITING 0x03
#define GENIE_FLASH_FLAG_ACTIVE  0x01
#define GENIE_FLASH_FLAG_INVALID 0x00
                                                
#define SEQ_COUNT_BUFFER_SIZE 4

#define ALIGN_ERROR(data) ((uint32_t)(data) & 0x00000003)

#define CELL_PREPEAR(cell, id, pn, start, size) cell.index = id;                \
                                                cell.pno = pn;                  \
                                                cell.offset = start;            \
                                                cell.end = start+size;          \
                                                memset(&cell.header, 0, sizeof(S_FLASH_HEADER));

#if 0   //next version
#define CELL_PREPEAR_BACKUP(cell, id) CELL_PREPEAR(cell, id, GENIE_FLASH_PARTITION_BACKUP,  \
                                            GENIE_FLASH_START_BACKUP, GENIE_FLASH_SIZE_BACKUP)
#endif
                                            
#define CELL_PREPEAR_RELIABLE(cell, id) CELL_PREPEAR(cell, id, GENIE_FLASH_PARTITION_SYSTEM,  \
                                            GENIE_FLASH_START_RELIABLE, GENIE_FLASH_SIZE_RELIABLE)

#define CELL_PREPEAR_USERDATA(cell, id) CELL_PREPEAR(cell, id, GENIE_FLASH_PARTITION_USERDATA,  \
                                            GENIE_FLASH_START_USERDATA, GENIE_FLASH_SIZE_USERDATA)

#define CELL_PREPEAR_RECYCLE(cell) CELL_PREPEAR(cell, 0, GENIE_FLASH_PARTITION_RECYCLE,  \
                                            GENIE_FLASH_START_RECYCLE, GENIE_FLASH_SIZE_RECYCLE)

typedef struct{
    uint16_t index;
    hal_partition_t pno;
    uint32_t offset;
    uint32_t end;
    S_FLASH_HEADER header;
} S_GENIE_FLASH_CELL;

typedef struct{
    uint16_t remain;
    uint32_t free_offset;
} S_GENIE_FLASH_INFO;

static S_GENIE_FLASH_INFO g_info_system;
static S_GENIE_FLASH_INFO g_info_userdata;
static S_GENIE_FLASH_INFO g_info_recycle;

#define CHECK_RET(ret) { if(ret != 0) BT_ERR("error ret(%d)", ret); }
#if(BT_DBG_ENABLED == 1)
#define RETURN_WHEN_ERR(result, err_code) { if(result != 0) { BT_ERR("%s[%d] error ret(%d)", __func__, __LINE__, result); return err_code; } }
#else
#define RETURN_WHEN_ERR(result, err_code) { if(result != 0) return err_code; }
#endif

static uint16_t _genie_get_crc(uint8_t *p_buff, uint16_t size)
{
    uint16_t crc;
    crc = util_crc16_compute(p_buff, size, NULL);
    BT_DBG("crc(0x%02X)", crc);
    return crc;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_encrypt(uint8_t *p_buff, uint16_t size)
{
    return 0;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_decrypt(uint8_t *p_buff, uint16_t size)
{
    return 0;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_checkflag(void)
{
    int32_t ret = 0;
    uint32_t offset = 0;
    uint32_t flash_flag = 0;

    ret = hal_flash_read(GENIE_FLASH_PARTITION_SYSTEM, &offset, (uint8_t *)(&flash_flag), sizeof(flash_flag));
    BT_DBG("%s ret(%d) flash_flag(0x%08X)", __func__, ret, flash_flag);

    RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);

    if(flash_flag == GENIE_FLASH_FLAG_INITED) {
        return GENIE_FLASH_SUCCESS;
    } else {
        return GENIE_FLASH_DATA_INVALID;
    }
}

static E_GENIE_FLASH_ERRCODE _genie_flash_erase_reliable(void)
{
    int32_t ret;
    uint32_t offset = 0;
    uint32_t flash_flag = GENIE_FLASH_FLAG_INITED;

    BT_DBG("");
    ret = hal_flash_erase(GENIE_FLASH_PARTITION_SYSTEM, 0, GENIE_FLASH_SIZE_SYSTEM);
    ret |= hal_flash_write(GENIE_FLASH_PARTITION_SYSTEM, &offset, &flash_flag, sizeof(flash_flag));

    RETURN_WHEN_ERR(ret, GENIE_FLASH_EARSE_FAIL);

    return GENIE_FLASH_SUCCESS;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_erase(void)
{
    int32_t ret;
    uint32_t offset = 0;

    BT_DBG("%s", __func__);
#if 0   //next version
    ret = hal_flash_erase(GENIE_FLASH_PARTITION_BACKUP, 0, GENIE_FLASH_SIZE_BACKUP);
#else
    ret = hal_flash_erase(GENIE_FLASH_PARTITION_SEQ, 0, GENIE_FLASH_SIZE_SEQ);
#endif
    ret |= hal_flash_erase(GENIE_FLASH_PARTITION_USERDATA, 0, GENIE_FLASH_SIZE_USERDATA);
    ret |= _genie_flash_erase_reliable();

    RETURN_WHEN_ERR(ret, GENIE_FLASH_EARSE_FAIL);

    return _genie_flash_erase_reliable();
}

static E_GENIE_FLASH_ERRCODE _genie_flash_get_header(S_GENIE_FLASH_CELL *p_cell)
{
    int32_t ret = 0;
    uint32_t flag_offset = 0xFFFFFFFF;

    //BT_DBG("");

    if(p_cell->offset < p_cell->end) {

        ret = hal_flash_read(p_cell->pno, &p_cell->offset, (uint8_t *)(&p_cell->header),
                                sizeof(S_FLASH_HEADER));

        RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);
#if 0
        BT_DBG("offset(0x%04X) flag(0x%02X) align(%d) index(0x%04X) length(%d) crc(0x%04X)",
                p_cell->offset-sizeof(S_FLASH_HEADER), p_cell->header.flag, p_cell->header.align,
                p_cell->header.index, p_cell->header.length, p_cell->header.crc);
#endif
        /* move offset to header*/
        p_cell->offset -= sizeof(S_FLASH_HEADER);

        if(p_cell->header.flag != GENIE_FLASH_FLAG_UNSED) {
            return GENIE_FLASH_SUCCESS;
        }
    }
    return GENIE_FLASH_SEARCH_NONE;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_check_remain(void)
{
    E_GENIE_FLASH_ERRCODE ret;
    S_GENIE_FLASH_CELL cell;

#if 0   //next version
    /* backup */
    g_info_backup.remain = GENIE_FLASH_SIZE_BACKUP;
    CELL_PREPEAR_BACKUP(cell, 0);
    do {
        ret = _genie_flash_get_header(&cell);
        if(ret == GENIE_FLASH_SUCCESS) {
            if(cell.header.flag == GENIE_FLASH_FLAG_ACTIVE) {
                g_info_backup.remain -= sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
            }
            cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
        }
    } while(ret == GENIE_FLASH_SUCCESS);
    if(g_info_backup.remain > GENIE_FLASH_SIZE_BACKUP) {
        g_info_backup.remain = 0;
    }
    if(cell.offset <= GENIE_FLASH_SIZE_BACKUP) {
        g_info_backup.free_offset = cell.offset;
    }
#endif
    /* system */
    g_info_system.remain = GENIE_FLASH_SIZE_RELIABLE;
    CELL_PREPEAR_RELIABLE(cell, 0);
    do {
        ret = _genie_flash_get_header(&cell);
        if(ret == GENIE_FLASH_SUCCESS) {
            if(cell.header.flag == GENIE_FLASH_FLAG_ACTIVE) {
                g_info_system.remain -= sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
            }
            cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
        }
    } while(ret == GENIE_FLASH_SUCCESS);
    if(g_info_system.remain > GENIE_FLASH_SIZE_RELIABLE) {
        g_info_system.remain = 0;
    }
    if(cell.offset <= GENIE_FLASH_SIZE_RELIABLE) {
        g_info_system.free_offset = cell.offset;
    }

    /* userdata */
    g_info_userdata.remain = GENIE_FLASH_SIZE_USERDATA;
    CELL_PREPEAR_USERDATA(cell, 0);
    do {
        ret = _genie_flash_get_header(&cell);
        if(ret == GENIE_FLASH_SUCCESS) {
            if(cell.header.flag == GENIE_FLASH_FLAG_ACTIVE) {
                g_info_userdata.remain -= sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
            }
            cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
        }
    } while(ret == GENIE_FLASH_SUCCESS);
    if(g_info_userdata.remain > GENIE_FLASH_SIZE_USERDATA) {
        g_info_userdata.remain = 0;
    }
    if(cell.offset <= GENIE_FLASH_SIZE_USERDATA) {
        g_info_userdata.free_offset = cell.offset;
    }

    g_info_recycle.remain = GENIE_FLASH_SIZE_RECYCLE;
    g_info_recycle.free_offset = GENIE_FLASH_START_RECYCLE;
    BT_DBG("ramain sys(%d) ud(%d)", g_info_system.remain, g_info_userdata.remain);

    return GENIE_FLASH_SUCCESS;
}


E_GENIE_FLASH_ERRCODE genie_flash_init(void)
{
    E_GENIE_FLASH_ERRCODE ret = _genie_flash_checkflag();

    if(ret != GENIE_FLASH_SUCCESS) {
        ret = _genie_flash_erase();
        ret |= _genie_flash_checkflag();
    }

    RETURN_WHEN_ERR(ret, GENIE_FLASH_INIT_FAIL);

    return _genie_flash_check_remain();
}

static E_GENIE_FLASH_ERRCODE _genie_flash_search(S_GENIE_FLASH_CELL *p_cell)
{
    int32_t ret = 0;

    BT_DBG("find index(0x%04X)", p_cell->index);

    do {
        ret = _genie_flash_get_header(p_cell);

        if(p_cell->header.flag == GENIE_FLASH_FLAG_ACTIVE && p_cell->header.index == p_cell->index) {
            BT_DBG("bingo");
            return GENIE_FLASH_SUCCESS;
        } else {
            /* found next cell */
            p_cell->offset += sizeof(S_FLASH_HEADER) + p_cell->header.length + p_cell->header.align;
        }
    } while(ret == GENIE_FLASH_SUCCESS);

    //BT_ERR("can not find index(0x%04X), ret(%d)", p_cell->index, ret);
    return ret;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_read(S_GENIE_FLASH_CELL *p_cell, uint8_t *p_buff, uint16_t *p_size)
{
    E_GENIE_FLASH_ERRCODE ret;

    ret = _genie_flash_search(p_cell);
    RETURN_WHEN_ERR(ret, ret);

    if(p_cell->header.length < *p_size || *p_size == 0) {
        *p_size = p_cell->header.length;
    }

    /* move offset */
    p_cell->offset += sizeof(S_FLASH_HEADER);
    
    ret = hal_flash_read(p_cell->pno, &p_cell->offset, p_buff, *p_size);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);
    
    if(_genie_get_crc(p_buff, *p_size) == p_cell->header.crc) {
        return GENIE_FLASH_SUCCESS;
    } else {
        BT_ERR("check crc fail");
        ret = GENIE_FLASH_CHECK_CRC_FAIL;
    }
}

static E_GENIE_FLASH_ERRCODE _genie_flash_copy(S_GENIE_FLASH_CELL *p_src, S_GENIE_FLASH_CELL *p_dst)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint8_t *p_data;
    uint32_t flag_offset = 0xFFFFFFFF;
    uint32_t seq = 0xFFFFFFFF;
    uint16_t size = 0x1000;

    /* step 1, erase destination */
    BT_DBG("1.1 erase pno(%d)", p_dst->pno);
    if(p_dst->pno == GENIE_FLASH_PARTITION_SYSTEM) {
        _genie_flash_erase_reliable();
        p_dst->offset = 4;
    } else {
        hal_flash_erase(p_dst->pno, 0, size);
    }
    

    if(p_src->pno == GENIE_FLASH_PARTITION_SYSTEM) {
        p_src->offset = 4;
    } else {
        p_src->offset = 0;
    }
    BT_DBG("1.2 src(0x%04X) dst(0x%04X)", p_src->offset, p_dst->offset);
    
    /* step 2, copy data to destination */
    do {
        ret = _genie_flash_get_header(p_src);
        
        if(ret == GENIE_FLASH_SUCCESS && p_src->header.flag == GENIE_FLASH_FLAG_ACTIVE) {
            /* step 2.1 copy new header */
            BT_DBG("2.1 copy header");
            memcpy(&p_dst->header, &p_src->header, sizeof(S_FLASH_HEADER));
        
            /* step 2.2 write header */
            BT_DBG("2.2 src(0x%04X) dst(0x%04X)", p_src->offset, p_dst->offset);
            flag_offset = (uint32_t)&p_dst->header.flag - (uint32_t)&p_dst->header + p_dst->offset;
            p_dst->header.flag = GENIE_FLASH_FLAG_WRITING;
            ret = hal_flash_write(p_dst->pno, &p_dst->offset, &p_dst->header, sizeof(S_FLASH_HEADER));
            RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

            /* step 2.3 read data */
            BT_DBG("2.3 read data");
            p_src->offset += sizeof(S_FLASH_HEADER);
            p_data = k_malloc(p_src->header.length);
            if(p_data == NULL) {
                BT_ERR("p_data is null!!!");
                return GENIE_FLASH_DATA_INVALID;
            }
            ret = hal_flash_read(p_src->pno, &p_src->offset, p_data, p_src->header.length);

            /* step 2.4 write data */
            BT_DBG("2.4 write data(0x%08X) dst(0x%04X)", p_data, p_dst->offset);
            ret = hal_flash_write(p_dst->pno, &p_dst->offset, p_data, p_dst->header.length);
            aos_free(p_data);
            RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

            /* step 2.5 update new flag */
            BT_DBG("2.5 update flag(0x%04X)", flag_offset);
            p_dst->header.flag = GENIE_FLASH_FLAG_ACTIVE;
            ret = hal_flash_write(p_dst->pno, &flag_offset, &p_dst->header.flag, 1);
            RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
            
            p_src->offset += p_src->header.align;
            p_dst->offset += p_dst->header.align;
        } else {
            p_src->offset += sizeof(S_FLASH_HEADER) + p_src->header.length + p_src->header.align;
        }
        
        /* read new data */
        ret = _genie_flash_get_header(p_src);
    } while(ret == GENIE_FLASH_SUCCESS);
    return GENIE_FLASH_SUCCESS;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_recycle(S_GENIE_FLASH_CELL *p_cell)
{
    S_GENIE_FLASH_CELL recycle;

    CELL_PREPEAR_RECYCLE(recycle);
    
    _genie_flash_copy(p_cell, &recycle);

    _genie_flash_copy(&recycle, p_cell);
}

static E_GENIE_FLASH_ERRCODE _genie_flash_delete_flag(hal_partition_t pno, uint32_t *p_offset)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint8_t flag = GENIE_FLASH_FLAG_INVALID;

    BT_DBG("pno(%d) offset(0x%04X)", pno, *p_offset);
    ret = hal_flash_write(pno, p_offset, &flag, sizeof(flag));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
    return GENIE_FLASH_SUCCESS;
}

static E_GENIE_FLASH_ERRCODE _genie_flash_delete(S_GENIE_FLASH_CELL *p_cell)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t flag_offset;

    ret = _genie_flash_search(p_cell);
    RETURN_WHEN_ERR(ret, ret);

    flag_offset = (uint32_t)&p_cell->header.flag - (uint32_t)&p_cell->header + p_cell->offset;
    if(flag_offset < p_cell->end) {
        return _genie_flash_delete_flag(p_cell->pno, &flag_offset);
    } else {
        BT_ERR("flag_offset error");
        return GENIE_FLASH_DELETE_FAIL;
    }
}

#if 0   //next version
static E_GENIE_FLASH_ERRCODE _genie_flash_write(S_GENIE_FLASH_CELL *p_cell, uint8_t *p_buff, uint16_t size)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint8_t align_count = 0;
    S_GENIE_FLASH_CELL old_cell;
    uint32_t old_flag_offset = 0xFFFFFFFF;
    uint32_t new_flag_offset = 0xFFFFFFFF;
    S_GENIE_FLASH_INFO *p_info;
    uint16_t end;
    uint8_t replace = 0;
    uint8_t recycle = 0;

    if(size == 0) {
        BT_ERR("size is zero!!!");
        return GENIE_FLASH_WRITE_FAIL;
    }

    /* get info */
    switch(p_cell->pno) {
        case GENIE_FLASH_PARTITION_BACKUP:
            p_info = &g_info_backup;
            end = GENIE_FLASH_START_BACKUP + GENIE_FLASH_SIZE_BACKUP;
            break;
        case GENIE_FLASH_PARTITION_SYSTEM:
            p_info = &g_info_system;
            end = GENIE_FLASH_START_RELIABLE + GENIE_FLASH_SIZE_RELIABLE;
            break;
        case GENIE_FLASH_PARTITION_USERDATA:
            p_info = &g_info_userdata;
            end = GENIE_FLASH_START_USERDATA + GENIE_FLASH_SIZE_USERDATA;
            break;
        case GENIE_FLASH_PARTITION_RECYCLE:
            p_info = &g_info_recycle;
            end = GENIE_FLASH_START_RECYCLE + GENIE_FLASH_SIZE_RECYCLE;
            break;
        default:
            BT_ERR("pno error!!!");
            return GENIE_FLASH_WRITE_FAIL;
    }

    /* check replace */
    if(_genie_flash_search(p_cell) == GENIE_FLASH_SUCCESS) {
        old_flag_offset = (uint32_t)&p_cell->header.flag - (uint32_t)&p_cell->header + p_cell->offset;
        memcpy(&old_cell, p_cell, sizeof(S_GENIE_FLASH_CELL));
        replace = 1;
    }

    /* check size */
    if(replace = 1) {
        if(p_info->remain - old_cell.header.length - old_cell.header.align < size) {
            BT_ERR("no memory");
            return GENIE_FLASH_WRITE_FAIL;
        }
    } else {
        if(p_info->remain < size + sizeof(S_FLASH_HEADER)) {
            BT_ERR("no memory");
            return GENIE_FLASH_WRITE_FAIL;
        }
    }
    
    /* check recycle */
    if(p_info->free_offset + size + sizeof(S_FLASH_HEADER) > end) {
        recycle = 1;
    }

    if(recycle == 1) {
        //TODO
    }

    //step 1 write new data
    p_cell->offset = new_flag_offset = p_info->free_offset;
    BT_DBG("new_flag_offset(0x%08X) cell.offset(0x%08X)", new_flag_offset, p_cell->offset);

    if(ALIGN_ERROR(size)) {
        align_count = 4 - (size & 0x0003);
    }
    p_cell->header.flag = GENIE_FLASH_FLAG_WRITING;
    p_cell->header.align = align_count;
    p_cell->header.index = p_cell->index;
    p_cell->header.length = size;
    p_cell->header.crc = _genie_get_crc(p_buff, size);

    BT_DBG("step1.1 cell.offset(0x%08X)", p_cell->offset);
    ret = hal_flash_write(p_cell->pno, &p_cell->offset, &p_cell->header, sizeof(S_FLASH_HEADER));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    BT_DBG("step1.2 cell.offset(0x%08X)", p_cell->offset);
    ret = hal_flash_write(p_cell->pno, &p_cell->offset, p_buff, size);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    //step 2 update new flag
    p_cell->header.flag = GENIE_FLASH_FLAG_ACTIVE;
    BT_DBG("step2 cell.offset(0x%08X)", p_cell->offset);
    ret = hal_flash_write(p_cell->pno, &new_flag_offset, &p_cell->header.flag, 1);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    //modify old flag
    if(old_flag_offset != 0xFFFFFFFF) {
        p_cell->header.flag = GENIE_FLASH_FLAG_INVALID;
        BT_DBG("old offset(0x%08X)", old_flag_offset);
        ret = hal_flash_write(p_cell->pno, &old_flag_offset, &p_cell->header.flag, 1);
        RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
    }

    return GENIE_FLASH_SUCCESS;
}
#else
static E_GENIE_FLASH_ERRCODE _genie_flash_write(S_GENIE_FLASH_CELL *p_cell, uint8_t *p_buff, uint16_t size)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint8_t align_count = 0;
    S_GENIE_FLASH_INFO *p_info;
    uint32_t old_flag_offset = 0xFFFFFFFF;
    uint32_t new_flag_offset = 0xFFFFFFFF;
    uint16_t old_size;
    uint16_t end;
    S_GENIE_FLASH_CELL recycle;

    if(size == 0) {
        BT_ERR("size is zero!!!");
        return GENIE_FLASH_WRITE_FAIL;
    }

    if(p_buff == NULL) {
        BT_ERR("p_buff is null!!!");
        return GENIE_FLASH_DATA_INVALID;
    }
    
    /* get info */
    switch(p_cell->pno) {
        case GENIE_FLASH_PARTITION_SYSTEM:
            p_info = &g_info_system;
            end = GENIE_FLASH_START_RELIABLE + GENIE_FLASH_SIZE_RELIABLE;
            break;
        case GENIE_FLASH_PARTITION_USERDATA:
            p_info = &g_info_userdata;
            end = GENIE_FLASH_START_USERDATA + GENIE_FLASH_SIZE_USERDATA;
            break;
        default:
            BT_ERR("pno error!!!");
            return GENIE_FLASH_WRITE_FAIL;
    }

    /* check size */
    if(p_info->remain < size + sizeof(S_FLASH_HEADER)) {
        BT_ERR("no memory");
        return GENIE_FLASH_WRITE_FAIL;
    }

    /* check recycle */
    if(p_info->free_offset + size + sizeof(S_FLASH_HEADER) > end) {
        memcpy(&recycle, p_cell, sizeof(S_GENIE_FLASH_CELL));
        ret = _genie_flash_recycle(&recycle);
        RETURN_WHEN_ERR(ret, ret);
        _genie_flash_check_remain();
    }

    /* find old */
    if(_genie_flash_search(p_cell) == GENIE_FLASH_SUCCESS) {
        old_flag_offset = (uint32_t)&p_cell->header.flag - (uint32_t)&p_cell->header + p_cell->offset;
        old_size = sizeof(S_FLASH_HEADER) +p_cell->header.length + p_cell->header.align;
    }

    //step 1 write new data
    p_cell->offset = new_flag_offset = p_info->free_offset;
    BT_DBG("new_flag_offset(0x%04X) cell.offset(0x%04X)", new_flag_offset, p_cell->offset);

    if(ALIGN_ERROR(size)) {
        align_count = 4 - (size & 0x0003);
    }
    p_cell->header.flag = GENIE_FLASH_FLAG_WRITING;
    p_cell->header.align = align_count;
    p_cell->header.index = p_cell->index;
    p_cell->header.length = size;
    p_cell->header.crc = _genie_get_crc(p_buff, size);

    BT_DBG("step1.1 write header offset(0x%04X)", p_cell->offset);
    ret = hal_flash_write(p_cell->pno, &p_cell->offset, &p_cell->header, sizeof(S_FLASH_HEADER));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    BT_DBG("step1.2 write data offset(0x%04X)", p_cell->offset);
    ret = hal_flash_write(p_cell->pno, &p_cell->offset, p_buff, size);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    //step 2 update new flag
    p_cell->header.flag = GENIE_FLASH_FLAG_ACTIVE;
    BT_DBG("step2 update flag offset(0x%04X)", new_flag_offset);
    ret = hal_flash_write(p_cell->pno, &new_flag_offset, &p_cell->header.flag, 1);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    /* update remain & free*/
    p_info->remain -= size + sizeof(S_FLASH_HEADER) + p_cell->header.align;
    p_info->free_offset = p_cell->offset + p_cell->header.align;

    //modify old flag
    if(old_flag_offset != 0xFFFFFFFF) {
        BT_DBG("old offset(0x%04X)", old_flag_offset);
        ret = _genie_flash_delete_flag(p_cell->pno, &old_flag_offset);
        RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
        p_info->remain += old_size;
    }

    return GENIE_FLASH_SUCCESS;
}
#endif

E_GENIE_FLASH_ERRCODE genie_flash_read_reliable(uint16_t index, uint8_t *p_buff, uint16_t *p_size)
{
    E_GENIE_FLASH_ERRCODE ret;
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_RELIABLE(cell, index);

    ret = _genie_flash_read(&cell, p_buff, p_size);
    RETURN_WHEN_ERR(ret, ret);

    return _genie_flash_decrypt(p_buff, *p_size);
}

E_GENIE_FLASH_ERRCODE genie_flash_write_reliable(uint16_t index, uint8_t *p_buff, uint16_t size)
{
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_RELIABLE(cell, index);

    _genie_flash_encrypt(p_buff, size);

    return _genie_flash_write(&cell, p_buff, size);
}

E_GENIE_FLASH_ERRCODE genie_flash_delete_reliable(uint16_t index)
{
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_RELIABLE(cell, index);

    return _genie_flash_delete(&cell);
}

E_GENIE_FLASH_ERRCODE genie_flash_read_userdata(uint16_t index, uint8_t *p_buff, uint16_t *p_size)
{
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_USERDATA(cell, index);

    return _genie_flash_read(&cell, p_buff, p_size);
}

E_GENIE_FLASH_ERRCODE genie_flash_write_userdata(uint16_t index, uint8_t *p_buff, uint16_t size)
{
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_USERDATA(cell, index);

    return _genie_flash_write(&cell, p_buff, size);
}

E_GENIE_FLASH_ERRCODE genie_flash_delete_userdata(uint16_t index)
{
    S_GENIE_FLASH_CELL cell;

    CELL_PREPEAR_USERDATA(cell, index);

    return _genie_flash_delete(&cell);
}

E_GENIE_FLASH_ERRCODE genie_flash_write_sub(uint16_t *p_sub)
{
    E_GENIE_FLASH_ERRCODE ret;

    BT_DBG("0x%04x 0x%04x 0x%04x 0x%04x", p_sub[0], p_sub[1], p_sub[2], p_sub[3]);

    ret = genie_flash_write_reliable(GFI_MESH_GROUP, p_sub, CONFIG_BT_MESH_MODEL_GROUP_COUNT<<1);

    return ret;
}

E_GENIE_FLASH_ERRCODE genie_flash_read_sub(uint16_t *p_sub)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint16_t group_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT];
    uint16_t size = sizeof(group_list);

    BT_DBG("");

    ret = genie_flash_read_reliable(GFI_MESH_GROUP, group_list, &size);

    if(ret == GENIE_FLASH_SUCCESS && size == sizeof(group_list)) {
        memcpy(p_sub, group_list, size);
        return GENIE_FLASH_SUCCESS;
    } else {
        BT_ERR("read size error");
        return GENIE_FLASH_READ_FAIL;
    }
}

E_GENIE_FLASH_ERRCODE genie_flash_write_para(struct bt_mesh_net *mesh_data)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint16_t data_size = sizeof(struct bt_mesh_net);
    uint8_t *p_data = k_malloc(data_size);

    //print_para(mesh_data);
    BT_DBG("size(%d) data(0x%08X)", data_size, p_data);

    memcpy(p_data, mesh_data, data_size);
    ret = genie_flash_write_reliable(GFI_MESH_PARA, p_data, data_size);
    aos_free(p_data);

    return ret;
}

E_GENIE_FLASH_ERRCODE genie_flash_read_para(struct bt_mesh_net *mesh_data)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint16_t data_size = sizeof(struct bt_mesh_net);
    uint16_t size = data_size;
    uint8_t *p_data = k_malloc(data_size);

    BT_DBG("%d", data_size);

    ret = genie_flash_read_reliable(GFI_MESH_PARA, p_data, &size);

    if(ret == GENIE_FLASH_SUCCESS) {
        if(size == data_size) {
            memcpy(mesh_data, p_data, size);
        } else {
            BT_ERR("read size error");
            ret = GENIE_FLASH_READ_FAIL;
        }
    }
    aos_free(p_data);
    //print_para(mesh_data);

    return ret;
}

E_GENIE_FLASH_ERRCODE genie_flash_write_addr(uint16_t addr)
{
    uint16_t data = addr;

    BT_DBG("0x%04x", addr);

    return genie_flash_write_reliable(GFI_MESH_SADDR, &data, 2);
}

E_GENIE_FLASH_ERRCODE genie_flash_read_addr(uint16_t *p_addr)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint16_t size = 2;
    uint16_t data;

    BT_DBG("");

    ret = genie_flash_read_reliable(GFI_MESH_SADDR, &data, &size);
    RETURN_WHEN_ERR(ret, ret);

    if(size == 2) {
        *p_addr = data;
        return GENIE_FLASH_SUCCESS;
    } else {
        BT_ERR("read size error");
        return GENIE_FLASH_READ_FAIL;
    }
}

static E_GENIE_FLASH_ERRCODE _genie_flash_read_seqbase(uint32_t *p_base)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t offset = GENIE_FLASH_START_SEQ_BASE;
    uint8_t flag;

    BT_DBG("");

    ret = hal_flash_read(GENIE_FLASH_PARTITION_SEQ, &offset, p_base, sizeof(uint32_t));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);

    BT_DBG("read raw base(0x%02X)", *p_base);
    flag = *p_base >> 24;

    if(flag != GENIE_FLASH_FLAG_ACTIVE) {
        BT_DBG("base seq is invalid(0x%02X)", flag);
        return GENIE_FLASH_DATA_INVALID;
    }

    *p_base &= 0x00FFFFFF;
    return GENIE_FLASH_SUCCESS;
}


static E_GENIE_FLASH_ERRCODE _genie_flash_read_seqcount(uint16_t *p_count)
{
    int32_t ret;
    uint32_t offset = GENIE_FLASH_START_SEQ_COUNT;
    uint8_t data[SEQ_COUNT_BUFFER_SIZE];
    uint8_t i, j;

    *p_count = 0;
    while(offset < GENIE_FLASH_START_SEQ_COUNT + GENIE_FLASH_SIZE_SEQ_COUNT) {
        ret = hal_flash_read(GENIE_FLASH_PARTITION_SEQ, &offset, data, SEQ_COUNT_BUFFER_SIZE);
        RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);
        for(i = 0; i < SEQ_COUNT_BUFFER_SIZE; i++) {
            //BT_DBG("read byte(0x%02X)", data[i])
            if(data[i] != 0) {
                //do sth then break
                for(j = 0; j < 8; j++) {
                    //BT_DBG("0x%02X 0x%02X 0x%02X", data[i], 0x01<<j, (data[i] & (0x01<<j)))
                    if((data[i] & (0x01<<j)) == 0) {
                        (*p_count)++;
                    }
                }
                //BT_DBG("get p_count(0x%02X)", *p_count)
                return GENIE_FLASH_SUCCESS;
            } else {
                *p_count += 8;
            }
        }
    }
    return GENIE_FLASH_SUCCESS;
}

E_GENIE_FLASH_ERRCODE genie_flash_read_seq(uint32_t *seq)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t base;
    uint16_t count;

    BT_DBG("");

    ret = _genie_flash_read_seqbase(&base);
    RETURN_WHEN_ERR(ret, ret);
    BT_DBG("read base(0x%02X)", base);

    ret = _genie_flash_read_seqcount(&count);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);
    BT_DBG("read count(0x%02X)", count);

    *seq = base+count;
    return GENIE_FLASH_SUCCESS;
}

#if 0   //next version
static E_GENIE_FLASH_ERRCODE _genie_flash_delete_seq(void)
{
    int32_t ret;
    uint32_t seq = 0;
    uint32_t offset = GENIE_FLASH_START_SEQ_BASE;

    BT_DBG("del seq\n");
    if(hal_flash_write(GENIE_FLASH_PARTITION_SYSTEM, &offset, &seq, sizeof(seq)) == 0) {
        return GENIE_FLASH_SUCCESS;
    } else {
        return GENIE_FLASH_DELETE_FAIL;
    }
}
#else
static E_GENIE_FLASH_ERRCODE _genie_flash_delete_seq(void)
{
    int32_t ret;
    BT_DBG("");
    ret = hal_flash_erase(GENIE_FLASH_PARTITION_SEQ, 0, GENIE_FLASH_SIZE_SEQ);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_DELETE_FAIL);
    
    return GENIE_FLASH_SUCCESS;
}
#endif

static E_GENIE_FLASH_ERRCODE _genie_flash_write_seqbase(uint32_t base)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t offset = GENIE_FLASH_START_SEQ_BASE;

    base |= GENIE_FLASH_FLAG_ACTIVE<<24;
    BT_DBG("base(0x%04X)", base);

    //_genie_flash_delete_seq();
    
    ret = hal_flash_write(GENIE_FLASH_PARTITION_SEQ, &offset, &base, sizeof(base));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);

    return GENIE_FLASH_SUCCESS;
}


static E_GENIE_FLASH_ERRCODE _genie_flash_write_seqcount(uint16_t count)
{
    int32_t ret;
    uint32_t offset = GENIE_FLASH_START_SEQ_COUNT + (count>>3);
    uint8_t write_byte = count % 8, zero = 0;
    uint32_t start = GENIE_FLASH_START_SEQ_COUNT;

    if(count && write_byte == 0) {
        offset -= 1;
    } else {
        write_byte = 0xFF >> write_byte;
    }

    BT_DBG("offset(0x%04X) byte(0x%02X)", offset, write_byte);

    //TODO
    //clear all bytes before offset
    while(start < offset) {
        ret = hal_flash_read(GENIE_FLASH_PARTITION_SEQ, &start, &zero, sizeof(zero));
        RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
        if(zero != 0) {
            zero = 0;
            start--;
            ret = hal_flash_write(GENIE_FLASH_PARTITION_SEQ, &start, &zero, sizeof(zero));
            RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
        }
    }

    ret = hal_flash_write(GENIE_FLASH_PARTITION_SEQ, &offset, &write_byte, sizeof(write_byte));
    RETURN_WHEN_ERR(ret, GENIE_FLASH_WRITE_FAIL);
    return GENIE_FLASH_SUCCESS;
}

E_GENIE_FLASH_ERRCODE genie_flash_write_seq(uint32_t seq)
{
    uint32_t base;
    int32_t ret;
    uint32_t offset = GENIE_FLASH_START_SEQ_BASE;
    uint16_t count = 0;

    BT_DBG("%s %d", __func__, seq);

    ret = _genie_flash_read_seqbase(&base);

    if(ret == GENIE_FLASH_DATA_INVALID) {
        if(base != 0xFFFFFFFF) {
            goto CLEAN;
        }
        goto WRITE_BASE;
    }

    if(ret == 0) {
        if(base >= seq) {
            BT_ERR("base(%d) > seq(%d)", base, seq);
            return GENIE_FLASH_WRITE_FAIL;
        } else {
            if(seq-base <= (GENIE_FLASH_SIZE_SEQ_COUNT<<3)) {
                //write new count
                ret = _genie_flash_read_seqcount(&count);
                RETURN_WHEN_ERR(ret, GENIE_FLASH_READ_FAIL);
                BT_DBG("read count(%d)", count);
                
                if(count < seq - base) {
                    count = seq - base;
                    return _genie_flash_write_seqcount(count);
                } else {
                    BT_ERR("base(%d)+count(%d) >= seq(%d)", base, count, seq);
                    return GENIE_FLASH_WRITE_FAIL;
                }
            } else {
                goto CLEAN;
            }
        }
    } else {
        BT_ERR("flash error");
        return GENIE_FLASH_WRITE_FAIL;
    }

CLEAN:
    BT_DBG("del seq\n");
    _genie_flash_delete_seq();
WRITE_BASE:
    return _genie_flash_write_seqbase(seq);
}

#if 0   //next version
E_GENIE_FLASH_ERRCODE genie_system_reset(void)
{
    S_GENIE_FLASH_CELL cell;
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t flag_offset = 0xFFFFFFFF;

    CELL_PREPEAR_RELIABLE(cell, GFI_MESH_PARA);

    ret = _genie_flash_get_header(&cell);
    /* delete */
    while(ret == GENIE_FLASH_SUCCESS) {
        if(cell.header.flag == GENIE_FLASH_FLAG_ACTIVE && cell.header.index != cell.index) {
            cell.header.flag = GENIE_FLASH_FLAG_ACTIVE;
            flag_offset = (uint32_t)&cell.header.flag - (uint32_t)&cell.header + cell.offset;
            ret = hal_flash_write(cell.pno, &flag_offset, &cell.header.flag, 1);
            if(ret != 0) {
                BT_ERR("clear flag failed");
                return GENIE_FLASH_WRITE_FAIL;
            }
        }
        cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
        
        /* read new data */
        ret = _genie_flash_get_header(&cell);
    }

    _genie_flash_delete_seq();
}
#else
E_GENIE_FLASH_ERRCODE genie_flash_reset_system(void)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint32_t pid = 0;
    uint8_t mac[6];
    uint8_t key[32];

    /* read tri truple */
    ret = genie_flash_read_trituple(&pid, mac, key);
    if(ret == GENIE_FLASH_SUCCESS || ret == GENIE_FLASH_SEARCH_NONE) {
        /* erase system */
        ret = hal_flash_erase(GENIE_FLASH_PARTITION_SEQ, 0, GENIE_FLASH_SIZE_SEQ);
        ret |= _genie_flash_erase_reliable();
        RETURN_WHEN_ERR(ret, ret);
        _genie_flash_check_remain();

        /* write tri truple */
        if(pid != 0) {
            ret = genie_flash_write_trituple(pid, mac, key);
            RETURN_WHEN_ERR(ret, ret);
        }

        /* delete seq */
        return _genie_flash_delete_seq();
    }
    RETURN_WHEN_ERR(ret, ret);
}
#endif

E_GENIE_FLASH_ERRCODE genie_flash_reset_userdata(void)
{
    E_GENIE_FLASH_ERRCODE ret;

    ret = hal_flash_erase(GENIE_FLASH_PARTITION_USERDATA, 0, GENIE_FLASH_SIZE_USERDATA);
    RETURN_WHEN_ERR(ret, GENIE_FLASH_EARSE_FAIL);
    
    _genie_flash_check_remain();
    
    return GENIE_FLASH_SUCCESS;
}

#if defined(CONFIG_GENIE_DEBUG_CMD_FLASH)
E_GENIE_FLASH_ERRCODE genie_flash_delete_seq(void)
{
    return _genie_flash_delete_seq();
}

E_GENIE_FLASH_ERRCODE genie_flash_write_data(hal_partition_t pno, uint16_t size)
{
    E_GENIE_FLASH_ERRCODE ret;
    printk("malloc size(%d)\n", size);
    uint8_t *p_data = k_malloc(size);
    
    printk("set data(0x%08X)\n", p_data);
    if(pno == GENIE_FLASH_PARTITION_SYSTEM) {
        ret = genie_flash_write_reliable(GFI_MESH_PARA, p_data, size);
    } else {
        ret = genie_flash_write_userdata(GFI_MESH_POWERUP, p_data, size);
    }
    aos_free(p_data);
    return ret;
}

void genie_print_data(hal_partition_t pno)
{
    E_GENIE_FLASH_ERRCODE ret;
    S_GENIE_FLASH_CELL cell;

    if(pno == GENIE_FLASH_PARTITION_SYSTEM) {
        CELL_PREPEAR_RELIABLE(cell, 0);
    } else {
        CELL_PREPEAR_USERDATA(cell, 0);
    }

    printk(F_RED "offset\t flag\t align\t index\t length\t\t crc" F_END);
    do {
        ret = _genie_flash_get_header(&cell);

        if(ret != GENIE_FLASH_SEARCH_NONE) {
            if(cell.header.flag == GENIE_FLASH_FLAG_ACTIVE) {
                BT_DBG("%04X\t %02X\t %d\t %04X\t %04X|%04d\t %04X",
                    cell.offset, cell.header.flag, cell.header.align, cell.header.index, cell.header.length,
                    cell.header.length, cell.header.crc);
                cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
            } else {
                BT_DBG_R("%04X\t %02X\t %d\t %04X\t %04X|%04d\t %04X",
                    cell.offset, cell.header.flag, cell.header.align, cell.header.index, cell.header.length,
                    cell.header.length, cell.header.crc);
                cell.offset += sizeof(S_FLASH_HEADER) + cell.header.length + cell.header.align;
            }
        }
    } while(ret == GENIE_FLASH_SUCCESS);

    return ret;
}

void genie_flash_dump(hal_partition_t pno, uint16_t size)
{
    uint32_t offset = 0;
    uint16_t i= 0;
    uint8_t data[16];
    
    if(size == 0) {
        size = 0x1000;
    }

    printk(F_RED "dump(%d)(%d)" F_END, pno, size);

    offset = 0;
    while(offset < size) {
        if(hal_flash_read(pno, &offset, data, 16))
            break;
        printf("[%04X]", offset-16);
        for(i = 0; i < 16; i++)
            printf(" %02X", data[i]);
        printf("\n");
    }
}
#endif
