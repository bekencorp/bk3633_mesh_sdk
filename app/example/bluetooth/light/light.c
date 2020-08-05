/* main.c - light demo */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED 1
#include "common/log.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>

#include <bluetooth.h>
#include <soc.h>
#include <api/mesh.h>
#include <vendor_model.h>
#include <api/genie_mesh.h>
#include "genie_app.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif

#define DEFAULT_MESH_GROUP1 0xC000
#define DEFAULT_MESH_GROUP2 0xCFFF


#ifdef BOARD_BK3633DEVKIT
#define WARM_PIN GPIO_P6 ///GPIO P10
#define COLD_PIN GPIO_P7 ///GPIO P11

#define LIGHT_BTN_DEBOUNCE_TIME 50 ///50ms
#define LIGHT_LEVEL_NUMBER		100 ///light 1~100 level
#define LIGHT_MIN_LEVEL			10  ///light min level
#define LIGHT_MAX_LEVEL			LIGHT_LEVEL_NUMBER  ///light max level
#define LIGHT_LEVEL_ADJ_BASE	10

#define LIGHT_LED        GPIO_P10 ///GPIO P10
#define LIGHT_LEVEL_UP   GPIO_P11 ///GPIO P11
#define LIGHT_LEVEL_DOWN GPIO_P12 ///GPIO P12
#define LIGHT_DRIVE_TYPE OUTPUT_PUSH_PULL

#define LIGHT_BTN_ONOFF      GPIO_P20 ///GPIO P20
#define LIGHT_BTN_LEVEL_UP   GPIO_P21 ///GPIO P21
#define LIGHT_BTN_LEVEL_DOWN GPIO_P22 ///GPIO P22
#define LIGHT_BTN_IRQ_TRIGGER IRQ_TRIGGER_FALLING_EDGE
#define LIGHT_BTN_INPUT_TYPE	INPUT_PULL_UP
#endif

#define MESH_ELEM_COUNT 1
#define MESH_ELEM_STATE_COUNT MESH_ELEM_COUNT

#define MESH_PBADV_TIME 600 //unit:s

typedef struct {
#if 0
    bool onoff;
    u16_t lightness;
    u16_t temp;
    bool lightness_changing;
    bool temp_changing;
#endif
    gpio_dev_t warm_pin;
    gpio_dev_t cold_pin;
    pwm_dev_t warm_pwm;
    pwm_dev_t cold_pwm;
} _LIGHT_CTL;

static uint8_t led_ctrl(gpio_dev_t *led, bool on);
static uint8_t leds_init(void);
static uint8_t btns_init(void);


static gpio_dev_t light_led;
static gpio_dev_t light_level_up;
static gpio_dev_t light_level_down;

static gpio_dev_t light_btn_onoff;
static gpio_dev_t light_btn_level_up;
static gpio_dev_t light_btn_level_down;

_LIGHT_CTL g_light_ctl;

S_ELEM_STATE g_elem_state[MESH_ELEM_STATE_COUNT];
S_MODEL_POWERUP g_powerup[MESH_ELEM_STATE_COUNT];


static struct bt_mesh_model element_models[] = {
    BT_MESH_MODEL_CFG_SRV(),
    BT_MESH_MODEL_HEALTH_SRV(),

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    MESH_MODEL_GEN_ONOFF_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    MESH_MODEL_GEN_LEVEL_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    MESH_MODEL_LIGHTNESS_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    MESH_MODEL_CTL_SRV(&g_elem_state[0]),
#endif

#ifndef CONFIG_ALI_SIMPLE_MODLE
    MESH_MODEL_CTL_SETUP_SRV(&g_elem_state[0]),
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    MESH_MODEL_HSL_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_SETUP_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_HUE_SRV(&g_elem_state[0]),
    MESH_MODEL_HSL_SAT_SRV(&g_elem_state[0]),
#endif

};

static struct bt_mesh_model g_element_vendor_models[] = {

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    MESH_MODEL_VENDOR_SRV(&g_elem_state[0]),
#endif
};

struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, element_models, g_element_vendor_models, 0),
};

uint32_t get_mesh_pbadv_time(void)
{
    return MESH_PBADV_TIME*1000;
}

uint8_t get_vendor_element_num(void)
{
    return MESH_ELEM_COUNT;
}

void mesh_group_init(u16_t *p_group)
{
    uint16_t group_list[CONFIG_BT_MESH_MODEL_GROUP_COUNT];
    memset(group_list, 0, sizeof(group_list));

#ifdef DEFAULT_MESH_GROUP1
    group_list[0] = DEFAULT_MESH_GROUP1;
#endif

#ifdef DEFAULT_MESH_GROUP2
    group_list[1] = DEFAULT_MESH_GROUP2;
#endif

    memcpy(p_group, group_list, sizeof(group_list));
}

