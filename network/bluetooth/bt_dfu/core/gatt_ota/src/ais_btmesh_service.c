#include <zephyr.h>
#include <misc/byteorder.h>
#include "ais_btmesh_service.h"
#include <gatt.h>
#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <api/mesh.h>
#include <errno.h>
#include <misc/printk.h>

#include "mesh_def.h"

#include "ais_common.h"
#include "bt_mesh_custom_log.h"
#include "common/log.h"
#include "ali_dfu_port.h"
#include "ali_dfu.h"
#include <stdbool.h>

static struct bt_gatt_ccc_cfg ais_ic_ccc_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg ais_nc_ccc_cfg[BT_GATT_CCC_MAX] = {};

ble_ais_t g_ais;

static const uint16_t ais_image_id[AIS_IMAGE_TYPE_MAX] = {0x00, 0x01};

#define ALI_PRODUCT_ID 0x1
#define AIS_DEMO    0

struct
{
    struct
    {
        uint16_t crc16;
        uint8_t frame_seq: 4;
        uint8_t frame_num: 4;
        uint16_t image_id;
        uint32_t image_size;
        uint32_t image_ver;
        uint32_t rx_size;
        uint8_t frame_remainder_len;
        uint8_t frame_remainder[4];
    } ota;
    void *timer;
} ais_server_ctx;
//AIS ADV
extern ais_hal_port_t g_ais_port;


typedef enum
{
    AIS_CCC_VALUE_NONE     = 0,
    AIS_CCC_VALUE_NOTIFY   = 1,
    AIS_CCC_VALUE_INDICATE = 2
} ais_ccc_value_t;
	

#if 0
static u8_t g_ais_btmesh_data[15] = 
{ 
	0xa8, 0x01,
	0x85, AIS_BT_VER_BLE4_2,
	0x15,
	0x1,
	0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33								
};

static const struct bt_data g_ais_ota_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xB3, 0xFE),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, g_ais_btmesh_data, 15),
};

static const struct bt_le_adv_param fast_adv_param = {
    .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
};

static const struct bt_le_adv_param slow_adv_param = {
    .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
    .interval_min = BT_GAP_ADV_SLOW_INT_MIN,
    .interval_max = BT_GAP_ADV_SLOW_INT_MAX,
};
	
#define CONFIG_BT_DEVICE_NAME1 "wangsen"
static const struct bt_data prov_sd1[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME1,
    (sizeof(CONFIG_BT_DEVICE_NAME1) - 1)),
};

uint32_t bt_mesh_ais_adv_start()
{

    if (bt_mesh_adv_start(BT_LE_ADV_CONN, g_ais_ota_ad, ARRAY_SIZE(g_ais_ota_ad),
                           NULL, 0/*ARRAY_SIZE(prov_sd1)*/) == 0) {

        /* Advertise 60 seconds using fast interval */
//            return K_SECONDS(60);
    }
//    return bt_prov_active() ? K_SECONDS(10) : K_SECONDS(30);
}

void bt_mesh_ais_adv_stop(void)
{
    int err;

    err = bt_mesh_adv_stop();
    if (err) {
        BT_ERR("Failed to stop advertising (err %d)", err);
    } else {
    }
}
#endif

static void service_enabled(void)
{
    if (g_ais.is_indication_enabled && g_ais.is_notification_enabled) {
        BT_INFO("Let's notify that service is enabled.\r\n");
    }
}

static int conn_count;
static bool ais_adv_enabled;

static void ais_connected(bt_mesh_conn_t conn, u8_t err)
{
    BT_DBG("conn %p err 0x%02x", conn, err);

    g_ais.conn_handle = BLE_CONN_HANDLE_MAGIC;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;
	
    printk("enter connected\n");

    conn_count++;

    /* Since we use ADV_OPT_ONE_TIME */
    ais_adv_enabled = false;

    /* Try to re-enable advertising in case it's possible */
    if (conn_count < CONFIG_BT_MAX_CONN) {
        //bt_mesh_adv_update();
    }
	bt_mesh_ais_adv_stop();

}

