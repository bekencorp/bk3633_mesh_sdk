/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <port/mesh_hal_ble.h>

#include "genie_app.h"
#include "ota_module.h"
#include "hal_ota.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_OTA)
#include "common/log.h"

static struct otas_env_tag* otas_env;

static struct bt_gatt_ccc_cfg beken_ota_ffc1_ccc_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg beken_ota_ffc2_ccc_cfg[BT_GATT_CCC_MAX] = {};


uint8_t ota_get_firmware_type(void)
{
    return otas_env->config_para.ota_firmware_type;
}

uint8_t ota_set_firmware_type(uint8_t type)
{
    otas_env->config_para.ota_firmware_type = type;
}

bool ota_update_complete_get(void)
{
    return otas_env->config_para.update_comp;
}

bool ota_update_complete_set(void)
{
    otas_env->config_para.update_comp = 1;
}

bool ota_update_start_set(void)
{
    otas_env->config_para.update_start = 1;
}

bool ota_update_abort(void)
{
    BT_DBG("");
    otas_env->config_para.update_start = 0;
}

bool ota_update_in_progress(void)
{
    return otas_env->config_para.update_start;
}

/*********************************************************************
 * @fn      otaImgIdentifyReq
 *
 * @brief   Process the Image Identify Request.
 *
 * @return  None
 */
static void otaImgBlockReq(const struct bt_gatt_attr *p_attr, uint16_t blkNum)
{
    uint8_t noti[2];

    noti[0] = LO_UINT16(blkNum);
    noti[1] = HI_UINT16(blkNum);

    if (otas_env->p_conn) {
        bt_gatt_notify(otas_env->p_conn, p_attr, noti, sizeof(noti));
    }
}

/*********************************************************************
 * @fn      otaImgBlockWrite
 *
 * @brief   Process the Image Block Write.
 *
 * @param   connHandle - connection message was received on
 * @param   pValue - pointer to data to be written
 *
 * @return  status
 */
uint8_t otaImgBlockWrite(const struct bt_gatt_attr *p_attr, uint8_t *pValue, u16_t len)
{
    if(!otas_env)
    {
        BT_DBG("OTA not CONNECTED");
        return 1;
    }

    static uint16_t  validBlockCnt = 0x0 ;
    uint16_t* otaBlkNum = &otas_env->config_para.otaBlkNum;
    uint16_t* otaBlkTot = &otas_env->config_para.otaBlkTot;

    uint16_t blkNum = READ16B(pValue);

    if(blkNum*100/(*otaBlkTot) > (blkNum-1)*100/(*otaBlkTot))
    {
        BT_DBG("OTA update %%%d\r\n", blkNum*100/(*otaBlkTot));
    }

    if(!ota_update_in_progress())
    {
        BT_DBG("OTA not started yet\r\n");
        return ( 0x03 );
    }

    if (*otaBlkNum == blkNum)
    {
        if(*otaBlkNum != 0)     //the first block is header message.
        {
            hal_ota_save(NULL, pValue + sizeof(blkNum), len - sizeof(blkNum));
        }

        if (*otaBlkNum == (*otaBlkTot) - 1)
        {
            BT_DBG("Transmission finish \r\n");
            ota_terminate_type_e termi_type = OTA_TERMI_FINISH;
            hal_ota_set_boot(NULL, &termi_type);
        }
        else
        {
            (*otaBlkNum)++;
        }
    
        if(*otaBlkNum == (*otaBlkTot) - 2)
        {
            BT_DBG("last block \r\n");
            otaImgBlockReq(p_attr, *otaBlkNum - 1);
        } 
    }
    else
    {
        otaImgBlockReq(p_attr, *otaBlkNum);
    }

    return ( 0x00 );
}

/*********************************************************************
 * @fn      otaImgIdentifyReq
 *
 * @brief   Process the Image Identify Request.
 *
 * @param   connHandle - connection message was received on
 * @param   pImgHdr - Pointer to the ota_hdr_para_t data to send.
 *
 * @return  None
 */
