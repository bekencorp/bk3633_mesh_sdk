/* main.c - light demo */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>
#include "inc/time_scene_server.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif

#include "light_board.h"
#include "uart_test_cmd.h"

#ifdef CONFIG_BT_MESH_JINGXUN
u8  app_time_flag;
struct k_delayed_work app_timer;
#endif CONFIG_BT_MESH_JINGXUN

led_flash_t g_flash_para;

uint32_t get_mesh_pbadv_time(void)
{
    return MESH_PBADV_TIME*1000;    //ms
}

S_ELEM_STATE g_elem_state[MESH_ELEM_STATE_COUNT];
S_MODEL_POWERUP g_powerup[MESH_ELEM_STATE_COUNT];

#if CONFIG_BLE_MESH_TIME_SCENE_SERVER
struct bt_mesh_time_state time_srv_state_0;
struct bt_mesh_time_srv time_srv_0 = {
    .state = &time_srv_state_0,
};

struct bt_mesh_time_setup_srv time_setup_srv_0 = {
    .state = &time_srv_state_0,
};

struct scene_register scene_reg[5] = {
    [0] = {.scene_number = 0x0000,
           .scene_value = NET_BUF_SIMPLE(2 + 30),
          },
    [1] = {.scene_number = 0x0000,
           .scene_value = NET_BUF_SIMPLE(2 + 30),
          },
    [2] = {.scene_number = 0x0000,
           .scene_value = NET_BUF_SIMPLE(2 + 30),
          },
    [3] = {.scene_number = 0x0000,
           .scene_value = NET_BUF_SIMPLE(2 + 30),
          },
    [4] = {.scene_number = 0x0000,
           .scene_value = NET_BUF_SIMPLE(2 + 30),
          },
};

struct bt_mesh_scenes_state scenes_srv_state_0 = {
    .scene_count = 16,
    .scenes = &scene_reg,
};

struct bt_mesh_scene_srv scene_srv_0 = {
    .state = &scenes_srv_state_0,
};

struct bt_mesh_scene_setup_srv scen_setup_srv_0 = {
    .state = &scenes_srv_state_0,
};

struct schedule_register sch_reg[5];
struct bt_mesh_scheduler_state scheduler_srv_state_0 = {
    .schedule_count = 5,
    .schedules = sch_reg,
};
struct bt_mesh_scheduler_srv schduler_srv_0 = {
    .state = &scheduler_srv_state_0,
};

struct bt_mesh_scheduler_setup_srv schduler_setup_srv_0 = {
    .state = &scheduler_srv_state_0,
};
#endif

struct k_delayed_work light_state_store_work;

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(),
    BT_MESH_MODEL_HEALTH_SRV(),

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    MESH_MODEL_GEN_ONOFF_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV
    MESH_MODEL_GEN_DEF_TRANS_TIME_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
    MESH_MODEL_GEN_ONPOWERUP_SRV(&g_elem_state[0]),
    MESH_MODEL_GEN_ONPOWERUP_SETUP_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    MESH_MODEL_GEN_LEVEL_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    MESH_MODEL_LIGHTNESS_SRV(&g_elem_state[0]),
#endif
    MESH_MODEL_LIGHTNESS_SETUP_SRV(&g_elem_state[0]),

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    MESH_MODEL_CTL_SRV(&g_elem_state[0]),
#endif

    MESH_MODEL_CTL_SETUP_SRV(&g_elem_state[0]),
    MESH_MODEL_CTL_TEMP_SRV(&g_elem_state[0]),
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    MESH_MODEL_HSL_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_SETUP_SRV(&g_elem_state[0]),
#endif
	MESH_MODEL_HSL_HUE_SRV(&g_elem_state[0]),
	MESH_MODEL_HSL_SAT_SRV(&g_elem_state[0]),

#if CONFIG_BLE_MESH_TIME_SCENE_SERVER
    MESH_MODEL_TIME_SRV(&time_srv_0),
    MESH_MODEL_TIME_SETUP_SRV(&time_setup_srv_0),
    MESH_MODEL_SCENE_SRV(&scene_srv_0),
    MESH_MODEL_SCENE_SETUP_SRV(&scen_setup_srv_0),
    MESH_MODEL_SCHEDULER_SRV(&schduler_srv_0),
    MESH_MODEL_SCHEDULER_SETUP_SRV(&schduler_setup_srv_0),