static void ais_disconnected(bt_mesh_conn_t conn, u8_t reason)
{
    int i;

    g_ais.conn_handle = BLE_CONN_HANDLE_INVALID;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;
    
    BT_INFO("enter disconnected\n");

    BT_DBG("conn %p reason 0x%02x", conn, reason);

    conn_count--;
	
	bt_mesh_ais_adv_start();
	
}

static ssize_t ais_btmesh_server_read(bt_mesh_conn_t conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, u16_t len, u16_t offset)
{
    u16_t *value = attr->user_data;
	
    BT_INFO("ota_ccc_read len %u: %s", len, bt_hex(buf, len));

    return bt_mesh_gatt_attr_read(conn, attr, buf, len, offset, value,
                                  sizeof(*value));
}

static ssize_t ais_btmesh_server_read_cb( bt_mesh_conn_t conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, u16_t len, u16_t offset)
{
}

//#if defined(CONFIG_BT_MESH_GATT_OTA)
static ssize_t ais_btmesh_service_write(bt_mesh_conn_t conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, u16_t len,
                             u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_client *client;
    u16_t value;

    BT_INFO("len %u: %s", len, bt_hex(buf, len));

    if (len != sizeof(value))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    value = sys_get_le16(buf);
    if (value != BT_GATT_CCC_NOTIFY)
    {
        BT_WARN("Client wrote 0x%04x instead enabling notify", value);
        return len;
    }

	return len;
}
							 
uint8_t aliota_index = 0;
/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param index          Attribute index of characteristic.
 * @param len            length of value to be written.
 * @param pvalue            value to be written.
 * @return Profile procedure result
 */ 
 
static ssize_t ais_btmesh_service_write_nr(bt_mesh_conn_t conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, u16_t len,
                             u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_client *client;
    u16_t value;

    //BT_DBG("len %u: %s", len, bt_hex(buf, len));

	if(len != 0)
	{
		ais_server_handle_msg(conn, (ais_pdu_t *)buf, len);
	}

	if (len != sizeof(value))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    value = sys_get_le16(buf);
    if (value != BT_GATT_CCC_NOTIFY)
    {
        BT_WARN("Client wrote 0x%04x instead enabling notify", value);
        return len;
    }
	
    return len;

}

static void indicate_ccc_handler(bt_mesh_conn_t conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, u16_t len, u16_t offset )
{
	const char *value = attr->user_data;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

static void notify_ccc_handler( bt_mesh_conn_t conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, u16_t len, u16_t offset)
{
	const char *value = attr->user_data;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));

}

static void ais_nc_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                   uint16_t                   value)
{
    ais_ccc_value_t val;

    switch (value) {
        case BT_GATT_CCC_NOTIFY:
            printf("CCC cfg changed to NOTIFY (%d).\r\n", value);
            val = AIS_CCC_VALUE_NOTIFY;
            break;
        default:
            printf("%s CCC cfg changed to %d.\r\n", __func__, value);
            val = AIS_CCC_VALUE_NONE;
            break;
    }

}

static void ais_ic_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                   uint16_t                   value)
{
    ais_ccc_value_t val;

    switch (value) {
        case BT_GATT_CCC_INDICATE:
            printf("CCC cfg changed to INDICATE (%d).\r\n", value);
            val = AIS_CCC_VALUE_INDICATE;
            break;
        default:
            printf("%s CCC cfg changed to %d.\r\n", __func__, value);
            val = AIS_CCC_VALUE_NONE;
            break;
    }

}

/* AIS OTA Service Declaration */
static struct bt_gatt_attr ota_attrs[] = {

    BT_GATT_PRIMARY_SERVICE(AIS_SERVICE_UUID),

    BT_GATT_CHARACTERISTIC(AIS_READ_UUID, BT_GATT_CHRC_READ),
    BT_GATT_DESCRIPTOR(AIS_READ_UUID, BT_GATT_PERM_READ, ais_btmesh_server_read, NULL, NULL),

