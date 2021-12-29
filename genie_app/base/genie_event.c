/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "genie_app.h"
//#include "mesh_hal_ble.h"
#include "mesh/cfg_srv.h"
#include "mesh.h"
#include "prov.h"
#include "foundation.h"


#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_EVENT)
#include "common/log.h"

#ifndef CONFIG_MESH_SEQ_COUNT_INT
#define CONFIG_MESH_SEQ_COUNT_INT 1000
#endif
extern void user_event(E_GENIE_EVENT event, void *p_arg);
extern S_ELEM_STATE g_elem_state[];

static bool g_genie_provisioned = 0;

const char *genie_event_str[] = {
    "SW_RESET",
    "HW_RESET_START",
    "HW_RESET_DONE",

    "SDK->REPEAT_RESET",

    "SDK->MESH_INIT",
    "SDK->PBADV_START",
    "SDK->PBADV_TIMEOUT",
    "SDK->SILENT_START",

    "SDK->PROV_START",
    "SDK->PROV_DATA",
    "SDK->PROV_TIMEOUT",
    "SDK->PROV_SUCCESS",
    "SDK->PROV_FAIL",

    "SDK->APPKEY_ADD",
    "SDK->APPKEY_DEL",
    "SDK->APPKEY_UPDATE",
    "SDK->NETKEY_ADD",
    "SDK->NETKEY_DEL",
    "SDK->NETKEY_UPDATE",
    "SDK->SUB_ADD",
    "SDK->SUB_DEL",
    "SDK->HB_SET",
    "SDK->SEQ_UPDATE",
    "SDK->STATE_SYNC",

    "SDK->ANALYZE_MSG",
    "SDK->AIS_DISCON",
#ifdef CONFIG_MESH_MODEL_TRANS
    "SDK->DELAY_START",
    "SDK->DELAY_END",
    "SDK->TRANS_START",
    "SDK->TRANS_CYCLE",
    "SDK->TRANS_END",
#endif
    "SDK->ACTION_DONE",
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    "SDK->PWRON_INDC",
    "SDK->INDICATE",
    "SDK->VENDOR_MSG",
#endif

    "APP->FAC_QUIT",
};


mesh_netkey_para_t genie_netkey[CONFIG_BT_MESH_SUBNET_COUNT];
mesh_appkey_para_t genie_appkey[CONFIG_BT_MESH_APP_KEY_COUNT];

#if BT_DBG_ENABLED
#define GENIE_MESH_EVENT_PRINT(id) BT_INFO("%s", genie_event_str[id])
#else
#define GENIE_MESH_EVENT_PRINT(id)
#endif
static void _genie_reset_prov(void)
{
    g_genie_provisioned = 0;
    /* reset prov */
    bt_mesh_reset();
#if 0
    bt_mesh_prov_reset_link();
#endif
    genie_flash_reset_system();
#ifdef GENIE_ULTRA_PROV
    ultra_prov_free();
#endif
}

static E_GENIE_EVENT _genie_event_handle_sw_reset(void)
{
    _genie_reset_prov();
    bt_mesh_adv_stop();
#if 0
    aos_reboot();
#endif
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    return GENIE_EVT_SDK_MESH_PBADV_START;
}

static E_GENIE_EVENT _genie_event_handle_hw_reset_start(void)
{
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    if(bt_mesh_is_provisioned()) {
        genie_indicate_hw_reset_event(); // Indicate hardware reset event to cloud
    }
#endif
    bt_mesh_adv_stop();
    genie_reset_done();
    return GENIE_EVT_HW_RESET_START;
}

static E_GENIE_EVENT _genie_event_handle_hw_reset_done(void)
{
    genie_reset_clean_count();
    _genie_reset_prov();
#if 0
    aos_reboot();
#endif
    /* restart adv */
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return GENIE_EVT_SDK_MESH_PBADV_START;
}

