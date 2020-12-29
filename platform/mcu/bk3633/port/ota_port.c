#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/hal_ota.h>
#include <hal/soc/soc.h>
#include "ota_port.h"

#define OTA_TWO_IMAGE_SWITCH_RUN 0
#define KV_HAL_OTA_CRC32  "hal_ota_get_crc32"

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
	uint8_t* ota_buf;
} ota_data_t;


typedef struct
{
	ota_data_t data_para;
	ota_tag_t ota_tag;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static CRC32_Context contex;

static uint32_t hal_ota_get_crc32(void);
static void hal_ota_save_crc32(uint32_t crc32);

#if OTA_TWO_IMAGE_SWITCH_RUN
#define OTA_IMAGE_A    0
#define OTA_IMAGE_B    1
#define KV_HAL_OTA_HDR  "hal_ota_get_hdr"
typedef struct
{
	uint32_t version;
	uint32_t hdr_num;
	struct
	{
		uint32_t img_ID;
		uint32_t hdr_len;
		uint32_t checksum;
		uint32_t img_len;
		uint32_t img_offset;
		uint32_t flash_offset;
	}ota_file[2];
}ota_hdr_t;

static ota_hdr_t ota_hdr_info;
static uint8_t ota_index;
static uint8_t ota_hdr_check;
static uint32_t _off_set = 0;

static int hal_get_ota_hdr(void)
{
    int len, ret;

	len = sizeof(ota_hdr_t);
    ret = aos_kv_get(KV_HAL_OTA_HDR, &ota_hdr_info, &len);

	return ret;
}

static int hal_ota_save_hdr(void)
{
    int len, ret;

	len = sizeof(ota_hdr_t);
	ret = aos_kv_set(KV_HAL_OTA_HDR, &ota_hdr_info, len, 1);

	return ret;
}

static void hal_get_ota_index(void)
{
	uint32_t offset, boot_addr;
	hal_logic_partition_t *partition_info;

	offset = 0x00;
	hal_flash_read(HAL_PARTITION_OTA_PARA, &offset, (void *)&boot_addr, sizeof(boot_addr));

	partition_info = hal_flash_get_info(PLATFORM_OTA_PARTATION);

	if(boot_addr == partition_info->partition_start_addr)
	{
		ota_index = OTA_IMAGE_A; //update to image A
	}
	else
	{
		ota_index = OTA_IMAGE_B; //update to image B
	}

	return;
}

static int hal_verify_ota_head(void)
{
	char imgid[5];
	
	if(~ota_hdr_info.version)
	{
		LOG("check ota version error %x.\r\n", ota_hdr_info.version);
		return 1;
	}

	if(ota_hdr_info.hdr_num != 2)
	{
		LOG("check ota number error %d.\r\n", ota_hdr_info.hdr_num);
		return 1;
	}

	memset(imgid, 0, sizeof(imgid));
	memcpy(imgid, (char *)&ota_hdr_info.ota_file[0].img_ID, 4);
	if(strcmp(imgid, "OTA1"))
	{
		LOG("OTA1 image id error.\r\n");
		return 1;
	}
	memcpy(imgid, (char *)&ota_hdr_info.ota_file[1].img_ID, 4);
	if(strcmp(imgid, "OTA2"))
	{
		LOG("OTA2 image id error.\r\n");
		return 1;
	}

	if(ota_hdr_info.ota_file[0].checksum == 0)
	{
		LOG("OTA1 checksum is zero.\r\n");
		return 1;
	}
	if(ota_hdr_info.ota_file[1].checksum == 0)
	{
		LOG("OTA2 checksum is zero.\r\n");
		return 1;
	}

	return 0;
}

static int hal_verify_ota_checksum(uint8_t crc_partition_idx, uint32_t crc_len)
{
	uint8_t *f_data;
	CRC32_Context ctx;
	uint32_t ota_crc;
	uint32_t i = 0, j = 0, len = 0, checksum = 0;

	CRC32_Final(&contex, &ota_info.ota_crc);//download crc
	if(ota_info.ota_crc == 0)
	{
		LOG("crc is zero.\r\n");
		return -1;
	}
	
	f_data = (uint8_t *)aos_malloc(0x1000);
	if(f_data == NULL)
	{
		LOG("hal_ota_crc16_compute malloc fail.\r\n");
		return -1;
	} 
	
	_off_set = 0;
	CRC32_Init(&ctx);
	
	while(i < crc_len)
	{
		if((0x1000 + i) < crc_len)
		{
			len = 0x1000;
		}
		else
		{
			len = crc_len - i;
		}
		
		hal_flash_read(crc_partition_idx, &_off_set, f_data, len);
		
		/*update checksum*/
		for(j = 0; j < len; j++)
		{
			checksum += f_data[j];
		}
		
		/*update crc32*/
		CRC32_Update(&ctx, f_data, len);
		
		i += len;
	}
	
	CRC32_Final(&ctx, &ota_crc);

	if(f_data)
	{
		aos_free(f_data);
	}

	/*verify crc*/
	if(ota_info.ota_crc != ota_crc)
	{
		LOG("crc check error: download_crc=0x%x, comput_crc=0x%x.\r\n", ota_info.ota_crc, ota_crc);
		return -1;
	}

	/*verify checksum*/
	if(ota_hdr_info.ota_file[ota_index].checksum != checksum)
	{
		LOG("checksum error: gen_chcksum=0x%x, comput_checksum=0x%x.\r\n", ota_hdr_info.ota_file[ota_index].checksum, checksum);
		return -1;
	}

	return 0;
}

static int bk3633_ota_init(hal_ota_module_t *m, void *something)
{
	uint32_t offset, delta;
    hal_logic_partition_t *partition_info;
	
    LOG("!!!!!!ota start!!!!!!\n");
	//hal_flash_dis_secure(0, 0, 0);//disable flash protect
	hal_get_ota_index();
    offset = *(uint32_t*)something;
	if(offset == 0)
	{
		if(ota_index == OTA_IMAGE_A)
		{
        	partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
			hal_flash_erase(HAL_PARTITION_APPLICATION, 0, partition_info->partition_length);
		}
		else
		{
			partition_info = hal_flash_get_info(PLATFORM_OTA_PARTATION);
			hal_flash_erase(PLATFORM_OTA_PARTATION, 0, partition_info->partition_length);
		}
		ota_hdr_check = 0;
		memset(&ota_info, 0, sizeof(ota_info));
		memset(&ota_hdr_info, 0, sizeof(ota_hdr_info));
		CRC32_Init(&contex);
	}
	else
	{
		ota_info.recv_len = offset;
		ota_info.ota_crc = 0;
		if(hal_get_ota_hdr())
		{
			LOG("get ota header fail.\r\n");
			return -1;
		}
		if(offset < sizeof(ota_hdr_t))
		{
			ota_hdr_check = 0;
		}
		else
		{
			ota_hdr_check = hal_verify_ota_head();
			if(ota_hdr_check == 0)
			{
				if(offset < ota_hdr_info.ota_file[1].img_offset)
				{
					delta = offset - sizeof(ota_hdr_t);
					if(ota_index == OTA_IMAGE_A)
					{
						_off_set = delta;
					}
				}
				else
				{
					delta = offset - ota_hdr_info.ota_file[1].img_offset;
					if(ota_index == OTA_IMAGE_B)
					{
						_off_set = delta;
					}
				}
			}
			else
			{
				LOG("check ota header error.\r\n");
				return -1;
			}
		}
		contex.crc = hal_ota_get_crc32();
	}
	
	return 0;
}

static int bk3633_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
	if(ota_index == OTA_IMAGE_A)
	{
    	hal_flash_read(HAL_PARTITION_APPLICATION, off_set, out_buf, out_buf_len);
	}
	else
	{
    	hal_flash_read(PLATFORM_OTA_PARTATION, off_set, out_buf, out_buf_len);
	}
	return 0;
}

