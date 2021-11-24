/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <port/mesh_hal_ble.h>
#include <k_api.h>         // AliOS API definitions

#include "genie_app.h"
#include "ota_module.h"
#include "hal_ota.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_OTA)
#include "common/log.h"
#include "flash_pub.h"


#include "JX_app.h"

static struct otas_env_tag* otas_env;

static struct bt_gatt_ccc_cfg beken_ota_ffc1_ccc_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg beken_ota_ffc2_ccc_cfg[BT_GATT_CCC_MAX] = {};

static kmutex_t ota_sync_mutex;

#define BT_DBG(f, ...) printf("%s: "f"\n", __func__, ##__VA_ARGS__)


static void beken_ota_ffc1_ccc_cfg_changed(const struct bt_gatt_attr *p_attr, struct bt_conn *conn, uint16_t value)
{
    printf("value %d\n", value);
	printf("%s %d value:%d\n", __func__, __LINE__, value);

    krhino_mutex_lock(&ota_sync_mutex, -1);
    if(value)
    {
        ota_srv_connected(conn);
    }
    else
    {
        ota_srv_disconnect();
    }
    krhino_mutex_unlock(&ota_sync_mutex);
}

uint8_t ota_get_firmware_type(void)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0xFF;
    }
    return otas_env->config_para.ota_firmware_type;
}

uint8_t ota_set_firmware_type(uint8_t type)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0xFF;
    }

    otas_env->config_para.ota_firmware_type = type;
    return 0;
}

bool ota_update_complete_get(void)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0;
    }

    return otas_env->config_para.update_comp;
}

bool ota_update_complete_set(void)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0;
    }

    krhino_mutex_lock(&ota_sync_mutex, -1);
    otas_env->config_para.update_comp = 1;
    krhino_mutex_unlock(&ota_sync_mutex);
}

bool ota_update_start_set(void)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0;
    }
    krhino_mutex_lock(&ota_sync_mutex, -1);
    otas_env->config_para.update_start = 1;
    krhino_mutex_unlock(&ota_sync_mutex);
}

bool ota_update_abort(void)
{
    BT_DBG("");
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0;
    }
    krhino_mutex_lock(&ota_sync_mutex, -1);
    otas_env->config_para.update_start = 0;
    krhino_mutex_unlock(&ota_sync_mutex);
}

bool ota_update_in_progress(void)
{
    if(!otas_env)
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
        return 0;
    }
    return otas_env->config_para.update_start;
}


void beken_get_image_header(beken_image_hdr* img_hdr)
{
    int32_t offset = -0x10;

    hal_flash_read(HAL_PARTITION_APPLICATION, &offset, img_hdr, sizeof(beken_image_hdr));
}



static ssize_t hilink_ota_auth_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{
    printf("(%s, %d) \n", __func__, __LINE__);

    return 0;
}

#if 0

#define DATA_LEN    16


void HiLink_hex_to_str(uint8_t *value, uint8_t *buf, uint8_t len)
{
    uint32_t temp;
	int j=0;

	for(uint8_t k=0;k<len/2;++k)
	{
		uint8_t m = 2;

		while(m--)
		{
			temp = (value[k]&(0xf<<(4*m)))>>(4*m);
			if(temp>9)
			{
				buf[j] = 'A'+temp-10;
			}
			else
			{
				buf[j] = '0'+temp;
			}
			j++;
		}
	}

	buf[len] = '\0';
}

int HiLink_pkcs7_padding(char *data, int dataSize, int dataLen)
{
	if(dataLen <= 0)
		return -1;

	if((dataLen+17) >= dataSize)
		return -2;

	uint8_t paddingNum = 16 - (dataLen % 16);

	memset(&data[dataLen], paddingNum, paddingNum);

	data[dataLen+paddingNum] = '\0';

	return dataLen+paddingNum;
}

