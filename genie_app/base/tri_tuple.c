/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

#include "genie_app.h"
#include "tri_tuple_default.h"

#define RETURN_WHEN_ERR(result, err_code) { if(result != 0) { BT_ERR("%s[%d] error ret(%d)", __func__, __LINE__, result); return err_code; } }

#define GENIE_SIZE_PID 4
#define GENIE_SIZE_KEY 16
#define GENIE_SIZE_MAC 6
#define GENIE_SIZE_TRI_TRUPLE (GENIE_SIZE_PID+GENIE_SIZE_KEY+GENIE_SIZE_MAC)

static uint32_t g_pid;
uint8_t g_mac[GENIE_SIZE_MAC];
static uint8_t g_key[GENIE_SIZE_KEY];
static uint8_t g_auth[32];
static uint8_t g_uuid[16];

E_GENIE_FLASH_ERRCODE genie_flash_write_trituple(uint32_t pid, uint8_t *p_mac,  uint8_t *p_key)
{
    //4+6+32
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];

    BT_DBG("");

    memcpy(data, &pid, GENIE_SIZE_PID);
    memcpy(data+GENIE_SIZE_PID, p_key, GENIE_SIZE_KEY);
    memcpy(data+GENIE_SIZE_PID+GENIE_SIZE_KEY, p_mac, GENIE_SIZE_MAC);

    return genie_flash_write_reliable(GFI_MESH_TRITUPLE, data, GENIE_SIZE_TRI_TRUPLE);
}

E_GENIE_FLASH_ERRCODE genie_flash_read_trituple(uint32_t *p_pid, uint8_t *p_mac,  uint8_t *p_key)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint8_t data[GENIE_SIZE_TRI_TRUPLE];
    uint16_t size = GENIE_SIZE_TRI_TRUPLE;

    BT_DBG("");

    ret = genie_flash_read_reliable(GFI_MESH_TRITUPLE, data, &size);
    RETURN_WHEN_ERR(ret, ret);

    if(size == GENIE_SIZE_TRI_TRUPLE) {
        memcpy(p_pid, data, GENIE_SIZE_PID);
        memcpy(p_key, data+GENIE_SIZE_PID, GENIE_SIZE_KEY);
        memcpy(p_mac, data+GENIE_SIZE_PID+GENIE_SIZE_KEY, GENIE_SIZE_MAC);
        return GENIE_FLASH_SUCCESS;
    } else {
        BT_ERR("read size error");
        return GENIE_FLASH_READ_FAIL;
    }
}

void genie_tri_tuple_set_silent_adv(void)
{
    g_uuid[13] |= UNPROV_ADV_FEATURE_SILENT_ADV;
    return;
}

uint8_t *genie_tri_tuple_get_uuid(void)
{
    int i;
    
    memcpy(g_mac,DEFAULT_MAC,6);
#if 0  
    // all fields in uuid should be in little-endian
    // CID: Taobao
    g_uuid[0] = 0xa8;
    g_uuid[1] = 0x01;

    // PID
    // Bit0~Bit3: 0001 (broadcast version)
    // Bit4：1 (one secret pre device)
    // Bit5: 1 (OTA support)
    // Bit6~Bit7: 01 (ble 4.0)
    g_uuid[2] = 0x71;

    // Product ID
    for (i = 0; i < 4; i++) {
        g_uuid[3 + i] = (g_pid >> (i<<3)) & 0xFF;
    }

    // mac addr (device name)
    for (i = 0; i < 6; i++) {
        g_uuid[7 + i] = g_mac[5 - i];
    }

    g_uuid[13] = 0;
    g_uuid[13] |= UNPROV_ADV_FEATURE_AUTO_BIND_MODEL_SUB;

    BT_DBG("uuid: %s", bt_hex(g_uuid, 16));
    return g_uuid;
#endif
   for (i = 0; i < 6; i++) {
        g_uuid[i] = g_mac[i];
    }
    
    g_uuid[6]=0x02; g_uuid[7]=0x15;
    //pid//dfvoarca
    g_uuid[8] = 0x64;// PID
    g_uuid[9] = 0x66; g_uuid[10] = 0x76; g_uuid[11] = 0x6f;g_uuid[12] = 0x61;
    g_uuid[13] = 0x72;g_uuid[14] = 0x63; g_uuid[15] = 0x61; //RFU
    BT_DBG("uuid: %s", bt_hex(g_uuid, 16));
    return g_uuid;
}

uint8_t *genie_tri_tuple_get_auth(void)
{
    int ret;
    char mac_str[GENIE_SIZE_MAC<<1+1] = "";
    char key_str[GENIE_SIZE_KEY<<1+1] = "";
    char static_str[55] = ""; // pid + ',' + mac + ',' + secret = 8+1+12+1+32+'\0'
    struct tc_sha256_state_struct sha256_ctx;
    
    hextostring(g_mac, mac_str, GENIE_SIZE_MAC);
    hextostring(g_key, key_str, GENIE_SIZE_KEY);

    sprintf(static_str, "%08x,%s,%s", g_pid, mac_str, key_str);

    BT_DBG("static oob: %s", static_str);

    /* calculate the sha256 of oob info and
     * fetch the top 16 bytes as static value
     */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, static_str, strlen(static_str));
    if (ret != TC_CRYPTO_SUCCESS) {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(g_auth, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS) {
        BT_ERR("sha256 final fail\n");
    }
#if 1
    else {
        BT_DBG("auth:");
        dump_print(g_auth, 16);
    }
#endif
    return g_auth;
}

int8_t genie_tri_tuple_load(void)
{
    char l_mac[] = DEFAULT_MAC;
    char l_key[] = DEFAULT_SECRET;
    E_GENIE_FLASH_ERRCODE ret;

    ret = genie_flash_read_trituple(&g_pid, g_mac, g_key);

    if(ret != GENIE_FLASH_SUCCESS) {
        BT_ERR("read error, use default");
        g_pid = DEFAULT_PID;
        stringtohex(l_key, g_key, 16);
        stringtohex(l_mac, g_mac, 6);
        return -1;
    }

    return 0;
}

void genie_tri_tuple_show(void)
{
    uint8_t i;

    printk(F_BLUE "%d ", g_pid);

    for(i = 0; i < 16; i++) {
        printk("%02x", g_key[i]);
    }

    printk(" ");
    for(i = 0; i < 6; i++) {
        printk("%02x", g_mac[i]);
    }
    printk("\n" F_END);

    return 0;
}