static void otaImgIdentifyReq(const struct bt_gatt_attr *p_attr, ota_hdr_para_t *pImgHdr)
{
    uint8_t msg[10];

    BT_DBG("");

    msg[0] = LO_UINT16(pImgHdr->ver);
    msg[1] = HI_UINT16(pImgHdr->ver);

    msg[2] = LO_UINT16(pImgHdr->len);
    msg[3] = HI_UINT16(pImgHdr->len);

    msg[4] = LO_UINT16((uint16_t)pImgHdr->uid);
    msg[5] = HI_UINT16((uint16_t)pImgHdr->uid);

    msg[6] = LO_UINT16((uint16_t)(pImgHdr->uid>>16));
    msg[7] = HI_UINT16((uint16_t)(pImgHdr->uid>>16));

    msg[8] = LO_UINT16(pImgHdr->rom_ver);
    msg[9] = HI_UINT16(pImgHdr->rom_ver);

    if (otas_env->p_conn) {
        bt_gatt_notify(otas_env->p_conn, p_attr, msg, sizeof(msg));
    }

}

void beken_get_image_header(beken_image_hdr* img_hdr)
{
    int32_t offset = -0x10;

    hal_flash_read(HAL_PARTITION_APPLICATION, &offset, img_hdr, sizeof(beken_image_hdr));
}

uint8_t otaImgIdentifyWrite( const struct bt_gatt_attr *p_attr, uint16_t length, uint8_t *pValue )
{
    if(!otas_env)
    {
        BT_DBG("OTA not CONNECTED");
        return 1;
    }

    ota_hdr_para_t* img_hdr = &otas_env->img_hdr;
    uint16_t* otaBlkNum = &otas_env->config_para.otaBlkNum;
    uint16_t* otaBlkTot = &otas_env->config_para.otaBlkTot;
    beken_image_hdr saved_tag;
    int ota_check_res = -1;

    if(ota_update_in_progress())
    {
        uint32_t offset = 0;
        ota_update_abort();
        ota_reset(offset, otas_env->p_conn);
    }

    if(length < 16)
    {
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

        BT_DBG("ver = 0x%x, len = 0x%x", saved_tag.ver, saved_tag.len);
        BT_DBG("rom_ver = 0x%x, uid = 0x%x", saved_tag.rom_ver, saved_tag.uid);

        otaImgIdentifyReq(p_attr, img_hdr);
        return 0;
    }

    saved_tag = *(beken_image_hdr*)pValue;

    img_hdr->ver = saved_tag.ver;
    img_hdr->len = saved_tag.len;
    img_hdr->uid = saved_tag.uid;
    img_hdr->rom_ver = saved_tag.rom_ver;
    BT_DBG("RX ver = 0x%x, len = 0x%x", img_hdr->ver, img_hdr->len);
    BT_DBG("RX uid = 0x%x, rom_ver = 0x%x", img_hdr->uid, img_hdr->rom_ver);

    if (img_hdr->uid == OTA_APP_PART_UID)
    {
        ota_set_firmware_type(OTA_FM_TYPE_APP);
        BT_DBG("app part upgrade");
    }
    else if (img_hdr->uid == OTA_APP_STACK_UID)
    {
        ota_set_firmware_type(OTA_FM_TYPE_STACK);
        BT_DBG("app and stack upgrade");
    }
    else
    {
        ota_set_firmware_type(OTA_FM_TYPE_NONE);
    }

    ota_check_res = hal_ota_tag_check(NULL, ota_get_firmware_type(), img_hdr->ver, img_hdr->rom_ver, (uint32_t)img_hdr->len * HAL_FLASH_WORD_SIZE - 16);

    if (!ota_check_res && !ota_update_in_progress())
    {
        BT_DBG("Start upgrade");
        *otaBlkNum = 0;
        *otaBlkTot = img_hdr->len / (OTA_BLOCK_SIZE / HAL_FLASH_WORD_SIZE);
        BT_DBG("Image size 0x%x, Total block %d", img_hdr->len, *otaBlkTot);

        ota_update_start_set();

        //update ota connect parameter.
        struct bt_le_conn_param param;
        param.interval_min = OTA_INIT_CONN_INT_MIN;
        param.interval_max = OTA_INIT_CONN_INT_MAX;
        param.latency = OTA_INIT_CONN_LATENCY;
        param.timeout = OTA_INIT_CONN_TO;
        int ret = bt_conn_le_param_update(otas_env->p_conn, &param);
        BT_DBG("update le para ret %d", ret);

        otaImgBlockReq(otas_env->fcc2_attr, *otaBlkNum);
    }
    else
    {
        BT_DBG("Tag check fail %d, %d", ota_check_res, ota_update_in_progress());
    }

    return ( 0x00 );//SUCCESS
}