static int bk3633_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
	uint32_t offset;

	if(ota_hdr_check)
	{
		LOG("check ota header fail.\r\n");
		return -1;
	}
	else
	{
		//bk_printf(".");
	}

	if(ota_info.recv_len < sizeof(ota_hdr_t))
	{
		offset = sizeof(ota_hdr_t) - ota_info.recv_len;
		if(in_buf_len < offset)
		{
			memcpy((uint8_t *)&ota_hdr_info + ota_info.recv_len, in_buf, in_buf_len);
		}
		else
		{
			memcpy((uint8_t *)&ota_hdr_info + ota_info.recv_len, in_buf, offset);
			ota_hdr_check = hal_verify_ota_head();
			if(ota_hdr_check == 0 && ota_index == OTA_IMAGE_A)
			{
				CRC32_Update(&contex, in_buf + offset, in_buf_len - offset);
				hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf + offset, in_buf_len - offset);
			}
		}
	}
	else
	{
		if(ota_index == OTA_IMAGE_A)
		{
			if(ota_info.recv_len + in_buf_len < ota_hdr_info.ota_file[1].img_offset)
			{
				CRC32_Update(&contex, in_buf, in_buf_len);
				hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, in_buf_len);
			}
			else
			{
				if(ota_info.recv_len < ota_hdr_info.ota_file[1].img_offset)
				{
					offset = ota_hdr_info.ota_file[1].img_offset - ota_info.recv_len;
					CRC32_Update(&contex, in_buf, offset);
					hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, offset);
				}
			}
		}
		else
		{
			if(ota_info.recv_len > ota_hdr_info.ota_file[1].img_offset)
			{
				CRC32_Update(&contex, in_buf, in_buf_len);
				hal_flash_write(PLATFORM_OTA_PARTATION, &_off_set, in_buf, in_buf_len);
			}
			else
			{
				if(ota_info.recv_len + in_buf_len > ota_hdr_info.ota_file[1].img_offset)
				{
					offset = ota_hdr_info.ota_file[1].img_offset - ota_info.recv_len;
					CRC32_Update(&contex, in_buf + offset, in_buf_len - offset);
					hal_flash_write(PLATFORM_OTA_PARTATION, &_off_set, in_buf + offset, in_buf_len - offset);
				}
			}
		}
	}
	
	ota_info.recv_len += in_buf_len;
	
	return 0;
}

