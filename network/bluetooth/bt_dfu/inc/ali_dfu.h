#ifndef ALI_DFU_H
#define ALI_DFU_H
#include <api/mesh.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "mesh_def.h"

#define ALI_PRODUCT_ID       463
#define ALI_VERSION_ID       0x00000001
#define ALI_SECRET_KEY       "c33fc1c2b6a28b85d0933cc72e5c274c"//"362bdeb7e820d40960ac45b5593e58c0"
#define ALI_SECRET_LEN       32
#define ALI_AIS_SUPPORT      1

#define DFU_AUTO_BETWEEN_DEVICES                0
#define DFU_PRODUCT_ID                          DFU_PRODUCT_ID_MESH_ALI_LIGHT
#define DFU_APP_VERSION                         0x00000000
#define DFU_PATCH_VERSION                       ({T_IMG_HEADER_FORMAT *p_header = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(RomPatch);\

/* sig mesh defined ids */
#define COMPANY_ID                              0x01A8
#define PRODUCT_ID                              0x0000
#define VERSION_ID                              0x0000

/* mesh ota defined ids */
#define DFU_UPDATER_FW_ID                       ALI_VERSION_ID

/** @brief  Index of each characteristic in service database. */
#define AIS_READ_INDEX                          0x02
#define AIS_WRITE_INDEX                         0x04
#define AIS_INDICATE_INDEX                      0x06
#define AIS_INDICATE_CCCD_INDEX                 0x07
#define AIS_WRITE_WO_RESP_INDEX                 0x09
#define AIS_NOTIFY_INDEX                        0x0B
#define AIS_NOTIFY_CCCD_INDEX                   0x0C

enum
{
    AIS_IMAGE_TYPE_APP,
    AIS_IMAGE_TYPE_PATCH,
    AIS_IMAGE_TYPE_MAX
} ;
typedef uint8_t ais_image_type_t;

typedef enum
{
    AIS_BT_VER_BLE4_0,
    AIS_BT_VER_BLE4_2,
    AIS_BT_VER_BLE5_0,
    AIS_BT_VER_BLE5_PLUS
} ais_bt_ver_t;

typedef struct
{
    uint8_t bt_ver: 2; /**< @ref ais_bt_ver_t */
    uint8_t ota: 1;
    uint8_t secure: 1;
    uint8_t enc: 1;
    uint8_t adv: 1;
    uint8_t rfu: 2;
} __attribute__ ((packed)) ais_fmsk_t;

typedef struct
{
    uint8_t vid;
    union
    {
        uint8_t fmsk;
        ais_fmsk_t _fmsk;
    };
    uint32_t pid;
    uint8_t mac[6];
} __attribute__ ((packed)) ais_adv_data_t;

enum
{
    AIS_DEVICE_REPORT = 0x1,
    AIS_APK_REQ = 0x2,
    AIS_DEVICE_RESP = 0x3,
    AIS_DEVICE_EXCEPTION = 0xf,
    AIS_OTA_GET_VER = 0x20,
    AIS_OTA_REPORT_VER = 0x21,
    AIS_OTA_UPD_REQ = 0x22,
    AIS_OTA_UPD_RESP = 0x23,
    AIS_OTA_FRAME_INFO = 0x24,
    AIS_OTA_FW_INFO_REQ = 0x25,
    AIS_OTA_FW_INFO = 0x26,
    AIS_OTA_FW_DATA = 0x2f,
    AIS_CMD_MAX = 0xff
} ;
	
typedef uint8_t ais_cmd_t;

typedef struct
{
    uint8_t msg_id: 4;
    uint8_t enc: 1;
    uint8_t ver: 3;
    ais_cmd_t cmd;
    uint8_t frame_seq: 4;
    uint8_t frame_num: 4;
    uint8_t frame_len;
}  __attribute__ ((packed))  ais_header_t;

typedef struct
{
    ais_image_type_t image_type;
} __attribute__ ((packed)) ais_ota_get_ver_t;

typedef struct
{
    ais_image_type_t image_type;
    uint32_t ver;
} __attribute__ ((packed)) ais_ota_report_ver_t;

enum
{
    AIS_OTA_TYPE_FULL,
    AIS_OTA_TYPE_INCREMENT
} ;
	
typedef uint8_t ais_ota_type_t;

typedef struct
{
    ais_image_type_t image_type;
    uint32_t ver;
    uint32_t fw_size;
    uint16_t crc16;
    ais_ota_type_t ota_type;
} __attribute__ ((packed)) ais_ota_upd_req_t;

typedef struct
{
    uint8_t state;
    uint32_t rx_size;
} __attribute__ ((packed)) ais_ota_upd_resp_t;

typedef struct
{
    uint8_t frame_seq: 4;
    uint8_t frame_num: 4;
    uint32_t rx_size;
} __attribute__ ((packed)) ais_ota_frame_info_t;

typedef struct
{
    uint8_t state;
}  __attribute__ ((packed))  ais_ota_fw_info_req_t;

typedef struct
{
    uint8_t state;
} __attribute__ ((packed)) ais_ota_fw_info_t;

typedef struct
{
    uint8_t state;
} __attribute__ ((packed)) ais_ota_upd_info_t;

typedef struct
{
    ais_header_t header;
    union
    {
        uint8_t payload[1];
        ais_ota_get_ver_t ota_get_ver;
        ais_ota_report_ver_t ota_report_ver;
        ais_ota_upd_req_t ota_upd_req;
        ais_ota_upd_resp_t ota_upd_resp;
        ais_ota_frame_info_t ota_frame_info;
        ais_ota_fw_info_t ota_fw_info;
        ais_ota_upd_info_t ota_upd_info;
        ais_ota_fw_info_req_t ota_fw_info_req;
    };
} __attribute__ ((packed)) ais_pdu_t;

typedef struct
{
	uint32_t flash_id;
	uint32_t write_size;	
	uint32_t erase_size;
	uint32_t ota_tmp_size;	
}ais_hal_port_t;

typedef ssize_t (*ais_write_t)(struct bt_conn *conn,
				   const struct bt_gatt_attr *attr,
				   const void *buf, u16_t len,
				   u16_t offset, u8_t flags);

typedef ssize_t (*ais_read_t)(bt_mesh_conn_t conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, u16_t len, u16_t offset);

typedef struct ais_dfu_port_cb
{
    ais_write_t   write;
    ais_write_t   write_nc;     
    ais_read_t    read;     
    ais_read_t    indicate;       
    ais_read_t    notify;     
} ais_dfu_port_cb_t;

typedef enum
{
    AIS_CB_READ,
    AIS_CB_WRITE_REQ,
    AIS_CB_OTA,
} ais_cb_type_t;

typedef enum
{
    AIS_OTA_START,
    AIS_OTA_GOING, //!< with
    AIS_OTA_SUCCESS,
    AIS_OTA_FAIL
} ais_ota_state_t;

typedef union
{
    struct
    {
        uint16_t *p_length;
        uint8_t **pp_value;
    } read;
    struct
    {
        ais_pdu_t *pmsg;
        uint16_t msg_len;
    } write_req;
    struct
    {
        ais_ota_state_t state;
        int8_t progress;
    } ota;
} ais_cb_data_t;

/** service data to inform application */
typedef struct
{
    uint8_t conn_id;
    ais_cb_type_t type;
    ais_cb_data_t data;
} ais_cb_msg_t;
void ali_dfu_init(void);

#endif