#endif
};

static struct bt_mesh_model vnd_models[] = {

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    MESH_MODEL_VENDOR_SRV(&g_elem_state[0]),
#endif
};

struct bt_mesh_model s0_models[] = {
    MESH_MODEL_GEN_LEVEL_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_HUE_SRV(&g_elem_state[0]),
    MESH_MODEL_CTL_TEMP_SRV(&g_elem_state[0]),
};

struct bt_mesh_model s1_models[] = {
    MESH_MODEL_GEN_LEVEL_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_SAT_SRV(&g_elem_state[0]),
};

struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models, 0),
    //BT_MESH_ELEM(0, s0_models, BT_MESH_MODEL_NONE, 0),
    //BT_MESH_ELEM(0, s1_models, BT_MESH_MODEL_NONE, 0),
};

#if (CONFIG_NET_BUF_DBG_LOG)
static bool show_count = false;
#endif //CONFIG_NET_BUF_DBG_LOG
static void light_state_store(struct k_work *work)
{
    uint8_t *p_read = aos_zalloc(sizeof(g_powerup));

    LIGHT_DBG("");
    if (!p_read) {
        LIGHT_DBG("%s, no memory.\n", __func__);
#if (CONFIG_NET_BUF_DBG_LOG)
        if (!show_count) {
            show_count = true;
            net_buf_dbg_show();
        }
#endif //CONFIG_NET_BUF_DBG_LOG
        return;
    }

    genie_flash_read_userdata(GFI_MESH_POWERUP, p_read, sizeof(g_powerup));

    if(memcmp(g_powerup, p_read, sizeof(g_powerup))) {
        LIGHT_DBG("save %d", g_powerup[0].last_onoff);
        genie_flash_write_userdata(GFI_MESH_POWERUP, (uint8_t *)g_powerup, sizeof(g_powerup));
    }

    aos_free(p_read);

#if CONFIG_BEKEN_OTA
    if (g_powerup[0].last_onoff == 0 && ota_update_complete_get() == 1) {
        //Means have ota, wait for reboot while light off
        aos_reboot();
    }
#endif
}

uint8_t get_vendor_element_num(void)
{
    return ARRAY_SIZE(elements);
}

void mesh_sub_init(u16_t *p_sub)
{
    memset(p_sub, 0, CONFIG_BT_MESH_MODEL_GROUP_COUNT<<1);

    p_sub[0] = DEFAULT_MESH_GROUP1;
    p_sub[1] = DEFAULT_MESH_GROUP2;
}

void _init_light_para(void)
{
    uint8_t i = 0;
    E_GENIE_FLASH_ERRCODE ret;

    //read flash
    memset(g_elem_state, 0, sizeof(g_elem_state));
    genie_elem_state_init(MESH_ELEM_STATE_COUNT, g_elem_state);

    // load light para
    ret = genie_flash_read_userdata(GFI_MESH_POWERUP, (uint8_t *)g_powerup, sizeof(g_powerup));
    if(ret == GENIE_FLASH_SUCCESS)
    {
        genie_restore_user_state(MESH_ELEM_STATE_COUNT, g_elem_state, g_powerup);
    }
    LIGHT_DBG("init parameter ret %d", ret);
    led_ctl_set_handler(g_elem_state[0].powerup.light_ln_last, g_elem_state[0].powerup.ctl_temp_last, 0);

    //LIGHT_DBG("done");
}