static int bk3633_ota_set_boot(hal_ota_module_t *m, void *something)
{
	int ret = 0;
    uint32_t offset, addr_rb, boot_addr;
	uint32_t crc_partition_idx, crc_len;
	hal_logic_partition_t *partition_info;
    ota_finish_param_t *param;

	param = (ota_finish_param_t *)something;
    if (param==NULL)
	{
		LOG("finish type is null.\r\n");
		return -1;
    }

	if(ota_hdr_check)
	{
		LOG("check ota head fail.\r\n");
		return -1;
	}

	if(param->result_type == OTA_FINISH)
	{
		if(ota_index == OTA_IMAGE_A)
		{
			crc_partition_idx = HAL_PARTITION_APPLICATION;
			crc_len = ota_hdr_info.ota_file[0].img_len;
			partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
		}
		else
		{
			crc_partition_idx = PLATFORM_OTA_PARTATION;
			crc_len = ota_hdr_info.ota_file[1].img_len;
			partition_info = hal_flash_get_info(PLATFORM_OTA_PARTATION);
		}
		boot_addr = partition_info->partition_start_addr;
		
		ret = hal_verify_ota_checksum(crc_partition_idx, crc_len);
		if(ret)
		{
			return -1;
		}
		
		offset = 0x00;
		hal_flash_erase(HAL_PARTITION_OTA_PARA, offset, sizeof(boot_addr));
		
		offset = 0x00;
		hal_flash_write(HAL_PARTITION_OTA_PARA, &offset, (const void *)&boot_addr, sizeof(boot_addr));
		
		offset = 0x00;
		hal_flash_read(HAL_PARTITION_OTA_PARA, &offset, &addr_rb, sizeof(addr_rb));

		if(addr_rb != boot_addr)
		{
			LOG("write boot address fail.\r\n");
			return -1;
		}
	}
	else
	{
		ret = hal_ota_save_hdr();
		if(ret)
		{
			LOG("save ota header fail.\r\n");
			return -1;
		}
        hal_ota_save_crc32(contex.crc);
	}

	hal_reboot();
	return ret;
}
#else

static int bk3633_ota_init(void *something)
{
    hal_logic_partition_t *partition_info;
	uint32_t _off_set = *(uint32_t*)something;
	ota_data_t* p = &(ota_info.data_para);

    LOG("bk3633 ota init %d\n", _off_set);

	partition_info = hal_flash_get_info( PLATFORM_OTA_PARTATION );

    if(_off_set == 0)
    {
        hal_flash_erase(PLATFORM_OTA_PARTATION, 0 ,partition_info->partition_length);
		memset(&ota_info, 0 , sizeof(ota_info));
        CRC32_Init(&contex);
    }
    else if(_off_set < partition_info->partition_length)
    {
		p->recv_len = _off_set;
        contex.crc = hal_ota_get_crc32();
        LOG("--------get crc32 context.crc=%d!--------\n",contex.crc);
    }
	else
	{
		LOG("Init offset fail\n");
		return -1;
	}

	if(p->ota_buf == NULL)
	{
		p->ota_buf = (uint8_t*)aos_malloc(OTA_PAGE_SIZE);
	}
	else
	{
		LOG("OTA buff in use\n");
		return -1;
	}

	if(p->ota_buf == NULL)
	{
		return -1;
	}
	else
	{
		p->page_size = OTA_PAGE_SIZE;
	}

    return 0;
}