static uint8_t in_swap[DATA_LEN];
static uint8_t out[DATA_LEN];
static uint8_t out_swap[DATA_LEN];
static uint8_t iv_swap[16];
static uint8_t in[DATA_LEN];

typedef struct
{
    int nr;                     /*!<  number of rounds  */
    uint32_t *rk;               /*!<  AES round keys    */
    uint32_t buf[68];           /*!<  unaligned data    */
}
aes_context;


bool hw_aes_encrypt128(uint32_t *input, uint32_t *output, uint16_t data_word_len,
                              uint32_t *p_key,
                              uint32_t *p_iv, T_HW_AES_MODE mode)
{

}

extern bool hw_aes_encrypt256(uint32_t *p_in, uint32_t *p_out, uint16_t data_word_len,
                              uint32_t *p_key,
                              uint32_t *p_iv, T_HW_AES_MODE mode);


static void swap_buf(const uint8_t *src, uint8_t *dst, uint16_t len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        dst[len - 1 - i] = src[i];
    }
}

bool HiLink_aes128_encrypt(uint8_t *p_in, uint8_t in_len, uint8_t *p_out, T_HW_AES_MODE aes_mode)
{
	bool ret = true;
	uint8_t aes_key_swap[32] = {0};

	if (in_len != 16 && in_len != 32)
    {
        return false;
    }

	//init
    memset(in_swap, 0, DATA_LEN);
    memset(out, 0, DATA_LEN);
    memset(out_swap, 0, DATA_LEN);

	memcpy(in, p_in, DATA_LEN);
	swap_buf(HiLink_ota_aes_key, aes_key_swap, in_len);
    swap_buf(HiLink_ota_aes_offset, iv_swap, 16);

	/* Most significant octet of plaintextData corresponds to in[0] */
    for (int i = 0; i < DATA_LEN / 16; ++i)
    {
        swap_buf(in + 16 * i, in_swap + 16 * i, 16);
    }

	//enc
    if (in_len == 16)
    {
        ret = hw_aes_encrypt128((uint32_t *) in_swap, (uint32_t *) out, DATA_LEN / 4,
                                (uint32_t *)aes_key_swap,
                                (uint32_t *)iv_swap, aes_mode);
    }

	/* Most significant octet of encryptedData corresponds to out[0] */
    for (int i = 0; i < DATA_LEN / 16; ++i)
    {
        swap_buf(out + 16 * i, out_swap + 16 * i, 16);
    }

#if 0
	DBG_DIRECT("enc result:");
    for (int i = 0; i < DATA_LEN / 16; ++i)
    {
        DBG_DIRECT("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x",
                   out_swap[16 * i + 0], out_swap[16 * i + 1], out_swap[16 * i + 2], out_swap[16 * i + 3],
                   out_swap[16 * i + 4], out_swap[16 * i + 5], out_swap[16 * i + 6], out_swap[16 * i + 7],
                   out_swap[16 * i + 8], out_swap[16 * i + 9], out_swap[16 * i + 10], out_swap[16 * i + 11],
                   out_swap[16 * i + 12], out_swap[16 * i + 13], out_swap[16 * i + 14], out_swap[16 * i + 15]);
    }
#endif
	memcpy(p_out, out_swap, in_len);

	return ret;
}


