/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef _AIS_BTMESH_SERVICE_H
#define _AIS_BTMESH_SERVICE_H


#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AIS_SERVICE_UUID      	   BT_UUID_DECLARE_16(0xFEB3) 
#define AIS_READ_UUID              BT_UUID_DECLARE_16(0xFED4) 
#define AIS_WRITE_UUID             BT_UUID_DECLARE_16(0xFED5)
#define AIS_INDICATE_UUID          BT_UUID_DECLARE_16(0xFED6)
#define AIS_WRITE_WO_RESP_UUID     BT_UUID_DECLARE_16(0xFED7)
#define AIS_NOTIFY_UUID            BT_UUID_DECLARE_16(0xFED8)


#define AIS_GATT_CMD_GET_VERSION    0x20
#define AIS_GATT_CMD_REP_VERSION    0x21
#define AIS_GATT_CMD_OTA_START      0x22
#define AIS_GATT_CMD_OTA_ACK        0x23

#define AIS_GATT_CMD_REP_CAPABILITY 0x24
#define AIS_GATT_CMD_REP_STOP       0x25
#define AIS_GATT_CMD_REP_RESULT     0x26
#define AIS_GATT_CMD_TRANSFER       0x2F


typedef struct {
    uint16_t mtu;
} ble_ais_init_t;

typedef struct ble_gatts_char_handles_s {
    uint16_t chrc_handle;
    uint16_t value_handle;
    uint16_t user_desc_handle;
    uint16_t cccd_handle;
    uint16_t sccd_handle;
} ble_gatts_char_handles_t;

typedef struct ble_ais_s {
    uint16_t service_handle;
    ble_gatts_char_handles_t rc_handles;  // Handles related to Read Characteristics
    ble_gatts_char_handles_t wc_handles;  // Handles related to Write Characteristics
    ble_gatts_char_handles_t ic_handles;  // Handles related to Indicate Characteristics
    ble_gatts_char_handles_t wwnrc_handles;  // Handles related to Write WithNoRsp Characteristics
    ble_gatts_char_handles_t nc_handles;  // Handles related to Notify Characteristics
    uint16_t conn_handle;  // Handle of the current connection
    bool is_indication_enabled;
    bool is_notification_enabled;
    void *p_context;
    uint16_t max_pkt_size;
} ble_ais_t;

uint32_t btmesh_ais_init(void);

uint32_t btmesh_ais_send_notification(uint8_t * p_data, uint16_t length);

uint32_t btmesh_ais_send_indication(uint8_t * p_data, uint16_t length);

bool ais_server_notify(uint8_t conn_id, uint8_t *pvalue, uint16_t len);

bool ais_server_indicate(uint8_t conn_id, uint8_t *pvalue, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif // BZ_AIS_H