    BT_GATT_CHARACTERISTIC(AIS_WRITE_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE),
    BT_GATT_DESCRIPTOR(AIS_WRITE_UUID, BT_GATT_PERM_WRITE, \
    NULL, ais_btmesh_service_write, NULL),

	BT_GATT_CHARACTERISTIC(AIS_INDICATE_UUID, BT_GATT_CHRC_READ|BT_GATT_CHRC_INDICATE),
    BT_GATT_DESCRIPTOR(AIS_INDICATE_UUID,  BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, \
    indicate_ccc_handler, NULL, NULL),
	BT_GATT_CCC(ais_ic_ccc_cfg, ais_ic_ccc_cfg_changed),

	BT_GATT_CHARACTERISTIC(AIS_WRITE_WO_RESP_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP),
    BT_GATT_DESCRIPTOR(AIS_WRITE_WO_RESP_UUID, BT_GATT_PERM_WRITE, \
    NULL, ais_btmesh_service_write_nr, NULL),

	BT_GATT_CHARACTERISTIC(AIS_NOTIFY_UUID, BT_GATT_CHRC_READ |  BT_GATT_CHRC_NOTIFY),
    BT_GATT_DESCRIPTOR(AIS_NOTIFY_UUID, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, \
    notify_ccc_handler, NULL, NULL),
    
	BT_GATT_CCC(ais_nc_ccc_cfg, ais_nc_ccc_cfg_changed),
};

static struct bt_gatt_service ota_svc = BT_GATT_SERVICE(ota_attrs);

int ais_btmesh_service_enable(void)
{
    int i;

    BT_DBG("ais_btmesh_service_enabled");

    bt_mesh_gatt_service_register(&ota_svc);
	    
    return 0;
}

void bt_mesh_ota_gatt_disconnect(void)
{
    int i;

}

int bt_mesh_ota_gatt_disable(void)
{
    BT_DBG("");

    bt_mesh_proxy_gatt_disconnect();
    bt_mesh_gatt_service_unregister(&ota_svc);

    return 0;
}

bool ais_server_send_device_report(uint8_t conn_id, ais_pdu_t *pmsg, uint16_t len)
{
    pmsg->header.cmd = AIS_DEVICE_REPORT;
    pmsg->header.msg_id = 0;
    pmsg->header.enc = 0;
    pmsg->header.ver = 0;
    pmsg->header.frame_seq = 0;
    pmsg->header.frame_num = 0;
    pmsg->header.frame_len = len - sizeof(ais_header_t);
    return ais_server_indicate(conn_id, (uint8_t *)pmsg, len);
}

bool ais_server_send_device_resp(uint8_t conn_id, ais_pdu_t *pmsg, uint16_t len, uint8_t msg_id)
{
    pmsg->header.cmd = AIS_DEVICE_RESP;
    pmsg->header.msg_id = msg_id;
    pmsg->header.enc = 0;
    pmsg->header.ver = 0;
    pmsg->header.frame_seq = 0;
    pmsg->header.frame_num = 0;
    pmsg->header.frame_len = len - sizeof(ais_header_t);
    return ais_server_indicate(conn_id, (uint8_t *)pmsg, len);
}

bool ais_server_send_device_exception(uint8_t conn_id, uint8_t msg_id)
{
    ais_pdu_t msg;
    msg.header.msg_id = msg_id;
    msg.header.enc = 0;
    msg.header.ver = 0;
    msg.header.cmd = AIS_DEVICE_EXCEPTION;
    msg.header.frame_seq = 0;
    msg.header.frame_num = 0;
    msg.header.frame_len = 0;
    return ais_server_indicate(conn_id, (uint8_t *)&msg, sizeof(ais_header_t));
}

bool ais_server_send_ota_msg(uint8_t conn_id, ais_pdu_t *pmsg, uint16_t len)
{
    pmsg->header.msg_id = 0;
    pmsg->header.enc = 0;
    pmsg->header.ver = 0;
    pmsg->header.frame_seq = 0;
    pmsg->header.frame_num = 0;
    pmsg->header.frame_len = len - sizeof(ais_header_t);
    return ais_server_notify(conn_id, (uint8_t *)pmsg, len);
}

