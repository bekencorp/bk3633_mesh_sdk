#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <k_api.h>         // AliOS API definitions

#include <errno.h>
#include <hal/hal_ota.h>
#include <hal/soc/soc.h>
#include "ota_port.h"



#define KV_HAL_OTA_CRC32  "hal_ota_get_crc32"



#define  MID_AREA (0x30000)

uint32_t flash_offset = 0;



#define LOG printf

typedef struct
{
	uint8_t ota_type;
	uint16_t ver;
	uint16_t rom_ver;
	uint32_t ota_size;
	uint32_t ota_crc;
} ota_tag_t;

typedef struct
{
    uint32_t recv_len;
	uint32_t page_size;
	uint32_t page_cnt;
	uint32_t page_checksum;
	uint32_t app_checksum;
	uint8_t* ota_buf;
} ota_data_t;


typedef struct
{
    uint8_t origin_sec;
	uint8_t update_sec;
} ota_sec_t;


typedef struct
{
	ota_data_t data_para;
	ota_tag_t ota_tag;
    ota_sec_t ota_sec;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static CRC32_Context contex;

static uint32_t hal_ota_get_crc32(void);
static void hal_ota_save_crc32(uint32_t crc32);

static kmutex_t ota_port_mutex;

void bk3633_ota_mutex_init(void)
{
    krhino_mutex_create(&ota_port_mutex, "ota_port_mutex");
}

static int __ota_verify_board_sec()
{
    ota_sec_t* s = &(ota_info.ota_sec);

#ifdef CONFIG_OTA_DUAL_SWITCH
    uint32_t base_addr = *((volatile uint32_t *)0x400000);
    base_addr = base_addr * 0x22 / 0x20;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( HAL_PARTITION_APPLICATION );
    if(partition_info->partition_start_addr == base_addr)
    {
        s->update_sec = HAL_PARTITION_OTA_TEMP;
        s->origin_sec = HAL_PARTITION_APPLICATION;
        return 0;
    }

    partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    if(partition_info->partition_start_addr == base_addr)
    {
        s->update_sec = HAL_PARTITION_APPLICATION;
        s->origin_sec = HAL_PARTITION_OTA_TEMP;
        return 0;
    }

    LOG("FAIL! %s base_addr 0x%x", __func__, base_addr);

    return -1;
#else
    s->update_sec = HAL_PARTITION_OTA_TEMP;
    s->origin_sec = HAL_PARTITION_APPLICATION;

    return 0;
#endif
}

static int __ota_sec_check(void)
{
    ota_sec_t* s = &(ota_info.ota_sec);
    int ret;
#ifdef CONFIG_OTA_DUAL_SWITCH
    if(s->update_sec == s->origin_sec)
    {
        return -1;
    }

    if(s->update_sec != HAL_PARTITION_OTA_TEMP && s->update_sec != HAL_PARTITION_APPLICATION)
    {
        return -1;
    }

    if(s->origin_sec != HAL_PARTITION_OTA_TEMP && s->origin_sec != HAL_PARTITION_APPLICATION)
    {
        return -1;
    }
#else
    if(s->update_sec != HAL_PARTITION_OTA_TEMP || s->origin_sec != HAL_PARTITION_APPLICATION)
    {
        return -1;
    }
#endif

    return 0;
}

static int __ota_get_update_sec(void)
{
    ota_sec_t* s = &(ota_info.ota_sec);

    if(!__ota_sec_check())
    {
        return s->update_sec;
    }
    else
    {
        LOG("UP SEC ERROR\n");
        return -1;
    }
}

static int __ota_get_origin_sec(void)
{
    ota_sec_t* s = &(ota_info.ota_sec);

    if(!__ota_sec_check())
    {
        return s->origin_sec;
    }
    else
    {
        LOG("ORI SEC ERROR\n");
        return -1;
    }
}

static int bk3633_ota_init(void *something)
{
    hal_logic_partition_t *partition_info;
	uint32_t _off_set = *(uint32_t*)something;
	ota_data_t* p = &(ota_info.data_para);

    LOG("bk3633 ota init _off_set %d\n, page_size: %d", _off_set, p->page_size);

    flash_offset = 0;


    __ota_verify_board_sec();

    int up_sec = __ota_get_update_sec();

    if(up_sec == -1)
    {
        return -1;
    }

	partition_info = hal_flash_get_info(up_sec);
	printf("\n%s %d ota:%x \n", __func__, __LINE__, p->ota_buf);
	
	krhino_mutex_lock(&ota_port_mutex, -1);
	
    if(_off_set == 0)
    {
        hal_flash_erase(up_sec, 0 ,partition_info->partition_length);

		memset(&ota_info.ota_tag, 0 , sizeof(ota_info.ota_tag));

		ota_info.data_para.recv_len =0;
		ota_info.data_para.page_size =0;
		ota_info.data_para.page_cnt =0;
		ota_info.data_para.page_checksum =0;
		ota_info.data_para.app_checksum =0;


        CRC32_Init(&contex);
    }
    else if(_off_set < partition_info->partition_length)
    {
		p->recv_len = _off_set;
        contex.crc = hal_ota_get_crc32();
    }
	else
	{
		LOG("Init offset fail\n");
		return -1;
	}

//jx debug
    p->page_checksum = 0xFFFFFFFF;
    //p->page_checksum = 0;
    p->app_checksum = 0;


	if(p->ota_buf == NULL)
	{
		p->ota_buf = (uint8_t*)aos_malloc(OTA_PAGE_SIZE);
	}
	else
	{
		LOG("OTA buff in use\n");
//		return -1;
	}

	if(p->ota_buf == NULL)
	{
		krhino_mutex_unlock(&ota_port_mutex);
		return -1;
	}
	else
	{
		p->page_size = OTA_PAGE_SIZE;
	}
    LOG("bk3633 ota init _off_set %d\n, page_size: %d", _off_set, p->page_size);
    krhino_mutex_unlock(&ota_port_mutex);
    return 0;
}

static int bk3633_ota_deinit(void *something)
{
	krhino_mutex_lock(&ota_port_mutex, -1);
	ota_data_t* p = &(ota_info.data_para);

	printf("\r\n %s %d ota:%x \r\n", __func__, __LINE__, p->ota_buf);
//	if(p->ota_buf)
//	{
//		aos_free(p->ota_buf);
//		p->ota_buf = NULL;
//	}

	hal_ota_save_crc32(contex.crc);
    krhino_mutex_unlock(&ota_port_mutex);
	return 0;
}

enum OTA_AREA
{
    OTA_AREA_A,
    OTA_AREA_B
};

#ifdef CONFIG_OTA_DUAL_SWITCH
static enum OTA_AREA ota_get_AREA()
{
    uint32_t base_addr = *((volatile uint32_t *)0x400000);
    base_addr = base_addr * 0x22 / 0x20;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( HAL_PARTITION_APPLICATION );
    if(partition_info->partition_start_addr == base_addr)
    {
        return OTA_AREA_B;
    }

    partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    if(partition_info->partition_start_addr == base_addr)
    {
        return OTA_AREA_A;
    }

    LOG("FAIL! %s base_addr 0x%x", __func__, base_addr);

    return -1;
}
#endif


static int bk3633_ota_push_buf(uint8_t* p_buf, uint32_t* offset, uint32_t p_size)
{
    uint8_t up_sec = __ota_get_update_sec();

    uint32_t re_p_size = p_size;
    
	ota_tag_t* t = &(ota_info.ota_tag);

    if(up_sec == -1)
    {
        return -1;
    }
    enum OTA_AREA ota_area = ota_get_AREA();

    printf("(%s, %d) ota_area:%d, offset: 0x%x, p_size:%d\n", __func__, __LINE__, ota_area, *offset , p_size);

    if (ota_area == OTA_AREA_A)
    {
        if (*offset + p_size  <= t->ota_size)
        {
            re_p_size = p_size;
            
        } 
        else if (*offset < t->ota_size && *offset + p_size  > t->ota_size)
        {
             re_p_size = t->ota_size - *offset;
        } 
        else 
        {
            //do nothing
            return 1;
        }
    }

    if (ota_area == OTA_AREA_B)
    {
        if (*offset < MID_AREA && *offset + p_size > MID_AREA)
        {
            re_p_size = *offset + p_size - MID_AREA;
            p_buf += (MID_AREA - *offset);
        } 
        else if (*offset >= MID_AREA && *offset + p_size <= MID_AREA + t->ota_size) 
        {
            re_p_size = p_size;
        } 
        else if (*offset < MID_AREA + t->ota_size && *offset + p_size > MID_AREA + t->ota_size) 
        
{
            re_p_size = MID_AREA + t->ota_size - *offset;
        } 
        else
        {
            //do nothing
            return 1;
        }
    }



	krhino_mutex_lock(&ota_port_mutex, -1);
	printf("OTA write flash_offset 0x%x, re_p_size %d\n", flash_offset, re_p_size);
    int ret = hal_flash_write(up_sec, &flash_offset, p_buf, re_p_size, true);

	CRC32_Update( &contex, p_buf, re_p_size);
    krhino_mutex_unlock(&ota_port_mutex);
    return ret;
}


//jx debug




enum
{
    CRC_MATCH = 0,
    CRC_UNMATCH,
    CRC_APP_NOT_SET
};


static int checkcrc()
{
    int ret;
	krhino_mutex_lock(&ota_port_mutex, -1);
    if (ota_info.data_para.app_checksum == 0)
    {
        ret = CRC_APP_NOT_SET;
    }

    if (ota_info.data_para.page_checksum ^ ~0U == ota_info.data_para.app_checksum)
    //if (ota_info.data_para.page_checksum  == ota_info.data_para.app_checksum)
    {
        ret = CRC_MATCH;
    }
    else
    {
        ret = CRC_UNMATCH;
    }

    LOG("checkcrc app checksum: 0x%04x, page checksum: 0x%04x, ret : %d \n", ota_info.data_para.app_checksum, ota_info.data_para.page_checksum, ret);
    krhino_mutex_unlock(&ota_port_mutex);
    return ret;
}

static int bk3633_ota_app_checksum(uint32_t app_checksum)
{
	ota_data_t* p = &(ota_info.data_para);
    p->app_checksum = app_checksum;
    int ret;

	if(p->ota_buf == NULL)
	{
		return -1;
	}

    printf("(%s, %d) \n", __func__, __LINE__);
    krhino_mutex_lock(&ota_port_mutex, -1);
    
    if (checkcrc() == CRC_MATCH)
    {
		printf("(%s, %d) \n", __func__, __LINE__);

        uint32_t have_recv_len = p->recv_len;
		
    	ret = bk3633_ota_push_buf(p->ota_buf, &(p->recv_len), p->page_cnt);

        p->recv_len = have_recv_len + p->page_cnt;
    	
		printf("(%s, %d) ret:%d\n", __func__, __LINE__, ret);
    	p->page_cnt = 0;
    	p->page_checksum = 0xFFFFFFFF;
    	//p->page_checksum = 0;
        p->app_checksum = 0;
        ret = 0;
    }
    else
    {
		printf("(%s, %d) \n", __func__, __LINE__);
    	p->page_cnt = 0;
    	p->page_checksum = 0xFFFFFFFF;
    	//p->page_checksum = 0;
        p->app_checksum = 0;
        ret = 1;
    }

	krhino_mutex_unlock(&ota_port_mutex);
    return ret;
}

static int bk3633_ota_save(uint8_t* in_buf, uint32_t in_buf_len)
{
	krhino_mutex_lock(&ota_port_mutex, -1);
	uint32_t remain = 0, checkin = 0;
	ota_data_t* p = &(ota_info.data_para);
	int ret = 0;

    LOG("bk3633_ota_save save %d\n", in_buf_len);

	if(!p->ota_buf)
	{
		return -1;
	}

	if(p->page_cnt + in_buf_len < p->page_size)
	{
		checkin = in_buf_len;
	}
	else
	{
		checkin = p->page_size - p->page_cnt;
		remain = in_buf_len - checkin;
	}

	memcpy(p->ota_buf + p->page_cnt, in_buf, checkin);

    p->page_checksum = CRC_Update_api(contex.crc_table, p->page_checksum, in_buf, checkin);

    //p->page_checksum = crc32(p->page_checksum, in_buf, checkin);

	p->page_cnt += checkin;

#if 0
	if(p->page_cnt == p->page_size)
	{
	    if (checkcrc() == CRC_MATCH)
	    {
    		ret = bk3633_ota_push_buf(p->ota_buf, &(p->recv_len), p->page_cnt);
    		p->page_cnt = 0;
    		p->page_checksum = 0xFFFFFFFF;
            p->app_checksum = 0;

    		if(remain)
    		{
    			bk3633_ota_save(in_buf + checkin, remain);
    		}
        }
        else
        {
    		p->page_cnt = 0;
    		p->page_checksum = 0xFFFFFFFF;
            p->app_checksum = 0;
        }
	}
#else
    if(remain)
    {

        printf("(%s, %d) Error had remain data  \n", __func__, __LINE__);
        ret = -1;
    }
#endif

    krhino_mutex_unlock(&ota_port_mutex);
    return ret;
}

static int bk3633_ota_read(volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    uint8_t up_sec = __ota_get_update_sec();

    if(up_sec == -1)
    {
        return -1;
    }

    hal_flash_read(up_sec, off_set, out_buf, out_buf_len);
    return 0;
}

int hal_ota_save_tag(ota_terminate_type_e termi_type, uint32_t recv_size)
{
    uint32_t offset;
    hal_logic_partition_t* ota_partition, *dest_partition;
	ota_img_hdr_t ota_img_hdr;
	ota_tag_t* t = &(ota_info.ota_tag);
    uint8_t up_sec = __ota_get_update_sec();
    uint8_t ori_sec = __ota_get_origin_sec();

    //recv_size -= MID_AREA; 

	LOG("hal_ota_save_tag, type %d, recv_size 0x%x, ota_size 0x%x", termi_type, recv_size, t->ota_size);

    if(up_sec == -1 || ori_sec == -1)
    {
        return -1;
    }
	
	krhino_mutex_lock(&ota_port_mutex, -1);

	if(termi_type== OTA_TERMI_FINISH && recv_size != t->ota_size)
	{
		LOG("Update not finished, type %d, recv_size %d, ota_size %d", termi_type, recv_size, t->ota_size);
		krhino_mutex_unlock(&ota_port_mutex);
		return -1;
	}

    ota_partition = hal_flash_get_info(up_sec);
	dest_partition = t->ota_type == OTA_PARTITION_STACK ? hal_flash_get_info(HAL_PARTITION_BT_FIRMWARE) :
                                   hal_flash_get_info(ori_sec);

	if(t->ota_size > dest_partition->partition_length)
	{
		LOG("Image is out of size %d, partition_length %d", t->ota_size, dest_partition->partition_length);
		krhino_mutex_unlock(&ota_port_mutex);
		return -1;
	}

	if(recv_size > t->ota_size)
	{
		LOG("Recive data %d out ot size %d", recv_size, t->ota_size);
		krhino_mutex_unlock(&ota_port_mutex);
		return -1;
	}

    ota_img_hdr.dst_adr = dest_partition->partition_start_addr;
	ota_img_hdr.block_size = dest_partition->partition_length;
	ota_img_hdr.src_adr = ota_partition->partition_start_addr;
	ota_img_hdr.crc = contex.crc;
	ota_img_hdr.ver = t->ver;
	ota_img_hdr.rom_ver = t->rom_ver;
	ota_img_hdr.size = t->ota_size;
	ota_img_hdr.termi_type = termi_type;
	ota_img_hdr.break_point = recv_size;
#ifdef CONFIG_OTA_DUAL_SWITCH
	ota_img_hdr.magic_num = DUAL_SWITCH_MAGIC_NUM;
#else
	ota_img_hdr.magic_num = 1;
#endif

	printf("crc : 0x%x", contex.crc);

	if(termi_type == OTA_TERMI_BREAKPOINT)
	{
		hal_ota_save_crc32(contex.crc);
	}

	static_partition_write(STATIC_SECTION_OTA, &ota_img_hdr, sizeof(ota_img_hdr_t));

    krhino_mutex_unlock(&ota_port_mutex);
    return 0;
}

static int bk3633_ota_set_boot(void *something)
{
    ota_terminate_type_e termi_type = *(ota_terminate_type_e *)something;
	ota_data_t* p = &(ota_info.data_para);

	if(p->ota_buf == NULL)
	{
		return -1;
	}

    krhino_mutex_lock(&ota_port_mutex, -1);
	LOG("bk3633_ota_set_boot \n");

	if(p->page_cnt)
	{

        uint32_t have_recv_len = p->recv_len;
		
    	bk3633_ota_push_buf(p->ota_buf, &(p->recv_len), p->page_cnt);

        p->recv_len = have_recv_len + p->page_cnt;

		p->page_cnt = 0;
	}

	int ret = hal_ota_save_tag(termi_type, flash_offset);

	LOG("ret %d, type %d\n", ret, termi_type);

    if (termi_type == OTA_TERMI_FINISH && !ret)
    {
        LOG("bk3633 start boot\n");
        hal_reboot();
        memset(&ota_info, 0 , sizeof(ota_info));
    }
    else if (termi_type == OTA_TERMI_BREAKPOINT)
    {
		;
    }
	krhino_mutex_unlock(&ota_port_mutex);
    return 0;
}

static int bk3633_ota_tag_get(void *something)
{

    krhino_mutex_lock(&ota_port_mutex, -1);
    printf("(%s, %d) \n", __func__, __LINE__);

	ota_img_tag_t* tag_get = (ota_img_tag_t*)something;
	ota_img_hdr_t hdl_old;

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	int ret = static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));

	if(!ret)
	{
        tag_get->ver = hdl_old.ver;
        tag_get->rom_ver = hdl_old.rom_ver;
        tag_get->size = hdl_old.size;
		krhino_mutex_unlock(&ota_port_mutex);
        return 0;
	}
    else
    {
		krhino_mutex_unlock(&ota_port_mutex);
        return -1;
    }

}