void user_data_init(void)
{

    //read flash
    memset(g_elem_state, 0, sizeof(g_elem_state));
    elem_state_init(MESH_ELEM_STATE_COUNT, g_elem_state);

    load_light_state();
    //tmall_mesh_user_task_init();
}

void reset_light_para(void)
{
    uint8_t i = 0;

    while(i < MESH_ELEM_STATE_COUNT) {
#ifdef CONFIG_ALI_SIMPLE_MODLE
        g_elem_state[i].state.onoff[T_CUR] = GEN_ONOFF_DEFAULT;
        g_elem_state[i].state.actual[T_CUR] = LIGHTNESS_DEFAULT;
        g_elem_state[i].state.temp[T_CUR] = CTL_TEMP_DEFAULT;
        g_elem_state[i].state.onoff[T_TAR] = GEN_ONOFF_DEFAULT;
        g_elem_state[i].state.actual[T_TAR] = LIGHTNESS_DEFAULT;
        g_elem_state[i].state.temp[T_TAR] = CTL_TEMP_DEFAULT;
        g_elem_state[i].state.trans = 0;
        g_elem_state[i].state.delay = 0;
        g_elem_state[i].state.trans_start_time = 0;
        g_elem_state[i].state.trans_end_time = 0;
        g_elem_state[i].powerup.last_actual = LIGHTNESS_DEFAULT;
        g_elem_state[i].powerup.last_temp = CTL_TEMP_DEFAULT;
        g_powerup[i].last_actual = LIGHTNESS_DEFAULT;
        g_powerup[i].last_temp = CTL_TEMP_DEFAULT;
        
#endif
        i++;
    }

    genie_flash_write_userdata(GFI_MESH_POWERUP, g_powerup, sizeof(g_powerup));

    BT_DBG("+ done");
}


void user_prov_complete(u16_t net_idx, u16_t addr)
{
    //flash 3 timers
}

void user_prov_reset(void)
{
    //flash 5 timers
}

void user_init()
{
    printf("%s, %d\r\n\r\n", __func__, __LINE__);
#if 0
    g_light_ctl.warm_pwm.port = 0;
    g_light_ctl.warm_pwm.config.freq = 8000;
    g_light_ctl.warm_pwm.config.duty_cycle = 2000;
    g_light_ctl.cold_pwm.port = 1;
    g_light_ctl.cold_pwm.config.freq = 8000;
    g_light_ctl.cold_pwm.config.duty_cycle = 2000;

    hal_pwm_init(&g_light_ctl.warm_pwm);
    hal_pwm_init(&g_light_ctl.cold_pwm);

    hal_pwm_start(&g_light_ctl.warm_pwm);
    hal_pwm_start(&g_light_ctl.cold_pwm);

    // int pins
    g_light_ctl.warm_pin.port = WARM_PIN;
    g_light_ctl.warm_pin.config = PWM_CHANNEL_1;
    g_light_ctl.cold_pin.port = COLD_PIN;
    g_light_ctl.cold_pin.config = PWM_CHANNEL_2;

    hal_gpio_init(&g_light_ctl.warm_pin);
    hal_gpio_init(&g_light_ctl.cold_pin);
#endif

    user_data_init();
    leds_init();
    // btns_init();

    //light_action(0);

    BT_DBG("+ done");
}

void led_flash(uint8_t times)
{
    printk("%s %d\n", times);
    aos_msleep(times * 1000);
}

void save_light_state(S_ELEM_STATE *p_elem)
{
    p_elem->powerup.last_actual = p_elem->state.actual[T_CUR];
    p_elem->powerup.last_temp = p_elem->state.temp[T_CUR];
    g_powerup[p_elem->elem_index].last_actual = p_elem->state.actual[T_CUR];
    g_powerup[p_elem->elem_index].last_temp = p_elem->state.temp[T_CUR];

    BT_DBG_R("elem %d, actual %d temp %d", p_elem->elem_index, g_powerup[p_elem->elem_index].last_actual, g_powerup[p_elem->elem_index].last_temp);

    genie_flash_write_userdata(GFI_MESH_POWERUP, g_powerup, sizeof(g_powerup));
}