void HiLink_OTA_aes_auth_get(uint8_t *auth_buf, uint8_t len)
{
	uint8_t aes_encrypt_in[32]={0};
	uint8_t bt_addr_bytes[17] = {0}, addr_temp[6]={0}, addr[6] = {0};

	// gap_get_param(GAP_PARAM_BD_ADDR, addr_temp);
	for (int i = 0; i < 6; i++)
	{
		addr[i] = addr_temp[5-i];
	}
	//memcpy(mac_addr, addr, 6);

	HiLink_hex_to_str(addr, bt_addr_bytes, 16);

	//pid load, default_name
	extern uint8_t default_name[24];
	memcpy(aes_encrypt_in, &default_name[17], 4);
	memcpy(&aes_encrypt_in[4], bt_addr_bytes, 12);
	printf("bf: aes\n");
	for(uint8_t i = 0; i<16; i++)
	{
		printf("%02x ", aes_encrypt_in[i]);
	}

	HiLink_pkcs7_padding(aes_encrypt_in, sizeof(aes_encrypt_in), 16);
	//aes128_cbc_encrypt(aes_encrypt_in, HiLink_ota_aes_key, aes_encrypt_in,HiLink_ota_aes_offset, 16);

	HiLink_aes128_encrypt(aes_encrypt_in, 16, aes_encrypt_in, AES_MODE_CBC);
	printf("af128: aes\n");
	for(uint8_t i = 0; i<16; i++)
	{
		printf("%02x ", aes_encrypt_in[i]);
	}

	memcpy(auth_buf, aes_encrypt_in, len);
}
#endif

static ssize_t hilink_ota_auth_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    printf("(%s, %d) \n", __func__, __LINE__);

#if 0
    uint8_t app_aes_auth[32] = {0};
    uint8_t dev_aes_buf[16] = {0};

    if (len >= 16 )
    {

        memcpy(app_aes_auth, p_buf, DATA_LEN);


        HiLink_OTA_aes_auth_get(dev_aes_buf, DATA_LEN);

        if(!memcmp(app_aes_auth, dev_aes_buf, DATA_LEN))
        {
            hilink_ota_auth.hilink_ntf_result = 0;
            HiLink_ota_en = true;
        }

    }
#endif


    return len;
}


static ssize_t hilink_ota_status_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{
     printf("(%s, %d) \n", __func__, __LINE__);

     return 0;
}


typedef uint16_t Hi_app_type_t;

static Hi_app_type_t Hi_app_cmd = 0;

static bool HiLink_ota_en = true;

static uint8_t checksum_ret = 1;


void show_buf(const char *name, uint8_t *buf, int len)
{
    int i = 0;

    printf("%s: ", name);
    for(i = 0; i < len; i++) {
        if(i % 50 == 0 && i != 0)
            printf("\n");
        printf("%02x ", buf[i]);
    }
    printf("\n");
}