static int bk3633_ota_deinit(void *something)
{
	ota_data_t* p = &(ota_info.data_para);

	if(p->ota_buf)
	{
		aos_free(p->ota_buf);
		p->ota_buf = NULL;
	}

	hal_ota_save_crc32(contex.crc);

	return 0;
}

static int bk3633_ota_push_buf(uint8_t* p_buf, uint32_t* offset, uint32_t p_size)
{
	//LOG("OTA write flash offset 0x%x, size %d\n", *offset, p_size);
    int ret = hal_flash_write(PLATFORM_OTA_PARTATION, offset, p_buf, p_size);

	CRC32_Update( &contex, p_buf, p_size);

    return ret;
}

static int bk3633_ota_save(uint8_t* in_buf, uint32_t in_buf_len)
{
	uint32_t remain = 0, checkin = 0;
	ota_data_t* p = &(ota_info.data_para);
	int ret = 0;

	//LOG("OTA save %d\n", in_buf_len);

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
	p->page_cnt += checkin;

	if(p->page_cnt == p->page_size)
	{
		ret = bk3633_ota_push_buf(p->ota_buf, &(p->recv_len), p->page_cnt);
		p->page_cnt = 0;

		if(remain)
		{
			bk3633_ota_save(in_buf + checkin, remain);
		}
	}

    return ret;
}

static int bk3633_ota_read(volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(PLATFORM_OTA_PARTATION, off_set, out_buf, out_buf_len);
    return 0;
}

int hal_ota_save_tag(ota_terminate_type_e termi_type, uint32_t recv_size)
{
    uint32_t offset;
    hal_logic_partition_t* ota_partition, *dest_partition;
	ota_img_hdr_t ota_img_hdr;
	ota_tag_t* t = &(ota_info.ota_tag);

	LOG("hal_ota_save_tag, type %d, recv_size 0x%x, ota_size 0x%x", termi_type, recv_size, t->ota_size);

	if(termi_type== OTA_TERMI_FINISH && recv_size != t->ota_size)
	{
		LOG("Update not finished, type %d, recv_size %d, ota_size %d", termi_type, recv_size, t->ota_size);
		return -1;
	}

    ota_partition = hal_flash_get_info( PLATFORM_OTA_PARTATION );
	dest_partition = t->ota_type == OTA_PARTITION_STACK ? hal_flash_get_info(PLATFORM_STACK_PARTATION) :
                                   hal_flash_get_info( PLATFORM_APP_PARTATION );

	if(t->ota_size > dest_partition->partition_length)
	{
		LOG("Image is out of size %d, partition_length %d", t->ota_size, dest_partition->partition_length);
		return -1;
	}

	if(recv_size > t->ota_size)
	{
		LOG("Recive data %d out ot size %d", recv_size, t->ota_size);
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
	ota_img_hdr.magic_num = 1;

	if(termi_type == OTA_TERMI_BREAKPOINT)
	{
		hal_ota_save_crc32(contex.crc);
	}

	static_partition_write(STATIC_SECTION_OTA, &ota_img_hdr, sizeof(ota_img_hdr_t));

    return 0;
}

static int bk3633_ota_set_boot(void *something)
{
    ota_terminate_type_e termi_type = *(ota_terminate_type_e *)something;
	ota_data_t* p = &(ota_info.data_para);

	LOG("bk3633 set boot\n");

	if(p->page_cnt)
	{
		bk3633_ota_push_buf(p->ota_buf, &(p->recv_len), p->page_cnt);
		p->page_cnt = 0;
	}

	int ret = hal_ota_save_tag(termi_type, p->recv_len);

	LOG("ret %d, type %d\n", ret, termi_type);

    if (termi_type == OTA_TERMI_FINISH && !ret)
    {
        LOG("bk3633 start boot\n");
        hal_reboot();
        memset(&ota_info, 0 , sizeof ota_info);
    }
    else if (termi_type == OTA_TERMI_BREAKPOINT)
    {
		;
    }
    return 0;
}

static int bk3633_ota_tag_get(void *something)
{
	ota_img_tag_t* tag_get = (ota_img_tag_t*)something;
	ota_img_hdr_t hdl_old;

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	int ret = static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));

	if(!ret)
	{
        tag_get->ver = hdl_old.ver;
        tag_get->rom_ver = hdl_old.rom_ver;
        tag_get->size = hdl_old.size;
        return 0;
	}
    else
    {
        return -1;
    }

}