extern int ota_service_register(void);
static E_GENIE_EVENT _genie_event_handle_mesh_init(void)
{
    //check provsioning status
    uint16_t addr;
    uint32_t seq = 0;
    uint8_t devkey[16];
    mesh_netkey_para_t netkey[CONFIG_BT_MESH_SUBNET_COUNT];
    mesh_appkey_para_t appkey[CONFIG_BT_MESH_APP_KEY_COUNT];
    printf("++++%s ++++\r\n", __func__);

#ifdef CONFIG_GENIE_OTA
    ////ais_check_ota_change();
#endif

#if defined(CONFIG_GENIE_DEBUG_CMD)
    print_sw_info();
#endif

    // bit0:unicast_address
    // bit1:seq
    // bit2:devkey
    // bit3:netkey
    // bit4:appkey
    uint8_t read_flag = 0;
    if(genie_flash_read_addr(&addr) == GENIE_FLASH_SUCCESS) {
        printf("addr 0x%x\r\n", addr);
        read_flag |= 0x01;
    }

    if(genie_flash_read_seq(&seq) == GENIE_FLASH_SUCCESS) {
        printf("seq 0x%x\r\n", seq);
        read_flag |= 0x02;
    }

#ifdef CONIFG_OLD_FLASH_PARA
    if(genie_flash_read_para(&bt_mesh) == GENIE_FLASH_SUCCESS) {
        BT_DBG("read old");
        read_flag |= 0x1C;  //0001 1100
        // save data by new format.
        memcpy(devkey, bt_mesh.dev_key, 16);
        memset(&netkey, 0, sizeof(netkey));
        memcpy(netkey.key, bt_mesh.sub[0].keys[0].net, 16);
        memset(&appkey, 0, sizeof(appkey));
        memcpy(appkey.key, bt_mesh.app_keys[0].keys[0].val, 16);
        genie_flash_write_devkey(devkey);
        genie_flash_write_netkey(&netkey);
        genie_flash_write_appkey(&appkey);
        genie_flash_delete_reliable(GFI_MESH_PARA);
        memset(&bt_mesh, 0, sizeof(bt_mesh));
        bt_mesh.sub[0].net_idx = BT_MESH_KEY_UNUSED;
        bt_mesh.app_keys[0].net_idx = BT_MESH_KEY_UNUSED;
    } else
#endif
    {
        if(genie_flash_read_devkey(devkey) == GENIE_FLASH_SUCCESS) {
            printf("devkey %s\r\n", bt_hex(devkey, 16));
            read_flag |= 0x04;
        }
        if(genie_flash_read_netkey(&netkey, CONFIG_BT_MESH_SUBNET_COUNT) == GENIE_FLASH_SUCCESS) {
            printf("netkey %s\r\n", bt_hex(netkey[0].key, 16));
            read_flag |= 0x08;
        }
        if(genie_flash_read_appkey(&appkey, CONFIG_BT_MESH_APP_KEY_COUNT) == GENIE_FLASH_SUCCESS) {
            printf("appkey %s\r\n", bt_hex(appkey[0].key, 16));
            read_flag |= 0x10;
        }
    }

    printf("flag %02x\n", read_flag);
#if 1
	if((read_flag & 0x1F) == 0x1F) {
#else
    if((read_flag & 0x1F) == 0x1D) {            ////(0x1F)
#endif
        BT_INFO(">>>proved<<<");
#if CONFIG_MESH_SEQ_COUNT_INT
        seq += CONFIG_MESH_SEQ_COUNT_INT;
#endif
#if CONFIG_BT_MESH_SHELL
        int cmd_netkey_info_sync(mesh_netkey_para_t *subnet, uint16_t count);
        int cmd_appkey_info_sync(mesh_appkey_para_t *appkeys, uint16_t count);
        cmd_netkey_info_sync(netkey, CONFIG_BT_MESH_SUBNET_COUNT);
        cmd_appkey_info_sync(appkey, CONFIG_BT_MESH_APP_KEY_COUNT);
#endif // CONFIG_BT_MESH_SHELL
#if (defined CONFIG_BT_MESH_TELINK) || (defined CONFIG_BT_MESH_JINGXUN)
        bt_mesh_proved_reset_flag_set(1);
#endif /* CONFIG_BT_MESH_TELINK||CONFIG_BT_MESH_JINGXUN */
        bool net_created = false;
        for (int i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
            if (netkey[i].net_index != BT_MESH_KEY_UNUSED) {
                if (!net_created) {
                    net_created = true;
                    bt_mesh_provision(&netkey[i].key[0], netkey[i].net_index, netkey[i].flag, 0, seq, addr, devkey);
                } else {
                    bt_mesh_net_key_add(netkey[i].net_index, &netkey[i].key[0]);
                }
            }
        }
        extern void genie_appkey_register(u16_t net_idx, u16_t app_idx, const u8_t val[16], bool update);

        for (int j = 0; j < CONFIG_BT_MESH_APP_KEY_COUNT; j++) {
            if (appkey[j].key_index != BT_MESH_KEY_UNUSED) {
                genie_appkey_register(appkey[j].net_index, appkey[j].key_index, &appkey[j].key[0], 0);
            }
        }

#ifdef CONFIG_GENIE_OTA
        ais_service_register();
#endif
#ifdef CONFIG_BEKEN_OTA
        ota_service_register();
#endif
        /* check hb */
        mesh_hb_para_t hb_para = {.count = 0};
        genie_flash_read_hb(&hb_para);
        if(hb_para.count == 0xFF) {
            extern u8_t genie_heartbeat_set(mesh_hb_para_t *p_para);
            genie_heartbeat_set(&hb_para);
        }

        if (!genie_reset_get_flag()) {
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_POWERON;
            genie_indicate_start(0, &g_elem_state[0]);
#endif
        } else {
            return GENIE_EVT_HW_RESET_START;
        }
    } else if(read_flag) {
        BT_INFO(">>>error<<<");
        genie_flash_reset_system();
        aos_reboot();
    } else {
        BT_INFO(">>>unprovisioned<<<");
        if (genie_reset_get_flag()) {
            return GENIE_EVT_HW_RESET_START;
        }
#ifdef CONFIG_GENIE_OTA
        ais_service_register();
#endif
#ifdef CONFIG_BEKEN_OTA
        ota_service_register();
#endif
        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

        return GENIE_EVT_SDK_MESH_PBADV_START;
    }
    return GENIE_EVT_SDK_MESH_INIT;
}

static E_GENIE_EVENT _genie_event_handle_pbadv_start(void)
{
    genie_pbadv_timer_start();
    return GENIE_EVT_SDK_MESH_PBADV_START;
}

static E_GENIE_EVENT _genie_event_handle_pbadv_timeout(void)
{
    genie_pbadv_timer_stop();
    bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return GENIE_EVT_SDK_MESH_SILENT_START;
}

static E_GENIE_EVENT _genie_event_handle_silent_start(void)
{
    genie_pbadv_start_silent_adv();
    return GENIE_EVT_SDK_MESH_SILENT_START;
}

static E_GENIE_EVENT _genie_event_handle_prov_start(void)
{
    if(!bt_mesh_is_provisioned()) {
        /* disable adv timer */
        genie_pbadv_timer_stop();
        /* enable prov timer */
        genie_prov_timer_start();
    }
    return GENIE_EVT_SDK_MESH_PROV_START;
}

static E_GENIE_EVENT _genie_event_handle_prov_data(uint16_t *p_addr)
{
    genie_flash_write_addr(p_addr);
    return GENIE_EVT_SDK_MESH_PROV_DATA;
}

static E_GENIE_EVENT _genie_event_handle_prov_timeout(void)
{
    return GENIE_EVT_SDK_MESH_PROV_FAIL;
}

static E_GENIE_EVENT _genie_event_handle_prov_success(void)
{
    g_genie_provisioned = 1;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    return GENIE_EVT_SDK_STATE_SYNC;
#else
    return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
#endif
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static E_GENIE_EVENT _genie_event_handle_sync(S_ELEM_STATE *p_elem)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    g_indication_flag |= INDICATION_FLAG_CTL;
#endif
    genie_indicate_start(1000, p_elem);

    return GENIE_EVT_SDK_STATE_SYNC;
}
#endif

static E_GENIE_EVENT _genie_event_handle_prov_fail(void)
{
    /* reset prov */
    _genie_reset_prov();
    /* restart adv */
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return GENIE_EVT_SDK_MESH_PBADV_START;
}

static void _genie_event_save_mesh_data(uint8_t *p_status)
{
    if(*p_status == 0) {
        genie_flash_write_para(&bt_mesh);
    }
}

static E_GENIE_EVENT _genie_event_handle_appkey_add(uint8_t *p_status)
{
    BT_ERR("g_in_prov %d", bt_mesh_is_provisioned());
    if(bt_mesh_is_provisioned()) {
        /* disable prov timer */
        genie_prov_timer_stop();
        if(*p_status == STATUS_SUCCESS) {
            //genie_flash_write_para(&bt_mesh);
            uint8_t devkey[16];
            printf("%s, CONFIG_BT_MESH_SUBNET_COUNT %d, CONFIG_BT_MESH_APP_KEY_COUNT %d\n",
                __func__, CONFIG_BT_MESH_SUBNET_COUNT, CONFIG_BT_MESH_APP_KEY_COUNT);
            memcpy(devkey, bt_mesh.dev_key, 16);
            genie_flash_write_devkey(devkey);

            for (int i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
                if (bt_mesh.sub[i].net_idx != BT_MESH_KEY_UNUSED) {
                    genie_netkey[i].net_index = bt_mesh.sub[i].net_idx;
                    memcpy(genie_netkey[i].key, bt_mesh.sub[i].keys[0].net, 16);
                }
            }

            for (int j = 0; j < CONFIG_BT_MESH_APP_KEY_COUNT; j++) {
                if (bt_mesh.app_keys[j].app_idx != BT_MESH_KEY_UNUSED) {
                    genie_appkey[j].net_index = bt_mesh.app_keys[j].net_idx;
                    genie_appkey[j].key_index = bt_mesh.app_keys[j].app_idx;
                    memcpy(genie_appkey[j].key, bt_mesh.app_keys[j].keys[0].val, 16);
                }
            }
            genie_flash_write_netkey(genie_netkey, CONFIG_BT_MESH_SUBNET_COUNT);
            genie_flash_write_appkey(genie_appkey, CONFIG_BT_MESH_APP_KEY_COUNT);
            return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
        }
		else if (STATUS_INSUFF_RESOURCES == *p_status) {
			return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
		}
		else {
            return GENIE_EVT_SDK_MESH_PROV_FAIL;
        }
    } else {
        _genie_event_save_mesh_data(p_status);
        return GENIE_EVT_SDK_APPKEY_ADD;
    }
}

static E_GENIE_EVENT _genie_event_handle_sub_add(void)
{
    genie_flash_write_sub(g_sub_list);
    return GENIE_EVT_SDK_SUB_ADD;
}

static E_GENIE_EVENT _genie_event_handle_hb_set(mesh_hb_para_t *p_para)
{
    BT_DBG("save");
    genie_flash_write_hb(p_para);
    return GENIE_EVT_SDK_HB_SET;
}

static E_GENIE_EVENT _genie_event_handle_seq_update(void)
{
    uint32_t seq = bt_mesh.seq;
#if CONFIG_MESH_SEQ_COUNT_INT
    if ((seq % CONFIG_MESH_SEQ_COUNT_INT == 0) || (seq == 1)) {
        genie_flash_write_seq(&seq);
    }
#else
    genie_flash_write_seq(&seq);
#endif
    return GENIE_EVT_SDK_SEQ_UPDATE;
}

static E_GENIE_EVENT _genie_event_handle_analyze_msg(S_ELEM_STATE *p_elem)
{
#ifdef CONFIG_MESH_MODEL_TRANS
    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            return GENIE_EVT_SDK_DELAY_START;
        } else {
            return GENIE_EVT_SDK_TRANS_START;
        }
    }