static void _reset_light_para(void)
{
    uint8_t i = 0;

    while(i < MESH_ELEM_STATE_COUNT) {
        g_elem_state[i].state.onoff[T_CUR] = GEN_ONOFF_DEFAULT;
        g_elem_state[i].state.light_ln_actual[T_CUR] = LIGHTNESS_DEFAULT;
        g_elem_state[i].state.ctl_temp[T_CUR] = CTL_TEMP_DEFAULT;
        g_elem_state[i].state.onoff[T_TAR] = GEN_ONOFF_DEFAULT;
        g_elem_state[i].state.light_ln_actual[T_TAR] = LIGHTNESS_DEFAULT;
        g_elem_state[i].state.ctl_temp[T_TAR] = CTL_TEMP_DEFAULT;
        g_elem_state[i].state.trans = 0;
        g_elem_state[i].state.delay = 0;
        g_elem_state[i].state.trans_start_time = 0;
        g_elem_state[i].state.trans_end_time = 0;

        g_elem_state[i].powerup.light_ln_last = LIGHTNESS_DEFAULT;
        g_elem_state[i].powerup.ctl_temp_last = CTL_TEMP_DEFAULT;

        g_powerup[i].last_onoff = GEN_ONOFF_DEFAULT;
        g_powerup[i].light_ln_last = LIGHTNESS_DEFAULT;
        g_powerup[i].ctl_temp_last = CTL_TEMP_DEFAULT;

        i++;
    }
    genie_flash_write_userdata(GFI_MESH_POWERUP, (uint8_t *)g_powerup, sizeof(g_powerup));
    LIGHT_DBG("done");
}


static led_func_ret_e _led_gen_onoff_srv(S_ELEM_STATE *p_elem)
{
    uint8_t last_onoff = p_elem->powerup.last_onoff;
    uint8_t onoff = p_elem->state.onoff[T_CUR];
    uint16_t light_para[LED_CHANNEL_MAX];

    light_type_e light_type = get_light_board_type();

    if(last_onoff != onoff)
    {
        LIGHT_DBG("last_onoff = %d, onoff = %d\n", last_onoff, onoff);
        if(onoff == 0)
        {
            led_ctl_set_handler(0, 0, 0);
        }
        else
        {
            switch(light_type)
            {
                case LIGHT_TYPE_HSL:
                    light_para[0] = p_elem->state.hsl_hue[T_CUR];
                    light_para[1] = p_elem->state.hsl_sat[T_CUR];
                    light_para[2] = p_elem->state.hsl_lightness[T_CUR];

                    led_hsl_set_handler(light_para[0], light_para[1], light_para[2]);
                    break;
                case LIGHT_TYPE_IDLE:
                case LIGHT_TYPE_CTL:
                    light_para[0]  = p_elem->state.ctl_lightness[T_CUR];
                    light_para[1]  = p_elem->state.ctl_temp[T_CUR];
                    light_para[2]  = p_elem->state.ctl_UV[T_CUR];

                    led_ctl_set_handler(light_para[0], light_para[1], light_para[2]);
                    break;
            }
        }
        return LED_FUNC_EXEC;
    }

    return LED_FUNC_PASS;
}

static void _save_gen_onoff_srv(S_ELEM_STATE *p_elem)
{
    p_elem->powerup.last_onoff = p_elem->state.onoff[T_CUR];
    g_powerup[p_elem->elem_index].last_onoff = p_elem->state.onoff[T_CUR];
}

static led_func_ret_e _led_ln_srv(S_ELEM_STATE *p_elem)
{
    uint16_t light_ln_last = p_elem->powerup.light_ln_last;

    uint16_t actual = p_elem->state.light_ln_actual[T_CUR];

    if(light_ln_last != actual)
    {
        LIGHT_DBG("light_ln_last = %d, actual = %d\n", light_ln_last, actual);
        led_ctl_set_handler(actual, p_elem->state.ctl_temp[T_CUR], p_elem->state.ctl_UV[T_CUR]);

        return LED_FUNC_EXEC;
    }

    return LED_FUNC_PASS;
}

static void _save_ln_srv(S_ELEM_STATE *p_elem)
{
    p_elem->powerup.light_ln_last = p_elem->state.light_ln_actual[T_CUR];
    g_powerup[p_elem->elem_index].light_ln_last = p_elem->state.light_ln_actual[T_CUR];
}