static int bk3633_ota_tag_init(uint16_t ver, uint16_t rom_ver)
{
    ota_img_hdr_t ota_img_hdr;
	ota_img_hdr_t hdl_old;

    krhino_mutex_lock(&ota_port_mutex, -1);
    printf("(%s, %d) \n", __func__, __LINE__);

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	int ret = static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));

	if(ret)
	{
		ota_img_hdr.ver = ver;
		ota_img_hdr.rom_ver = rom_ver;
		ota_img_hdr.magic_num = 0;

    	static_partition_write(STATIC_SECTION_OTA, &ota_img_hdr, sizeof(ota_img_hdr_t));
		krhino_mutex_unlock(&ota_port_mutex);
		return 0;
	}
	else
	{
		krhino_mutex_unlock(&ota_port_mutex);
		return -1;
	}

}

static int bk3633_ota_tag_check(uint32_t ota_type, uint16_t ver, uint16_t rom_ver, uint32_t size)
{
	uint32_t offset;
    ota_img_hdr_t hdl_old;
	hal_logic_partition_t *partition_info;
	ota_tag_t* t = &(ota_info.ota_tag);
    uint8_t up_sec = __ota_get_update_sec();
    uint8_t ori_sec = __ota_get_origin_sec();
    int ret_temp;

	LOG("bk3633_ota_tag_check type %d, version 0x%x, rom_ver 0x%x, size 0x%x\n", ota_type, ver, rom_ver, size);

    if(up_sec == -1 || ori_sec == -1)
    {
        return -1;
    }

	krhino_mutex_lock(&ota_port_mutex, -1);
	memset(t, 0 ,sizeof(ota_tag_t));

	partition_info = hal_flash_get_info(up_sec);

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	ret_temp = static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));


    printf("ota_tag_check static_partition_read : %d\n", ret_temp);


	if(hdl_old.ver == ver)
	{
		LOG("ota_tag_check Same Image Version!!!  (%x-%x): %d \n", hdl_old.ver, ver);
		// goto exit;
	}
	else
	{
		t->ver = ver;
	}

	if(hdl_old.rom_ver != rom_ver)
	{
		LOG("ota_tag_check Diff ROM Version!!!(%x-%x)\n", hdl_old.rom_ver, rom_ver);
		// goto exit;
	}
	else
	{
		t->rom_ver = rom_ver;
	}


	if(ota_type != OTA_PARTITION_APP && ota_type != OTA_PARTITION_STACK)
	{
		LOG("ota_tag_check Wrong OTA TYPE!!!\n");
		// goto exit;
	}
	else
	{
		t->ota_type = ota_type;
	}


	if(size <= 0 || size > partition_info->partition_length)
	{
		LOG("ota_tag_check Size doesn't match!!!\n");
		// goto exit;
	}
	else
	{
		t->ota_size = size;
	}

	hal_logic_partition_t* dest_partition;
	dest_partition = ota_type == OTA_PARTITION_STACK ? hal_flash_get_info(HAL_PARTITION_BT_FIRMWARE) :
                                   hal_flash_get_info(ori_sec);

	if(size > dest_partition->partition_length)
	{
		LOG("ota_tag_check Image is out of size");
		// goto exit;
	}

    krhino_mutex_unlock(&ota_port_mutex);
	return 0;