#endif
    return GENIE_EVT_SDK_ACTION_DONE;
}

static E_GENIE_EVENT _genie_event_handle_ais_discon(S_ELEM_STATE *p_elem)
{
    /* restart adv */
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return GENIE_EVT_SDK_MESH_PBADV_START;
}

#ifdef CONFIG_MESH_MODEL_TRANS
static E_GENIE_EVENT _genie_event_handle_delay_start(S_ELEM_STATE *p_elem)
{
    mesh_timer_stop(p_elem);
    k_timer_start(&p_elem->state.delay_timer, p_elem->state.delay * 5);
    return GENIE_EVT_SDK_DELAY_START;
}

static E_GENIE_EVENT _genie_event_handle_delay_end(S_ELEM_STATE *p_elem)
{
    u32_t cur_time = k_uptime_get();

    p_elem->state.delay = 0;

    if(p_elem->state.trans == 0 || cur_time >= p_elem->state.trans_end_time) {
        clear_trans_para(p_elem);
        return GENIE_EVT_SDK_ACTION_DONE;
    } else {
        return GENIE_EVT_SDK_TRANS_START;
    }
}

static E_GENIE_EVENT _genie_event_handle_trans_start(S_ELEM_STATE *p_elem)
{
    u32_t cur_time = k_uptime_get();

    mesh_timer_stop(p_elem);

    //check time
    if(cur_time >= p_elem->state.trans_end_time - MESH_TRNSATION_CYCLE) {
        return GENIE_EVT_SDK_TRANS_END;
    } else {
        //start cycle
        k_timer_start(&p_elem->state.trans_timer, MESH_TRNSATION_CYCLE);
        BT_DBG("start trans %p", &p_elem->state.trans_timer);
        return GENIE_EVT_SDK_TRANS_START;
    }
}