static ssize_t hilink_ota_status_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
	BT_DBG("GATT_UUID_CHAR_OTA_INDEX, HiLink_ota_en = %d", HiLink_ota_en);

    krhino_mutex_lock(&ota_sync_mutex, -1);
    u16_t len_back = len;

	if(!HiLink_ota_en)
	{
        krhino_mutex_unlock(&ota_sync_mutex);
		return -1;
	}


    uint8_t * pvalue = (uint8_t *)p_buf;


	uint16_t position = 0;
	//sensor_ltr303ALS_work_set(false);

	while(len)
	{
		Hi_app_cmd = LE_EXTRN2WORD(pvalue + position);
		// LOG("hilink_ota_status_write app_cmd:%04x", Hi_app_cmd);
		switch(Hi_app_cmd)
		{
			case HiLink_OTA_CMD_FILE_START:{

			#if 0
				position += 1 +sizeof(Hi_app_type_t);		//lens
				HiLink_OTA_total_size = pvalue[position]<<24 | pvalue[position+1]<<16 | pvalue[position+2]<<8 | pvalue[position+3];
				position += 4;

				if(HiLink_OTA_total_size>0)
				{
					HiLink_OTA_sli_buffer_init(false);
				}

				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}
				break;
            #endif

			}
			case HiLink_OTA_CMD_FILE_CHECKSUM:{
				//uint8_t checksum_notify_buf[] = {0xFF, 0x02, 0x01, 0x00};
				uint32_t HiLink_OTA_wr_size = 0;
				uint32_t HiLink_OTA_checksum = 0;
				position += 1 + sizeof(Hi_app_type_t);		//lens
				HiLink_OTA_wr_size = pvalue[position]<<24 | pvalue[position+1]<<16 | pvalue[position+2]<<8 | pvalue[position+3];
				position += 4;
				HiLink_OTA_checksum = pvalue[position]<<24 | pvalue[position+1]<<16 | pvalue[position+2]<<8 | pvalue[position+3];
				position += 4;


                #if 0
				if(HiLink_OTA_crc32_value == HiLink_OTA_checksum)
				{
					HiLink_OTA_crc_result = 0;
				}
				else
				{
					HiLink_OTA_crc_result = 1;
				}

				HiLink_OTA_crc32_value = 0;
				#else

                // printf("(%s, %d) HiLink_OTA_checksum : 0x%x \n", __func__, __LINE__, HiLink_OTA_checksum);


                checksum_ret = hal_ota_app_checksum(NULL, HiLink_OTA_checksum);

				#endif


				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}

				break;
			}
			case HiLink_OTA_CMD_FILE_WRITE:{
				uint32_t offset = 0;
				uint16_t data_size = 0;
				position += 1 + sizeof(Hi_app_type_t);		//lens

				offset = pvalue[position]<<24 | pvalue[position+1]<<16 | pvalue[position+2]<<8 | pvalue[position+3];
				position += 4;

				data_size = pvalue[position]<<8 | pvalue[position+1];
				position += 2;

				// HiLink_OTA_server_handle_ota_msg(offset, (uint8_t *)&pvalue[position], data_size);

                 //show_buf("write data:", (uint8_t *)&pvalue[position], data_size);


                hal_ota_save(NULL, (uint8_t *)&pvalue[position], data_size);

				position += data_size;

				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}
				break;
			}
			case HiLink_OTA_CMD_FILE_FINSHED:{
				position += sizeof(Hi_app_type_t);
				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}

                //ota_terminate_type_e termi_type = OTA_TERMI_FINISH;
                //hal_ota_set_boot(NULL, &termi_type);

				break;
			}
			case HiLink_OTA_CMD_FILE_RESET:{
				position += sizeof(Hi_app_type_t);
				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}

		        //ota_update_abort();

                ota_terminate_type_e termi_type = OTA_TERMI_FINISH;
                hal_ota_set_boot(NULL, &termi_type);

				break;
			}
			case HiLink_OTA_CMD_FILE_DEV_VERSION:{
				position += sizeof(Hi_app_type_t);		//lens

				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}
				break;
			}
			default:{
				position += sizeof(Hi_app_type_t);		//lens

				if(position>=len)
				{
					len = 0;
				}
				else
				{
					len -= position;
				}
				break;
			}
		}
	}

    krhino_mutex_unlock(&ota_sync_mutex);
    return len_back;

    // return 0;
}