exit:
	memset(t, 0 ,sizeof(ota_tag_t));
	krhino_mutex_unlock(&ota_port_mutex);
	return -1;

}



struct hal_ota_module_s bk3633_ota_module = {
    .init = bk3633_ota_init,
	.deinit = bk3633_ota_deinit,
    .ota_save = bk3633_ota_save,
    .ota_read = bk3633_ota_read,
    .ota_set_boot = bk3633_ota_set_boot,
    .ota_tag_check = bk3633_ota_tag_check,
    .ota_tag_get = bk3633_ota_tag_get,
    .ota_tag_init = bk3633_ota_tag_init,
    .ota_app_checksum = bk3633_ota_app_checksum,
	.ota_app_mutex_init = bk3633_ota_mutex_init,
};

static uint32_t hal_ota_get_crc32(void)
{
    int len = 4;
    uint32_t crc32=0;
	krhino_mutex_lock(&ota_port_mutex, -1);
    aos_kv_get(KV_HAL_OTA_CRC32, &crc32, &len);
	krhino_mutex_unlock(&ota_port_mutex);
    return crc32;
}

static void  hal_ota_save_crc32(uint32_t crc32)
{
	krhino_mutex_lock(&ota_port_mutex, -1);
    aos_kv_set(KV_HAL_OTA_CRC32, &crc32, 4, 1);
	krhino_mutex_unlock(&ota_port_mutex);
}