static E_GENIE_EVENT _genie_event_handle_trans_cycle(S_ELEM_STATE *p_elem)
{
    if(calc_cur_state(p_elem) == 0) {
        p_elem->state.trans = 0;
    }
    return GENIE_EVT_SDK_TRANS_CYCLE;
}

static E_GENIE_EVENT _genie_event_handle_trans_end(S_ELEM_STATE *p_elem)
{
    //clear paras
    clear_trans_para(p_elem);
    //action done
    return GENIE_EVT_SDK_ACTION_DONE;
}
#endif

static E_GENIE_EVENT _genie_event_handle_action_done(S_ELEM_STATE *p_elem)
{
    model_message_index_e dest_index = p_elem->message_index;

    BT_DBG("main index (%d)", dest_index);

    //Note that the bound states may change along with the main states

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if(p_elem->state.onoff[T_CUR] != p_elem->state.onoff[T_TAR])
    {
        BT_DBG("onoff cur(%d) tar(%d)", p_elem->state.onoff[T_CUR], p_elem->state.onoff[T_TAR]);
        p_elem->state.onoff[T_CUR] = p_elem->state.onoff[T_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if(p_elem->state.light_ln_actual[T_CUR] != p_elem->state.light_ln_actual[T_TAR])
    {
        BT_DBG("actual cur(0x%x) tar(0x%x)", p_elem->state.light_ln_actual[T_CUR], p_elem->state.light_ln_actual[T_TAR]);
        p_elem->state.light_ln_actual[T_CUR] = p_elem->state.light_ln_actual[T_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if(p_elem->state.ctl_lightness[T_CUR] != p_elem->state.ctl_lightness[T_TAR])
    {
        BT_DBG("lightness cur(0x%x) tar(0x%x)", p_elem->state.ctl_lightness[T_CUR], p_elem->state.ctl_lightness[T_TAR]);
        p_elem->state.ctl_lightness[T_CUR] = p_elem->state.ctl_lightness[T_TAR];
    }

    if(p_elem->state.ctl_temp[T_CUR] != p_elem->state.ctl_temp[T_TAR])
    {
        BT_DBG("temp cur(0x%x) tar(0x%x)", p_elem->state.ctl_temp[T_CUR], p_elem->state.ctl_temp[T_TAR]);
        p_elem->state.ctl_temp[T_CUR] = p_elem->state.ctl_temp[T_TAR];
    }

    if(p_elem->state.ctl_UV[T_CUR] != p_elem->state.ctl_UV[T_TAR])
    {
        BT_DBG("UV cur(0x%x) tar(0x%x)", p_elem->state.ctl_UV[T_CUR], p_elem->state.ctl_UV[T_TAR]);
        p_elem->state.ctl_UV[T_CUR] = p_elem->state.ctl_UV[T_TAR];
    }

#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV

    if(p_elem->state.hsl_lightness[T_CUR] != p_elem->state.hsl_lightness[T_TAR]) {
        BT_DBG("hsl_lightness cur(0x%x) tar(0x%x)", p_elem->state.hsl_lightness[T_CUR], p_elem->state.hsl_lightness[T_TAR]);
        p_elem->state.hsl_lightness[T_CUR] = p_elem->state.hsl_lightness[T_TAR];
    }

    if(p_elem->state.hsl_hue[T_CUR] != p_elem->state.hsl_hue[T_TAR]) {
        BT_DBG("hsl_hue cur(0x%x) tar(0x%x)", p_elem->state.hsl_hue[T_CUR], p_elem->state.hsl_hue[T_TAR]);
        p_elem->state.hsl_hue[T_CUR] = p_elem->state.hsl_hue[T_TAR];
    }

    if(p_elem->state.hsl_sat[T_CUR] != p_elem->state.hsl_sat[T_TAR]) {
        BT_DBG("hsl_sat cur(0x%x) tar(0x%x)", p_elem->state.hsl_sat[T_CUR], p_elem->state.hsl_sat[T_TAR]);
        p_elem->state.hsl_sat[T_CUR] = p_elem->state.hsl_sat[T_TAR];
    }

#endif

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    if(bt_mesh_is_provisioned()) {
        return GENIE_EVT_SDK_INDICATE;
    }
#endif
    return GENIE_EVT_SDK_ACTION_DONE;
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static E_GENIE_EVENT _genie_event_handle_pwron_indc(S_ELEM_STATE *p_elem)
{
    //g_indication_flag |= INDICATION_FLAG_POWERON;
    return GENIE_EVT_SDK_INDICATE;
}

static E_GENIE_EVENT _genie_event_handle_indicate(S_ELEM_STATE *p_elem)
{
    if(g_indication_flag) {
        standart_indication(p_elem);
    }
    return GENIE_EVT_SDK_INDICATE;
}

static E_GENIE_EVENT _genie_event_handle_vnd_msg(vnd_model_msg *p_msg)
{
    BT_DBG("vendor message received");
    genie_vnd_msg_handle(p_msg);
    return GENIE_EVT_SDK_VENDOR_MSG;
}
#ifdef MESH_MODEL_VENDOR_TIMER
static E_GENIE_EVENT _genie_event_handle_order_msg(vendor_attr_data_t *attr_data)
{
    BT_DBG("type:%04x data:%04x\r\n",attr_data->type,attr_data->para);

    if (attr_data->type == ONOFF_T) {
        g_elem_state[0].state.onoff[T_TAR] = attr_data->para;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
    }

    return GENIE_EVT_SDK_ACTION_DONE;
}

#endif
#endif

bool genie_is_provisioned(void)
{
    return g_genie_provisioned;
}

void genie_event(E_GENIE_EVENT event, void *p_arg)
{
    E_GENIE_EVENT next_event = event;
    uint8_t ignore_user_event = 0;

#ifdef CONFIG_MESH_MODEL_TRANS
    if(event != GENIE_EVT_SDK_TRANS_CYCLE) {
        GENIE_MESH_EVENT_PRINT(event);
    }
#endif
    switch(event) {
        case GENIE_EVT_SW_RESET:
            //call user_event first
            user_event(GENIE_EVT_SW_RESET, p_arg);
            ignore_user_event = 1;
            next_event = _genie_event_handle_sw_reset();
            break;

        case GENIE_EVT_HW_RESET_START:
            _genie_event_handle_hw_reset_start();
            break;

        case GENIE_EVT_HW_RESET_DONE:
            //call user_event first
            user_event(GENIE_EVT_HW_RESET_DONE, p_arg);
            ignore_user_event = 1;
            next_event = _genie_event_handle_hw_reset_done();
            break;

#ifdef CONFIG_GENIE_RESET_BY_REPEAT
        case GENIE_EVT_REPEAT_RESET:
            next_event = GENIE_EVT_REPEAT_RESET;
            break;
#endif

        case GENIE_EVT_SDK_MESH_INIT:
            //update p_arg to user_event
            p_arg = (void *)&g_elem_state[0];
            next_event = _genie_event_handle_mesh_init();
            break;

        case GENIE_EVT_SDK_MESH_PBADV_START:
            next_event = _genie_event_handle_pbadv_start();
            break;

        case GENIE_EVT_SDK_MESH_PBADV_TIMEOUT:
            next_event = _genie_event_handle_pbadv_timeout();
            break;

        case GENIE_EVT_SDK_MESH_SILENT_START:
            next_event = _genie_event_handle_silent_start();
            break;

        case GENIE_EVT_SDK_MESH_PROV_START:
            next_event = _genie_event_handle_prov_start();
            break;

        case GENIE_EVT_SDK_MESH_PROV_DATA:
            next_event = _genie_event_handle_prov_data((uint16_t *)p_arg);
            break;

        case GENIE_EVT_SDK_MESH_PROV_TIMEOUT:
            next_event = _genie_event_handle_prov_timeout();
            break;

        case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
            next_event = _genie_event_handle_prov_success();
            break;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        case GENIE_EVT_SDK_STATE_SYNC:
            next_event = _genie_event_handle_sync(p_arg);
            break;
#endif
        case GENIE_EVT_SDK_MESH_PROV_FAIL:
            next_event = _genie_event_handle_prov_fail();
            break;

        case GENIE_EVT_SDK_APPKEY_ADD:
        {
            next_event = _genie_event_handle_appkey_add((uint8_t *)p_arg);
            if(next_event == GENIE_EVT_SDK_MESH_PROV_SUCCESS) {
                p_arg = &g_elem_state[0];
            }
            break;
        }

        case GENIE_EVT_SDK_APPKEY_DEL:
        case GENIE_EVT_SDK_APPKEY_UPDATE:
        case GENIE_EVT_SDK_NETKEY_ADD:
        case GENIE_EVT_SDK_NETKEY_DEL:
        case GENIE_EVT_SDK_NETKEY_UPDATE:
        //case GENIE_EVT_SDK_IVI_UPDATE:
            //_genie_event_save_mesh_data((uint8_t *)p_arg);
            break;

        case GENIE_EVT_SDK_SUB_ADD: {
            int i;
            uint16_t sub_addr = sys_get_le16(p_arg);
            for (i = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT; i++) {
                if (g_sub_list[i] == sub_addr) {
                    break;
                }

                if (g_sub_list[i] == BT_MESH_ADDR_UNASSIGNED) {
                    g_sub_list[i] = sub_addr;
                    next_event = _genie_event_handle_sub_add();
                    break;
                }
            }
        }
            break;
        case GENIE_EVT_SDK_SUB_DEL: {
            int i;
            uint16_t sub_addr = sys_get_le16(p_arg);
            for (i = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT; i++) {
                if (g_sub_list[i] == sub_addr) {
                    g_sub_list[i] == BT_MESH_ADDR_UNASSIGNED;
                    next_event = _genie_event_handle_sub_add();
                    break;
                }
            }
        }
            break;

        case GENIE_EVT_SDK_HB_SET:
            next_event = _genie_event_handle_hb_set((mesh_hb_para_t *)p_arg);
            break;

        case GENIE_EVT_SDK_SEQ_UPDATE:
            next_event = _genie_event_handle_seq_update();
            break;

        case GENIE_EVT_SDK_ANALYZE_MSG:
            next_event = _genie_event_handle_analyze_msg((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_AIS_DISCON:
            next_event = _genie_event_handle_ais_discon((S_ELEM_STATE *)p_arg);
            break;

#ifdef CONFIG_MESH_MODEL_TRANS
        case GENIE_EVT_SDK_DELAY_START:
            next_event = _genie_event_handle_delay_start((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_DELAY_END:
            next_event = _genie_event_handle_delay_end((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_TRANS_START:
            next_event = _genie_event_handle_trans_start((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_TRANS_CYCLE:
            next_event = _genie_event_handle_trans_cycle((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_TRANS_END:
            next_event = _genie_event_handle_trans_end((S_ELEM_STATE *)p_arg);
            break;
#endif

        case GENIE_EVT_SDK_ACTION_DONE:
            next_event = _genie_event_handle_action_done((S_ELEM_STATE *)p_arg);
            break;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        case GENIE_EVT_SDK_MESH_PWRON_INDC:
            next_event = _genie_event_handle_pwron_indc((S_ELEM_STATE *)p_arg);
            //p_arg = &g_elem_state[0];
            break;

        case GENIE_EVT_SDK_INDICATE:
            next_event = _genie_event_handle_indicate((S_ELEM_STATE *)p_arg);
            break;

        case GENIE_EVT_SDK_VENDOR_MSG:
            next_event = _genie_event_handle_vnd_msg((vnd_model_msg *)p_arg);
            break;
#endif
#ifdef MESH_MODEL_VENDOR_TIMER
        case GENIE_EVT_TIME_OUT:
        {
            next_event = _genie_event_handle_order_msg((vendor_attr_data_t *)p_arg);
            p_arg = &g_elem_state[0];
            break;
        }
#endif
        default:
            break;
    }

    if(!ignore_user_event) {
        user_event(event, p_arg);
    }

    if(next_event != event) {
        genie_event(next_event, p_arg);
    }
}