void load_light_state(void)
{
    uint8_t i = 0;
    uint16_t size = sizeof(g_powerup);
    E_GENIE_FLASH_ERRCODE ret = genie_flash_read_userdata(GFI_MESH_POWERUP, (uint8_t *)g_powerup, &size);

    if(ret == GENIE_FLASH_SUCCESS) {
        while(i < MESH_ELEM_STATE_COUNT) {
            memcpy(&g_elem_state[i].powerup, &g_powerup[i], sizeof(S_MODEL_POWERUP));
            BT_DBG_R("elem %d, actual %d temp %d", i, g_powerup[i].last_actual, g_powerup[i].last_temp);
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            if(g_powerup[i].last_actual) {
                g_elem_state[i].state.actual[T_TAR] = g_powerup[i].last_actual;
            }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            if(g_powerup[i].last_temp) {
                g_elem_state[i].state.temp[T_TAR] = g_powerup[i].last_temp;
            }
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            if(g_elem_state[i].state.actual[T_TAR] == 0) {
                g_elem_state[i].state.onoff[T_TAR] = 0;
            } else {
                g_elem_state[i].state.onoff[T_TAR] = 1;
            }
#endif
#ifdef CONFIG_MESH_MODEL_TRANS
            if(g_elem_state[i].state.onoff[T_TAR] == 0) {
                g_elem_state[i].state.trans = 0;
                g_elem_state[i].state.delay = 0;
                g_elem_state[i].state.trans_start_time = 0;
                g_elem_state[i].state.trans_end_time = 0;
            } else {
                g_elem_state[i].state.trans = 0x41;
                g_elem_state[i].state.delay = 100;
                g_elem_state[i].state.trans_start_time = k_uptime_get() + g_elem_state[i].state.delay * 5;
                g_elem_state[i].state.trans_end_time = g_elem_state[i].state.trans_start_time + get_transition_time(g_elem_state[i].state.trans);
            }
#endif
#endif
            i++;
        }
        BT_DBG("+ done");
    }
}

u16_t vendor_model_msg_handle(vnd_model_msg *p_msg)
{
    printk("vendor model message received\n");
    if (!p_msg)
        return -1;

    printk("opcode:0x%x, tid:%d, len:%d", p_msg->opid, p_msg->tid, p_msg->len);
    if (p_msg->data && p_msg->len)
        printk("payload: %s", bt_hex(p_msg->data, p_msg->len));

    switch (p_msg->opid) {
        case VENDOR_OP_ATTR_GET_STATUS:
            /* report VENDOR_OP_ATTR_STATUS */
            //_light_report_status();
            break;
        case VENDOR_OP_ATTR_SET_ACK:
            /* TODO: set status
             * report VENDOR_OP_ATTR_STATUS
             * */
            //_light_report_status();
            break;
        case VENDOR_OP_ATTR_SET_UNACK:
            /* TODO: set status */
            break;
        case VENDOR_OP_ATTR_CONFIME:
            /* clear indicate status */

            break;
        case VENDOR_OP_ATTR_TRANS_MSG:
            break;
        default:
            break;
    }

    return 0;
}

#if defined(CONFIG_MESH_MODEL_CTL_SRV)
static void _led_set(uint8_t elem_index, uint8_t on, uint16_t actual, uint16_t temperature)
{
    static uint8_t last_onoff = 0;
    static uint16_t last_acual = 0;
    printk(">>>>elem %d set %d %d %d\n", elem_index, on, actual, temperature);
    if(last_onoff != on) {
        last_onoff = on;
        led_ctrl(&light_led, last_onoff);
    }
    if(last_acual != actual) {
        last_acual = actual;
    }
}
#elif defined(CONFIG_MESH_MODEL_LIGHTNESS_SRV)
static void _led_set(uint8_t elem_index, uint8_t on, uint16_t actual)
{
    static uint8_t last_onoff = 0;
    static uint16_t last_acual = 0;
    printk(">>>>elem %d set %d %d\n", elem_index, on, actual);
    if(last_onoff != on) {
        last_onoff = on;
        led_ctrl(&light_led, last_onoff);
    }
    if(last_acual != actual) {
        last_acual = actual;
    }
}
#elif defined(CONFIG_MESH_MODEL_GEN_ONOFF_SRV)
static void _led_set(uint8_t elem_index, uint8_t on)
{
    static uint8_t last_onoff = 0;
    printk(">>>>elem %d set %d\n", elem_index, on);
    if(last_onoff != on) {
        last_onoff = on;
        led_ctrl(&light_led, last_onoff);
    }
}
#endif