static int bk3633_ota_tag_init(uint16_t ver, uint16_t rom_ver)
{
    ota_img_hdr_t ota_img_hdr;
	ota_img_hdr_t hdl_old;

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	int ret = static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));

	if(ret)
	{
		ota_img_hdr.ver = ver;
		ota_img_hdr.rom_ver = rom_ver;
		ota_img_hdr.magic_num = 0;

    	static_partition_write(STATIC_SECTION_OTA, &ota_img_hdr, sizeof(ota_img_hdr_t));
		return 0;
	}
	else
	{
		return -1;
	}
	
}

static int bk3633_ota_tag_check(uint32_t ota_type, uint16_t ver, uint16_t rom_ver, uint32_t size)
{
	uint32_t offset;
    ota_img_hdr_t hdl_old;
	hal_logic_partition_t *partition_info;
	ota_tag_t* t = &(ota_info.ota_tag);

	LOG("bk3633_ota_tag_check type %d, version 0x%x, rom_ver 0x%x, size 0x%x\n", ota_type, ver, rom_ver, size);

	memset(t, 0 ,sizeof(ota_tag_t));

	partition_info = hal_flash_get_info( PLATFORM_OTA_PARTATION );

    memset(&hdl_old, 0, sizeof(ota_img_hdr_t));
	static_partition_read(STATIC_SECTION_OTA, &hdl_old, sizeof(ota_img_hdr_t));

	if(hdl_old.ver == ver)
	{
		LOG("Same Image Version!!!\n");
		goto exit;		
	}
	else
	{
		t->ver = ver;
	}

	if(hdl_old.rom_ver != rom_ver)
	{
		LOG("Diff ROM Version!!!(%x-%x)\n", hdl_old.rom_ver, rom_ver);
		goto exit;		
	}
	else
	{
		t->rom_ver = rom_ver;
	}
	

	if(ota_type != OTA_PARTITION_APP && ota_type != OTA_PARTITION_STACK)
	{
		LOG("Wrong OTA TYPE!!!\n");
		goto exit;
	}
	else
	{
		t->ota_type = ota_type;
	}


	if(size <= 0 || size > partition_info->partition_length)
	{
		LOG("Size doesn't match!!!\n");
		goto exit;
	}
	else
	{
		t->ota_size = size;
	}

	hal_logic_partition_t* dest_partition;
	dest_partition = ota_type == OTA_PARTITION_STACK ? hal_flash_get_info(PLATFORM_STACK_PARTATION) :
                                   hal_flash_get_info(PLATFORM_APP_PARTATION);

	if(size > dest_partition->partition_length)
	{
		LOG("Image is out of size");
		goto exit;
	}

	return 0;

exit:
	memset(t, 0 ,sizeof(ota_tag_t));
	return -1;

}

#endif

struct hal_ota_module_s bk3633_ota_module = {
    .init = bk3633_ota_init,
	.deinit = bk3633_ota_deinit,
    .ota_save = bk3633_ota_save,
    .ota_read = bk3633_ota_read,
    .ota_set_boot = bk3633_ota_set_boot,
	.ota_tag_check = bk3633_ota_tag_check,
	.ota_tag_get = bk3633_ota_tag_get,
	.ota_tag_init = bk3633_ota_tag_init,
};

static uint32_t hal_ota_get_crc32(void)
{
    int len = 4;
    uint32_t crc32=0;
    aos_kv_get(KV_HAL_OTA_CRC32, &crc32, &len);
    return crc32;
}

static void  hal_ota_save_crc32(uint32_t crc32)
{
    aos_kv_set(KV_HAL_OTA_CRC32, &crc32, 4, 1);
}