static ssize_t hilink_ota_version_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{
    printf("%s, %d \n", __func__, __LINE__);

    krhino_mutex_lock(&ota_sync_mutex, -1);
	u8 i;

//    printf("buf:%s, %d \n",__func__, __LINE__);


    u16_t ret_len = 0;


    if(ota_update_in_progress())
    {
        uint32_t offset = 0;
        ota_update_abort();
        ota_reset(offset, otas_env->p_conn);
    }

    ota_srv_connected(p_conn);

    beken_image_hdr saved_tag;
    ota_hdr_para_t* img_hdr = &otas_env->img_hdr;

    ota_hdr_custom_para_t* custom_hdr;

    custom_hdr = (struct ota_hdr_custom_para_t * )aos_malloc(sizeof(ota_hdr_custom_para_t));

    if(!custom_hdr)
    {
        BT_ERR("OTA env malloc error\n");
        krhino_mutex_unlock(&ota_sync_mutex);
        return;
    }

    ota_img_tag_t static_hdr;
    int ret = hal_ota_tag_get(NULL, &static_hdr);

    if(!ret)
    {
        saved_tag.ver = static_hdr.ver;
        saved_tag.len = static_hdr.size;
        saved_tag.rom_ver = static_hdr.rom_ver;
        saved_tag.uid = OTA_APP_PART_UID;
    }
    else
    {
        beken_get_image_header(&saved_tag);
        hal_ota_tag_init(NULL, saved_tag.ver, saved_tag.rom_ver);
        int ret = hal_ota_tag_get(NULL, &static_hdr);
    }

    img_hdr->ver = saved_tag.ver;
    img_hdr->len = saved_tag.len;
    img_hdr->rom_ver = saved_tag.rom_ver;
    img_hdr->uid = saved_tag.uid;


    custom_hdr->ver = saved_tag.ver;
    custom_hdr->len = saved_tag.len;
    custom_hdr->rom_ver = saved_tag.rom_ver;
    custom_hdr->uid = saved_tag.uid;
    custom_hdr->custom_ver = (u16)PROJECT_SW_VERSION; //CUSTOM_VERSION;

    printf("ver = 0x%x, len = 0x%x", saved_tag.ver, saved_tag.len);
    printf("rom_ver = 0x%x, uid = 0x%x \n", saved_tag.rom_ver, saved_tag.uid);

    printf("offset = 0x%x, length = 0x%x \n", offset, sizeof(ota_hdr_para_t));
    printf("custom:uid=%x ver=%x, rom_ver=%x custom_ver=%x len=%d\n",
		custom_hdr->uid, custom_hdr->ver, custom_hdr->rom_ver, custom_hdr->custom_ver, custom_hdr->len);

    ret_len =  bt_gatt_attr_read(p_conn, p_attr, buf, len, offset, custom_hdr, sizeof(ota_hdr_custom_para_t));


    if(custom_hdr)
    {
        aos_free(custom_hdr);
        custom_hdr = NULL;
    }
    krhino_mutex_unlock(&ota_sync_mutex);
    return ret_len;
}



// static ota_hdr_para_t img_hdr;

static ssize_t hilink_ota_version_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    printf("(%s, %d) \n", __func__, __LINE__);

    krhino_mutex_lock(&ota_sync_mutex, -1);
    ota_srv_connected(p_conn);

//jx debug
    // bt_mesh_scan_stop();


    ota_hdr_para_t* img_hdr = &otas_env->img_hdr;
    beken_image_hdr saved_tag;
    int ota_check_res = -1;

    saved_tag = *(beken_image_hdr*)p_buf;

    img_hdr->ver = saved_tag.ver;
    img_hdr->len = saved_tag.len;
    img_hdr->uid = saved_tag.uid;
    img_hdr->rom_ver = saved_tag.rom_ver;
    printf("RX ver = 0x%x, len = 0x%x\n", img_hdr->ver, img_hdr->len);
    printf("RX uid = 0x%x, rom_ver = 0x%x\n", img_hdr->uid, img_hdr->rom_ver);

    if (img_hdr->uid == OTA_APP_PART_UID)
    {
        ota_set_firmware_type(OTA_FM_TYPE_APP);
        printf("app part upgrade\n");
    }
    else if (img_hdr->uid == OTA_APP_STACK_UID)
    {
        ota_set_firmware_type(OTA_FM_TYPE_STACK);
        printf("app and stack upgrade\n");
    }
    else
    {
        ota_set_firmware_type(OTA_FM_TYPE_NONE);
    }

    ota_check_res = hal_ota_tag_check(NULL, ota_get_firmware_type(), img_hdr->ver, img_hdr->rom_ver, (uint32_t)img_hdr->len * HAL_FLASH_WORD_SIZE - 16);

//jx debug
    ota_check_res = 0;


    if (!ota_check_res && !ota_update_in_progress())
    {
        printf("Start upgrade\n");
        ota_update_start_set();
        //update ota connect parameter.
        struct bt_le_conn_param param;
        param.interval_min = OTA_INIT_CONN_INT_MIN;
        param.interval_max = OTA_INIT_CONN_INT_MAX;
        param.latency = OTA_INIT_CONN_LATENCY;
        param.timeout = OTA_INIT_CONN_TO;
        int ret = bt_conn_le_param_update(otas_env->p_conn, &param);
        printf("update le para ret %d\n", ret);

        // otaImgBlockReq(otas_env->version_attr, *otaBlkNum);
    }
    else
    {
        BT_DBG("Tag check fail %d, %d", ota_check_res, ota_update_in_progress());
    }

    krhino_mutex_unlock(&ota_sync_mutex);
    return len;
}