uint32_t ais_server_get_image_ver(ais_image_type_t image_type)
{
    uint32_t ver;
    switch (image_type)
    {
    case AIS_IMAGE_TYPE_APP:
        ver = ALI_VERSION_ID;
        break;
    case AIS_IMAGE_TYPE_PATCH:
        //ver = DFU_PATCH_VERSION;
        break;
    default:
        ver = 0xffffffff;
        break;
    }
    return ver;
}

void ais_server_handle_msg(uint8_t conn_id, ais_pdu_t *pmsg, uint16_t len)
{
    bool ret =  false;
    ais_pdu_t resp;
#if 0	
    BT_DBG("ais_server_handle_msg: image id 0x%04x, size %d/%d, remain %d, expected frame seq %d, conn_id %d, cmd 0x%x, len %d, pmsg =",
           ais_server_ctx.ota.image_id, ais_server_ctx.ota.rx_size, ais_server_ctx.ota.image_size,
           ais_server_ctx.ota.frame_remainder_len, ais_server_ctx.ota.frame_seq, conn_id, pmsg->header.cmd,
           len);
	BT_DBG("pmsg->header.cmd 0x%x\n", pmsg->header.cmd);
#endif
	switch (pmsg->header.cmd)
    {
    case AIS_OTA_GET_VER:
		
//		BT_DBG("%d %s len :0x%x, BB 0x%x\n", __LINE__, __func__, len, sizeof(ais_header_t) + sizeof(ais_ota_get_ver_t));
        if (len == sizeof(ais_header_t) + sizeof(ais_ota_get_ver_t))
        {
            ret = true;
            resp.header.cmd = AIS_OTA_REPORT_VER;
            resp.ota_report_ver.image_type = pmsg->ota_get_ver.image_type;
            resp.ota_report_ver.ver = ais_server_get_image_ver(pmsg->ota_get_ver.image_type);
			
            ais_server_send_ota_msg(conn_id, &resp, sizeof(ais_header_t) + sizeof(ais_ota_report_ver_t));
        }
        break;
    case AIS_OTA_UPD_REQ:
        if (len == sizeof(ais_header_t) + sizeof(ais_ota_upd_req_t))
        {
            ret = true;
			
			BT_DBG("update image ver: 0x%x tmp_size: 0x%x fw_size :0x%x, our image ver: 0x%x tmp_size: 0x%x\n",  pmsg->ota_upd_req.ver,
				pmsg->ota_upd_req.fw_size, pmsg->ota_upd_req.fw_size,ais_server_get_image_ver(pmsg->ota_upd_req.image_type),g_ais_port.ota_tmp_size);
            if (pmsg->ota_upd_req.ver <= ais_server_get_image_ver(pmsg->ota_upd_req.image_type) ||
                g_ais_port.ota_tmp_size < pmsg->ota_upd_req.fw_size || 0 == pmsg->ota_upd_req.fw_size
                || (pmsg->ota_upd_req.fw_size & 0x3) != 0)
            {
                resp.ota_upd_resp.state = 0;
                resp.ota_upd_resp.rx_size = 0;
            }
            else
            {
                if (pmsg->ota_upd_req.ota_type == AIS_OTA_TYPE_FULL)
                {
                    ais_server_ctx.ota.rx_size = 0;
                }
                else if (ais_server_ctx.ota.rx_size != 0)
                {
                    if (ais_image_id[pmsg->ota_upd_req.image_type] != ais_server_ctx.ota.image_id ||
                        pmsg->ota_upd_req.ver != ais_server_ctx.ota.image_ver ||
                        pmsg->ota_upd_req.fw_size != ais_server_ctx.ota.image_size ||
                        pmsg->ota_upd_req.crc16 != ais_server_ctx.ota.crc16)
                    {
                        ais_server_ctx.ota.rx_size = 0;
                    }
                }

                if (ais_server_ctx.ota.rx_size == 0)
                {
                    ais_server_ctx.ota.image_id = ais_image_id[pmsg->ota_upd_req.image_type];
                    ais_server_ctx.ota.image_size = pmsg->ota_upd_req.fw_size;
                    ais_server_ctx.ota.crc16 = pmsg->ota_upd_req.crc16;
                    ais_server_ctx.ota.image_ver = pmsg->ota_upd_req.ver;
                    ais_server_ctx.ota.frame_remainder_len = 0;
                }
                ais_server_ctx.ota.frame_seq = 0;
                ais_server_ctx.ota.frame_num = 0;
                resp.ota_upd_resp.state = 1;
                resp.ota_upd_resp.rx_size = ais_server_ctx.ota.rx_size;
			}
			/* zyuan debug */
			resp.ota_upd_resp.state = 1;
            resp.header.cmd = AIS_OTA_UPD_RESP;
            ais_server_send_ota_msg(conn_id, &resp, sizeof(ais_header_t) + sizeof(ais_ota_upd_resp_t));
        }
        break;
    case AIS_OTA_FW_INFO_REQ:
        if (len == sizeof(ais_header_t) + sizeof(ais_ota_fw_info_req_t))
        {
            ret = true;
            resp.header.cmd = AIS_OTA_FW_INFO;
            resp.ota_fw_info.state = 0;
            printf("todo delete vincent: state set to 1\n");
            resp.ota_fw_info.state = 1;

            if (pmsg->ota_fw_info_req.state == 1)
            {
            	printf("wang A 0x%x,B 0x%x\n",ais_server_ctx.ota.rx_size,ais_server_ctx.ota.rx_size);
                // TODO: calculate the fw crc first
                if (ais_server_ctx.ota.image_size == ais_server_ctx.ota.rx_size)
                {
#if 1                
                    unlock_flash_all();
                    //flash_lock(FLASH_LOCK_USER_MODE_READ);
                    resp.ota_fw_info.state = dfu_check_checksum(ais_server_ctx.ota.image_id);
                    //flash_unlock(FLASH_LOCK_USER_MODE_READ);
                    resp.ota_fw_info.state = 1;
                    lock_flash();
#endif
                }
            }			
            ais_server_send_ota_msg(conn_id, &resp, sizeof(ais_header_t) + sizeof(ais_ota_fw_info_t));

            if (resp.ota_fw_info.state)
            {
                BT_INFO("dfu success, reboot!");
                unlock_flash_all();
                BT_INFO("sleep 5000\n");
                k_sleep(5000);
                dfu_reboot();
            }
        }
        break;
    case AIS_OTA_FW_DATA:
        {
            if (pmsg->header.frame_seq != ais_server_ctx.ota.frame_seq)
            {
                BT_ERR("ais_server_handle_msg: fail, frame seq expected %d, rx %d", ais_server_ctx.ota.frame_seq,
                       pmsg->header.frame_seq);
            }
            else if (pmsg->header.frame_len + sizeof(ais_header_t) != len)
            {
                BT_ERR("ais_server_handle_msg: fail, frame len declared %d, rx %d", pmsg->header.frame_len,
                       len - sizeof(ais_header_t));
            }
            else
            {
                uint8_t offset = 0;
                uint8_t *payload = pmsg->payload;
                uint16_t payload_len = pmsg->header.frame_len;
                ais_server_ctx.ota.frame_seq = (pmsg->header.frame_seq + 1) % 16;
                ais_server_ctx.ota.frame_num = pmsg->header.frame_num;

                if (ais_server_ctx.ota.frame_remainder_len)
                {
                    memcpy(ais_server_ctx.ota.frame_remainder + ais_server_ctx.ota.frame_remainder_len, pmsg->payload,
                           4 - ais_server_ctx.ota.frame_remainder_len);
                    unlock_flash_all();
                    ali_dfu_image_update(ais_server_ctx.ota.image_id,
                                   ais_server_ctx.ota.rx_size - ais_server_ctx.ota.frame_remainder_len, 4,
                                   (uint32_t *)ais_server_ctx.ota.frame_remainder);
                    lock_flash();
                    offset = 4 - ais_server_ctx.ota.frame_remainder_len;
                    payload += offset;
                    payload_len -= MIN(payload_len, offset);
                }
                ais_server_ctx.ota.frame_remainder_len = payload_len % 4;
                payload_len = payload_len & 0xfffc;
                if (payload_len)
                {
                    unlock_flash_all();
                    ali_dfu_image_update(ais_server_ctx.ota.image_id, ais_server_ctx.ota.rx_size + offset, payload_len,
                                   (uint32_t *)payload);
                    lock_flash();
                }

                if (ais_server_ctx.ota.frame_remainder_len)
                {
                    memcpy(ais_server_ctx.ota.frame_remainder, payload + payload_len,
                           ais_server_ctx.ota.frame_remainder_len);
                }
                ais_server_ctx.ota.rx_size += pmsg->header.frame_len;
			}
            ret = true;
            resp.header.cmd = AIS_OTA_FRAME_INFO;
            resp.ota_frame_info.frame_seq = (ais_server_ctx.ota.frame_seq + 15) % 16;
            resp.ota_frame_info.frame_num = ais_server_ctx.ota.frame_num;
            resp.ota_frame_info.rx_size = ais_server_ctx.ota.rx_size;
            ais_server_send_ota_msg(conn_id, &resp, sizeof(ais_header_t) + sizeof(ais_ota_frame_info_t));
        }
        break;
    default:
        break;
    }
    if (false == ret)
    {  	
        ais_server_send_device_exception(conn_id, pmsg->header.msg_id);
    }
}
							 