static led_func_ret_e _led_ctl_srv(S_ELEM_STATE *p_elem)
{
    uint16_t ctl_ln_last = p_elem->powerup.ctl_ln_last;
    uint16_t ctl_temp_last = p_elem->powerup.ctl_temp_last;
    uint16_t ctl_uv_last = p_elem->powerup.ctl_uv_last;

    uint16_t ctl_lightness = p_elem->state.ctl_lightness[T_CUR];
    uint16_t temperature = p_elem->state.ctl_temp[T_CUR];
    uint16_t ctl_UV = p_elem->state.ctl_UV[T_CUR];

    if(ctl_ln_last != ctl_lightness || ctl_temp_last != temperature || ctl_uv_last != ctl_UV)
    {
        LIGHT_DBG("ctl_ln_last = %d, ctl_lightness = %d\n", ctl_ln_last, ctl_lightness);
        LIGHT_DBG("ctl_temp_last = %d, temperature = %d\n", ctl_temp_last, temperature);
        LIGHT_DBG("ctl_uv_last = %d, ctl_UV = %d\n", ctl_uv_last, ctl_UV);
        led_ctl_set_handler(ctl_lightness, temperature, ctl_UV);

        return LED_FUNC_EXEC;
    }

    return LED_FUNC_PASS;
}

static void _save_ctl_srv(S_ELEM_STATE *p_elem)
{
    p_elem->powerup.ctl_ln_last = p_elem->state.ctl_lightness[T_CUR];
    g_powerup[p_elem->elem_index].ctl_ln_last = p_elem->state.ctl_lightness[T_CUR];

    p_elem->powerup.ctl_temp_last = p_elem->state.ctl_temp[T_CUR];
    g_powerup[p_elem->elem_index].ctl_temp_last = p_elem->state.ctl_temp[T_CUR];

    p_elem->powerup.ctl_uv_last = p_elem->state.ctl_UV[T_CUR];
    g_powerup[p_elem->elem_index].ctl_uv_last = p_elem->state.ctl_UV[T_CUR];
}

static led_func_ret_e _led_hsl_srv(S_ELEM_STATE *p_elem)
{
    uint16_t last_hsl_lightness = p_elem->powerup.last_hsl_lightness;
    uint16_t last_hsl_hue = p_elem->powerup.last_hsl_hue;
    uint16_t last_hsl_sat = p_elem->powerup.last_hsl_sat;

    uint16_t hsl_lightness = p_elem->state.hsl_lightness[T_CUR];
    uint16_t hsl_hue = p_elem->state.hsl_hue[T_CUR];
    uint16_t hsl_sat = p_elem->state.hsl_sat[T_CUR];

    if(last_hsl_lightness != hsl_lightness || last_hsl_hue != hsl_hue || last_hsl_sat != hsl_sat)
    {
        LIGHT_DBG("last_hsl_lightness = %d, hsl_lightness = %d\n", last_hsl_lightness, hsl_lightness);
        LIGHT_DBG("last_hsl_hue = %d, hsl_hue = %d\n", last_hsl_hue, hsl_hue);
        LIGHT_DBG("last_hsl_sat = %d, hsl_sat = %d\n", last_hsl_sat, hsl_sat);
        led_hsl_set_handler(hsl_hue, hsl_sat, hsl_lightness);

        return LED_FUNC_EXEC;
    }

    return LED_FUNC_PASS;
}

static void _save_hsl_srv(S_ELEM_STATE *p_elem)
{
    p_elem->powerup.last_hsl_lightness = p_elem->state.hsl_lightness[T_CUR];
    g_powerup[p_elem->elem_index].last_hsl_lightness = p_elem->state.hsl_lightness[T_CUR];

    p_elem->powerup.last_hsl_hue = p_elem->state.hsl_hue[T_CUR];
    g_powerup[p_elem->elem_index].last_hsl_hue = p_elem->state.hsl_hue[T_CUR];

    p_elem->powerup.last_hsl_sat = p_elem->state.hsl_sat[T_CUR];
    g_powerup[p_elem->elem_index].last_hsl_sat = p_elem->state.hsl_sat[T_CUR];
}

static void _save_light_state(S_ELEM_STATE *p_elem)
{
    model_message_index_e dest_index = p_elem->message_index;

    LIGHT_DBG("dest_index %d", dest_index);

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    _save_ln_srv(p_elem);
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    _save_gen_onoff_srv(p_elem);
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    _save_ctl_srv(p_elem);
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    _save_hsl_srv(p_elem);
#endif

    p_elem->message_index = MM_INDEX_NONE;

    k_delayed_work_submit(&light_state_store_work, LIGHT_STATE_STORE_DELAY_TIME);

}