static ssize_t hilink_ota_checksum_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{
    uint8_t value;
    u16_t length ;

    printf("(%s, %d) \n",__func__, __LINE__);

    length = sizeof(uint8_t);

    value = checksum_ret;

    return bt_gatt_attr_read(p_conn, p_attr, buf, len, offset, &value, length);

}

static ssize_t hilink_ota_checksum_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    //printf("(%s, %d) \n",__func__, __LINE__);

    krhino_mutex_lock(&ota_sync_mutex, -1);
    uint8_t * pvalue = (uint8_t *)p_buf;

    uint32_t app_checksum =  pvalue[0]<<24 | pvalue[1]<<16 | pvalue[2]<<8 | pvalue[3];


   //printf("(%s, %d) app_checksum %x  len: %d \n",__func__, __LINE__, app_checksum, len);


    if (len >= 4)
    {
        checksum_ret = hal_ota_app_checksum(NULL, app_checksum);
    }
    krhino_mutex_unlock(&ota_sync_mutex);
    return len;
}

//#define GATT_UUID128_HILINK_SERVICE                0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA0, 0xFD, 0x00,0x00

//#define GATT_UUID128_HILINK_OTA_SERVICE          0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA6, 0xFD, 0x00,0x00
//#define GATT_UUID128_HILINK_READ_SERVICE         0xEA, 0x12, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA3, 0xFD, 0x00,0x00

static struct bt_gatt_attr hilink_ota_srv_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(HILINK_SERVICE),

    BT_GATT_CHARACTERISTIC(HILINK_OTA_AUTH_UUID,  BT_GATT_PERM_WRITE |BT_GATT_CHRC_READ),
    BT_GATT_DESCRIPTOR(HILINK_OTA_AUTH_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, hilink_ota_auth_read, hilink_ota_auth_write, NULL),


    BT_GATT_CHARACTERISTIC(HILINK_OTA_STATUS_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE ),
    BT_GATT_DESCRIPTOR(HILINK_OTA_STATUS_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, hilink_ota_status_read, hilink_ota_status_write, NULL),

    BT_GATT_CHARACTERISTIC(HILINK_OTA_VERSION_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE),
    BT_GATT_DESCRIPTOR(HILINK_OTA_VERSION_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, hilink_ota_version_read, hilink_ota_version_write, NULL),
    BT_GATT_CCC(beken_ota_ffc1_ccc_cfg, beken_ota_ffc1_ccc_cfg_changed),
    BT_GATT_CUD(NULL, BT_GATT_PERM_READ),

    BT_GATT_CHARACTERISTIC(HILINK_OTA_CHECKSUM_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE ),
    BT_GATT_DESCRIPTOR(HILINK_OTA_CHECKSUM_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, hilink_ota_checksum_read, hilink_ota_checksum_write, NULL),
};


//jx debug

//static struct bt_gatt_service _ota_srv = BT_GATT_SERVICE(_ota_srv_attrs);

static struct bt_gatt_service _ota_srv = BT_GATT_SERVICE(hilink_ota_srv_attrs);


static u8_t g_ais_adv_data[14] = {
    0xF0, 0x05, //beken
    0x85,       //vid & sub
    0x15,       //FMSK
    0x15, 0x11, 0x22, 0x33,             //PID
    0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33  //MAC
};

struct bt_data g_ais_adv[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_SOME, 0xB3, 0xFE),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, g_ais_adv_data, 14),
};

static const struct bt_data g_ais_sd[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, (sizeof(CONFIG_BT_DEVICE_NAME) - 1)),
};

