#ifndef _BT_DFU_EXPORT_H
#define _BT_DFU_EXPORT_H
#include <stdint.h>

#define OTA_BREEZE_FW_VER_LEN              (8)

typedef enum {
    OTA_CMD = 1,
    OTA_EVT,
} ota_info_type_t;

typedef enum {
   ALI_OTA_ON_AUTH_EVT,
   ALI_OTA_ON_TX_DONE,
   ALI_OTA_ON_DISCONNECTED,
   ALI_OTA_ON_DISCONTINUE_ERR,
} ali_ota_evt_type_re_t;

typedef struct {
    uint8_t  cmd;
    uint8_t  frame;
    uint8_t  data[256];
    uint16_t len;
} breeze_ota_cmd_t;

typedef struct {
    uint8_t evt;
    uint8_t d;
} breeze_ota_evt_t;

typedef struct {
    ota_info_type_t type;
    union {
        breeze_ota_cmd_t m_cmd;
        breeze_ota_evt_t m_evt;
    } cmd_evt;
} breeze_otainfo_t;

typedef void (*ota_breeze_get_data_t) (breeze_otainfo_t* breeze_info);

typedef struct _ota_ble_version_{
    unsigned char fw_ver[OTA_BREEZE_FW_VER_LEN];
    unsigned int fw_ver_len;
}ota_breeze_version_t;

typedef struct _ota_ble_service_dat{
    unsigned char is_ota_enable;
    ota_breeze_version_t verison;
    ota_breeze_get_data_t get_dat_cb;
}ota_breeze_service_manage_t;


int ota_breeze_service_init(ota_breeze_service_manage_t* ota_manage);


#endif