void user_event(E_GENIE_EVENT event, void *p_arg)
{
    E_GENIE_EVENT next_event = event;

    //BT_DBG_R("%s, %s %p\n", __func__, genie_event_str[event], p_arg);
    switch(event) {
        case GENIE_EVT_SW_RESET:
        case GENIE_EVT_HW_RESET_START:
            BT_DBG_R("FLASH x5");
            led_flash(5);
            reset_light_para();
            break;
        case GENIE_EVT_SDK_MESH_INIT:
            user_init();
            next_event = GENIE_EVT_SDK_ANALYZE_MSG;
            break;
        case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
            BT_DBG_R("FLASH x3");
            led_flash(3);
            break;
#ifdef CONFIG_MESH_MODEL_TRANS
        case GENIE_EVT_SDK_TRANS_CYCLE:
#endif
        case GENIE_EVT_SDK_ACTION_DONE:
        {
            S_ELEM_STATE *p_elem = (S_ELEM_STATE *)p_arg;
#if defined(CONFIG_MESH_MODEL_CTL_SRV)
            _led_set(p_elem->elem_index, p_elem->state.onoff[T_CUR], p_elem->state.actual[T_CUR], p_elem->state.temp[T_CUR]);
#elif defined(CONFIG_MESH_MODEL_LIGHTNESS_SRV)
            _led_set(p_elem->elem_index, p_elem->state.onoff[T_CUR], p_elem->state.actual[T_CUR]);
#elif defined(CONFIG_MESH_MODEL_GEN_ONOFF_SRV)
            _led_set(p_elem->elem_index, p_elem->state.onoff[T_CUR]);
#endif
            if(event == GENIE_EVT_SDK_ACTION_DONE)
                save_light_state(p_elem);
            break;
        }
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        case GENIE_EVT_SDK_INDICATE:
            break;
        case GENIE_EVT_SDK_VENDOR_MSG:
        {
            vendor_model_msg_handle((vnd_model_msg *)p_arg);
            break;
        }
#endif
        case GENIE_EVT_HW_RESET_DONE:
            printk("GENIE_EVT_HW_RESET_DONE\n");
            break;
        default:
            break;
    }
    
    if(next_event != event) {
        genie_event(next_event, p_arg);
    }
}

static uint8_t led_ctrl(gpio_dev_t *led, bool on)
{
    if (!led)
        return -1;

    if (on)
        hal_gpio_output_low(led);
    else
        hal_gpio_output_high(led);
}

uint8_t light_turn_onoff(bool on)
{
    #if 0
    S_MESH_STATE *p_state = &g_elem_state[0].state;

    if (g_light_ctl.onoff != on) {
        p_state->onoff[T_TAR] = on;
        genie_light_action_notify(&g_elem_state[0]);
    }
#endif
    return 0;
}

s8_t light_adjust_lightness(u8_t l)
{
    #if 0
    S_MESH_STATE *p_state = &g_elem_state[0].state;
    u16_t lightness = (0xFFFF/100) * l; // calculate roughly, don't care about lightness's range

    if (!l) {
        printk("invalid lightness value:0\n");
        return 0;
    }

    p_state->onoff[T_TAR] = true;
    p_state->actual[T_TAR] = lightness;
    genie_light_action_notify(&g_elem_state[0]);

    printk("%s:%d\n", __func__, __LINE__);
#endif
    return 0;
}

/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static u8_t leds_init(void)
{
    light_led.port = LIGHT_LED;
    light_led.config = LIGHT_DRIVE_TYPE;
    hal_gpio_init(&light_led);

    light_level_up.port = LIGHT_LEVEL_UP;
    light_level_up.config = LIGHT_DRIVE_TYPE;
    hal_gpio_init(&light_level_up);

    light_level_down.port = LIGHT_LEVEL_DOWN;
    light_level_down.config = LIGHT_DRIVE_TYPE;
    hal_gpio_init(&light_level_down);
    printk("%s:%d\n", __func__, __LINE__);
    return 0;
}

static void light_btn_onoff_gpio_irq_handler(void *arg)
{
    //printk("%s:%d\r\n", __func__, __LINE__);
}

static void light_btn_level_up_gpio_irq_handler(void *arg)
{
    //printk("%s:%d\r\n", __func__, __LINE__);
}


/**@brief   Function for the BTNs initialization.
 *
 * @details Initializes all BTNs used by this application.
 */
static uint8_t btns_init(void)
{
    light_btn_onoff.port = LIGHT_BTN_ONOFF;
    light_btn_onoff.config = LIGHT_BTN_INPUT_TYPE;
    hal_gpio_init(&light_btn_onoff);
    hal_gpio_enable_irq(&light_btn_onoff, LIGHT_BTN_IRQ_TRIGGER,
    light_btn_onoff_gpio_irq_handler, NULL);

    light_btn_level_up.port = LIGHT_BTN_LEVEL_UP;
    light_btn_level_up.config = LIGHT_BTN_INPUT_TYPE;
    hal_gpio_init(&light_btn_level_up);
    hal_gpio_enable_irq(&light_btn_level_up, LIGHT_BTN_IRQ_TRIGGER,
    light_btn_level_up_gpio_irq_handler, NULL);

    light_btn_level_down.port = LIGHT_BTN_LEVEL_DOWN;
    light_btn_level_down.config = LIGHT_BTN_INPUT_TYPE;
    hal_gpio_init(&light_btn_level_down);
    printk("%s:%d\n", __func__, __LINE__);
    return 0;
}

int application_start(int argc, char **argv)
{
    printk("%s:%d\n", __func__, __LINE__);
    ais_ota_bt_storage_init();

    // init genie (app & mesh)
    genie_init();

    //aos init
    return 0;
}