bool ais_server_notify(uint8_t conn_id, uint8_t *pvalue, uint16_t len)
{
    ais_pdu_t *pmsg = (ais_pdu_t *)pvalue;		
    //BT_INFO("ais_server_notify: cmd 0x%x, len %d", pmsg->header.cmd, len);
	bt_gatt_notify(NULL, &ota_attrs[11], pvalue, len);

	return true;
}

bool ais_server_indicate(uint8_t conn_id, uint8_t *pvalue, uint16_t len)
{
    ais_pdu_t *pmsg = (ais_pdu_t *)pvalue;
	
    //BT_INFO("ais_server_indicate: cmd 0x%x, len %d", pmsg->header.cmd, len);
	bt_gatt_notify(NULL, &ota_attrs[7], pvalue, len);

	return true;
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param server_id          Service ID.
 * @param index          Attribute index of characteristic data.
 * @param cccd_bits         CCCD bits from stack.
 * @return None
 */
void ais_server_cccd_update_cb(uint8_t conn_id, uint16_t index,
                               uint16_t cccd_bits)
{
    BT_INFO("ais_server_cccd_update_cb: index = %d, cccd_bits = 0x%x", index, cccd_bits);
    switch (index)
    {
    case AIS_INDICATE_CCCD_INDEX:
        break;
    case AIS_NOTIFY_CCCD_INDEX:
        break;
    default:
        break;
    }
}

/**
 * @brief Service Callbacks.
 */
static ais_dfu_port_cb_t ais_server_cbs =
{
	.write = ais_btmesh_service_write, // Write callback function pointer
 	.write_nc = ais_btmesh_service_write_nr,
	.read = ais_btmesh_server_read_cb,  // Read callback function pointer
	.indicate = ais_server_indicate,
	.notify = ais_server_notify
};

static struct bt_mesh_conn_cb conn_callbacks = {
    .connected = ais_connected,
    .disconnected = ais_disconnected,
};

uint32_t btmesh_ais_init(void)
{
    memset(&g_ais, 0, sizeof(ble_ais_t));
    g_ais.conn_handle = BLE_CONN_HANDLE_INVALID;
    g_ais.is_indication_enabled = false;
    g_ais.is_notification_enabled = false;

	ais_btmesh_service_enable();
		
    return 0;
	
}
							