static led_func_ret_e _led_ctrl(S_ELEM_STATE *p_elem)
{
    led_func_ret_e ret = LED_FUNC_EXEC;

    model_message_index_e dest_index = p_elem->message_index;

    switch(dest_index)
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        case MM_INDEX_SRV_ONOFF:
            ret == _led_gen_onoff_srv(p_elem);
            break;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        case MM_INDEX_SRV_CTL:
            ret == _led_ctl_srv(p_elem);
            break;
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
        case MM_INDEX_SRV_HSL:
            ret == _led_hsl_srv(p_elem);
            break;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        case MM_INDEX_SRV_LN:
            ret == _led_ln_srv(p_elem);
            break;
#endif
        default:
            ret = LED_FUNC_PASS;
            break;
    }

    return ret;
}

void _cal_flash_next_step(uint32_t delta_time)
{
    uint16_t actual_end;
    if(delta_time < 1000) {
        actual_end = g_flash_para.actual_tar;
        g_flash_para.temp_cur = g_flash_para.temp_tar;
    } else {
        actual_end = g_flash_para.actual_start;
        delta_time %= 1000;
    }
    if(delta_time > LED_FLASH_ON_TIME) {
        delta_time -= LED_FLASH_ON_TIME;
        g_flash_para.actual_cur = g_flash_para.actual_start * delta_time / LED_FLASH_OFF_TIME;
    } else {
        g_flash_para.actual_cur = actual_end * (LED_FLASH_ON_TIME - delta_time) / LED_FLASH_ON_TIME;
    }
    //LIGHT_DBG("delta %d, actual %04x", delta_time, g_flash_para.actual_cur);
}

static void _led_flash_timer_cb(void *p_timer, void *p_arg)
{
    uint32_t cur_time = k_uptime_get();
    if(cur_time >= g_flash_para.time_end) {
        //_led_set(1, g_flash_para.actual_tar, g_flash_para.temp_tar);
    } else {
        _cal_flash_next_step(g_flash_para.time_end - cur_time);
        //_led_set(1, g_flash_para.actual_cur, g_flash_para.temp_cur);
        k_timer_start(&g_flash_para.timer, LED_FLASH_CYCLE);
    }
}

void _led_flash(uint8_t times, uint8_t reset)
{
    static uint8_t inited = 0;
    if(inited == 0) {
        k_timer_init(&g_flash_para.timer, _led_flash_timer_cb, NULL);
        inited = 1;
    }

    if(g_elem_state[0].state.onoff[T_CUR] == 1) {
        if(g_elem_state[0].state.light_ln_actual[T_CUR]) {
            g_flash_para.actual_start = g_flash_para.actual_cur = g_elem_state[0].state.light_ln_actual[T_CUR];
        } else {
            g_flash_para.actual_start = g_flash_para.actual_cur = LIGHTNESS_DEFAULT;
        }
        if(g_elem_state[0].state.ctl_temp[T_CUR]) {
            g_flash_para.temp_cur = g_elem_state[0].state.ctl_temp[T_CUR];
        } else {
            g_flash_para.temp_cur = CTL_TEMP_DEFAULT;
        }
        if(reset) {
            g_flash_para.actual_tar = LIGHTNESS_DEFAULT;
            g_flash_para.temp_tar = CTL_TEMP_DEFAULT;
        } else {
            g_flash_para.actual_tar = g_flash_para.actual_cur;
            g_flash_para.temp_tar = g_flash_para.temp_cur;
        }
        g_flash_para.time_end = k_uptime_get() + times*LED_FLASH_PERIOD;
    } else {
        if(g_elem_state[0].powerup.light_ln_last && !reset) {
            g_flash_para.actual_start = g_flash_para.actual_tar = g_elem_state[0].powerup.light_ln_last;
        } else {
            g_flash_para.actual_start = g_flash_para.actual_tar = LIGHTNESS_DEFAULT;
        }
        g_flash_para.actual_cur = 0;
        if(g_elem_state[0].powerup.ctl_temp_last) {
            g_flash_para.temp_cur = g_elem_state[0].powerup.ctl_temp_last;
        } else {
            g_flash_para.temp_cur = CTL_TEMP_DEFAULT;
        }
        if(reset) {
            g_flash_para.temp_tar = CTL_TEMP_DEFAULT;
        } else {
            g_flash_para.temp_tar = g_flash_para.temp_cur;
        }
        g_flash_para.time_end = k_uptime_get() + times*LED_FLASH_PERIOD - LED_FLASH_OFF_TIME;
    }
    //LIGHT_DBG("%d (%d-%d) tar %04x", times, k_uptime_get(), g_flash_para.time_end, g_flash_para.actual_tar);

    k_timer_start(&g_flash_para.timer, LED_FLASH_CYCLE);
}