struct bt_le_adv_param fast_adv_param = {
    .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),\
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,\
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,   \
    .own_addr = NULL, \
};

int g_multiadv_instant_id;
void bt_gatt_adv_init(void)
{
#ifdef CONFIG_BT_MESH_MULTIADV
    BT_DBG("");

    int err;
    genie_ais_adv_init(g_ais_adv_data, 0);

    BT_DBG("%s multi adv start\n");
    err = bt_le_multi_adv_start(&fast_adv_param, g_ais_adv, ARRAY_SIZE(g_ais_adv), g_ais_sd, ARRAY_SIZE(g_ais_sd), &g_multiadv_instant_id);
    BT_DBG("%s multi adv end %d\n", instant_id);
    if (err)
    {
        BT_ERR("Multi Advertising failed to start (err %d)\n", err);
    }
#endif
}

void ota_srv_connected(struct bt_conn *p_conn)
{
    printf("%s, %d \n", __func__, __LINE__);
    if(!otas_env)
    {
        otas_env = (struct otas_env_tag*)aos_malloc(sizeof(struct otas_env_tag));
    }

    printf("%s, %d \n", __func__, __LINE__);
    if(!otas_env)
    {
        BT_ERR("OTA env malloc error\n");
        return;
    }

    printf("%s, %d \n", __func__, __LINE__);
    memset(otas_env, 0, sizeof(struct otas_env_tag));
    otas_env->p_conn = p_conn;

    hal_flash_secure_sector(FLASH_PROTECT_SEC_64);
}

void ota_srv_disconnect(void)
{
    uint32_t offset = 0;

    if(otas_env)
    {
        ota_reset(offset, otas_env->p_conn);
        aos_free(otas_env);
        otas_env = NULL;
    }
    else
    {
        os_printf("OTA NOT INITIATED %d\n", __LINE__);
    }
}

static int ota_get_board_sec();

int ota_service_register(void)
{
    BT_DBG(" \n ota_service_register");
    uint32_t offset = 0;

    bt_gatt_adv_init();

    bt_gatt_service_register(&_ota_srv);

//jx debug
    //ota_srv_connected();

    printf("custom ver is : %d \n", CUSTOM_VERSION);
    //printf("custom ver is : %x \n", PROJECT_SW_VERSION);
    printf("(%s, %d) base addr: 0x%x \n", __func__, __LINE__, ota_get_board_sec());


    
    ota_init(offset);

    return 0;
}

void ota_init(uint32_t p_offset)
{
    hal_flash_secure_sector(FLASH_PROTECT_SEC_64);
    hal_ota_register_module(&bk3633_ota_module);

    hal_ota_init(&p_offset);
    hal_flash_secure_sector(FLASH_PROTECT_ALL);
}

void ota_reset(uint32_t p_offset, struct bt_conn *p_conn)
{
    bt_conn_disconnect(p_conn, BT_HCI_ERR_UNSPECIFIED);

    //bt_conn_disconnect(p_conn, BT_HCI_ERR_UNSPECIFIED);
    hal_ota_deinit(NULL);

    ota_init(p_offset);
}

void ota_mutex_init(void)
{
    krhino_mutex_create(&ota_sync_mutex, "hilink_ota_mutex");
    hal_ota_mutex_init(NULL);
}

#ifdef CONFIG_OTA_DUAL_SWITCH
static int ota_get_board_sec()
{
    uint32_t base_addr = *((volatile uint32_t *)0x400000);
    base_addr = base_addr * 0x22 / 0x20;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( HAL_PARTITION_APPLICATION );
    if(partition_info->partition_start_addr == base_addr)
    {
        return HAL_PARTITION_APPLICATION;
    }

    partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    if(partition_info->partition_start_addr == base_addr)
    {
        return HAL_PARTITION_OTA_TEMP;
    }

    LOG("FAIL! %s base_addr 0x%x", __func__, base_addr);

    return -1;
}
#endif