static ssize_t beken_ota_ffc1_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{

    u8_t value[OTAS_FFC1_DATA_LEN] = {0};
    u16_t length = 0;

    length = OTAS_FFC1_DATA_LEN * sizeof(uint8_t);

    BT_DBG("length %d", length);

    memcpy(value, otas_env->ffc1_value, length);

    return bt_gatt_attr_read(p_conn, p_attr, buf, len, offset, value, length);
}

static ssize_t beken_ota_ffc2_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        void *buf, u16_t len, u16_t offset)
{
    BT_DBG("");

    u8_t value[OTAS_FFC2_DATA_LEN] = {0};
    u16_t length = 0;

    return bt_gatt_attr_read(p_conn, p_attr, buf, len, offset, value, sizeof(*value));
}


static ssize_t beken_ota_ffc1_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_DBG("len %d: %s", len, bt_hex(p_buf, len));

    otaImgIdentifyWrite(p_attr, len, (uint8_t *)p_buf);
    return len;
}

static ssize_t beken_ota_ffc2_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                        const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    otaImgBlockWrite(p_attr, (uint8_t *)p_buf, len);
    return len;
}

static void beken_ota_ffc1_ccc_cfg_changed(const struct bt_gatt_attr *p_attr, struct bt_conn *conn, uint16_t value)
{
    BT_DBG("value %d", value);

    if(value)
    {
        ota_srv_connected(conn);
    }
    else
    {
        ota_srv_disconnect();
    }
}

static void beken_ota_ffc2_ccc_cfg_changed(const struct bt_gatt_attr *p_attr, struct bt_conn *conn, uint16_t value)
{
    BT_DBG("value %d", value);
}

/* BEKEN OTA Service Declaration */
static struct bt_gatt_attr _ota_srv_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BEKEN_OTA_SERVICE_UUID),

    BT_GATT_CHARACTERISTIC(BEKEN_OTA_IMG_IDENTIFY_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY),
    BT_GATT_DESCRIPTOR(BEKEN_OTA_IMG_IDENTIFY_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, beken_ota_ffc1_read, beken_ota_ffc1_write, NULL),
    BT_GATT_CCC(beken_ota_ffc1_ccc_cfg, beken_ota_ffc1_ccc_cfg_changed),
    BT_GATT_CUD(NULL, BT_GATT_PERM_READ),

    BT_GATT_CHARACTERISTIC(BEKEN_OTA_IMG_BLOCK_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY),
    BT_GATT_DESCRIPTOR(BEKEN_OTA_IMG_BLOCK_UUID, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, beken_ota_ffc2_read, beken_ota_ffc2_write, NULL),
    BT_GATT_CCC(beken_ota_ffc2_ccc_cfg, beken_ota_ffc2_ccc_cfg_changed),
    BT_GATT_CUD(NULL, BT_GATT_PERM_READ),
};

static struct bt_gatt_service _ota_srv = BT_GATT_SERVICE(_ota_srv_attrs);

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
    if(!otas_env)
    {
        otas_env = (struct otas_env_tag*)aos_zalloc(sizeof(struct otas_env_tag));
    }

    if(!otas_env)
    {
        BT_ERR("OTA env malloc error\n");
        return;
    }

    memset(otas_env, 0, sizeof(struct otas_env_tag));

    otas_env->config_para.otaBlkNum = 0;
    otas_env->config_para.otaBlkTot = 0xFFFF;
    otas_env->p_conn = p_conn;
    otas_env->fcc1_attr = &_ota_srv_attrs[2];
    otas_env->fcc2_attr = &_ota_srv_attrs[6];

    hal_flash_secure_sector(FLASH_PROTECT_SEC_64);
}

void ota_srv_disconnect(void)
{
    uint32_t offset = 0;
    ota_reset(offset, otas_env->p_conn);

    if(otas_env)
    {
        aos_free(otas_env);
        otas_env = NULL;
    }

    hal_flash_secure_sector(FLASH_PROTECT_SEC_120);
}

int ota_service_register(void)
{
    BT_DBG("");
    uint32_t offset = 0;

    bt_gatt_adv_init();

    bt_gatt_service_register(&_ota_srv);

    ota_init(offset);

    return 0;
}

void ota_init(uint32_t p_offset)
{
    hal_ota_register_module(&bk3633_ota_module);

    hal_ota_init(&p_offset);
}

void ota_reset(uint32_t p_offset, struct bt_conn *p_conn)
{
    bt_conn_disconnect(p_conn, BT_HCI_ERR_UNSPECIFIED);

    hal_ota_deinit(NULL);

    ota_init(p_offset);
}