static void _user_init(void)
{
    k_delayed_work_init(&light_state_store_work, light_state_store);
}

void user_event(E_GENIE_EVENT event, void *p_arg)
{
    E_GENIE_EVENT next_event = event;

    //BT_DBG_R("%s, %s %p\n", __func__, genie_event_str[event], p_arg);
    switch(event) {
        case GENIE_EVT_SW_RESET:
        case GENIE_EVT_HW_RESET_START:
            BT_DBG_R("FLASH x5");
            _led_flash(5, 1);
            break;
        case GENIE_EVT_HW_RESET_DONE:
            _reset_light_para();
            BT_DBG("GENIE_EVT_HW_RESET_DONE\n");
            break;
        case GENIE_EVT_SDK_MESH_INIT:
            _user_init();
            break;
        case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
            BT_DBG_R("FLASH x3");
            _led_flash(3, 0);
            break;
        case GENIE_EVT_SDK_TRANS_CYCLE:
        case GENIE_EVT_SDK_ACTION_DONE:
        {
            S_ELEM_STATE *p_elem = (S_ELEM_STATE *)p_arg;
            if(_led_ctrl(p_elem) == LED_FUNC_EXEC)
                _save_light_state(p_elem);
            break;
        }
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        case GENIE_EVT_SDK_INDICATE:
            break;
        case GENIE_EVT_SDK_VENDOR_MSG:
            break;
#endif
#ifdef CONFIG_GENIE_RESET_BY_REPEAT
        case GENIE_EVT_REPEAT_RESET:
            erase_reboot_uart_cmd_handler(NULL);
            break;
#endif
        default:
            break;
    }
    
    if(next_event != event) {
        genie_event(next_event, p_arg);
    }
}

#ifdef CONFIG_BT_MESH_JINGXUN
u8 JX_model_flag =0;
void vendor_C7_ack(u8_t tid);

static void app_timer_cb(void *p_timer, void *args)
{
	static uint16_t times = 0;

	times++;
	JX_model_flag =1;

    app_time_flag = 0;
//	k_delayed_work_submit(&app_timer, 1000);
	vendor_C7_ack(times);

    //BT_DBG(" ++++++++++++++++++++++ %s, times %d flag = %d ++++++++++++++++\r\n", 
     //        __func__,times, app_time_flag);
}
#endif //CONFIG_BT_MESH_JINGXUN

gpio_irq_handler_t gpio_test_func(void)
{
    //sleep_mode_enable(0);
    bk_printf("~~~~~~~gpio wake up!!!!!\r\n\r\n");
}

int application_start(int argc, char **argv)
{
    /* genie initilize */
    genie_init();

    led_startup();

#if CONFIG_UART_TEST_CMD
    uart_test_init();
#endif

    BT_INFO("BUILD_TIME:%s", __DATE__","__TIME__);
#ifdef CONFIG_BT_MESH_JINGXUN
	k_delayed_work_init(&app_timer, app_timer_cb);
#endif //CONFIG_BT_MESH_JINGXUN
    //aos_loop_run();


    sleep_mode_enable(1);

    
    gpio_dev_t gpio_test;
    gpio_test.port = GPIO_P02;

    hal_gpio_enable_irq(&gpio_test, IRQ_TRIGGER_FALLING_EDGE, gpio_test_func, NULL);

    return 0;
}


