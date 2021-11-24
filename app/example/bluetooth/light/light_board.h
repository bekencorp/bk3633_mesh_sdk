#ifndef __LIGHT_BOARD_H
#define __LIGHT_BOARD_H

#include "common/log.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bluetooth.h>
#include <soc.h>
#include <api/mesh.h>
#include "genie_app.h"
#include "gpio_pub.h"


#define CONFIG_INFO_ENABLE IS_ENABLED(CONFIG_BT_MESH_DEBUG_LIGHT)

#ifdef CONFIG_INFO_ENABLE
#define LIGHT_DBG(fmt, ...)  printf("[%s]"fmt"\n", __func__, ##__VA_ARGS__)
#define LOG_EN_DBG(fmt, ...)  printf("[%s]"fmt"\n", __func__, ##__VA_ARGS__)
#else
#define LIGHT_DBG(fmt, ...)
#endif


#define LIGHT_CTL_TEMP_MIN            (0x0320)    // 800
#define LIGHT_CTL_TEMP_MAX            (0x4E20)    // 20000

#define LED_FLASH_CYCLE               20//MESH_TRNSATION_CYCLE

#define LED_FLASH_PERIOD              2000
#define LED_FLASH_ON_TIME             1200
#define LED_FLASH_OFF_TIME            800

#define  LED_R                        GPIO_P33//GPIO_P10  //PWM0-P10
#define  LED_G                        GPIO_P33//GPIO_P11  //PWM1-P11
#define  LED_B                        GPIO_P33//GPIO_P12  //PWM2-P12

#define  GPIO_TO_PWM(x)               (x - GPIO_P10)

#define  LIGHT_PERIOD                 1000

#define LN_RATIO(ln)                  (LIGHTNESS_MAX - ln)            //for GPIO low is led on


/* element configuration start */
#define MESH_ELEM_COUNT               1
#define MESH_ELEM_STATE_COUNT         MESH_ELEM_COUNT

#define LIGHT_STATE_STORE_DELAY_TIME  (5000)

typedef struct{
    pwm_dev_t light_led_r;
    pwm_dev_t light_led_g;
    pwm_dev_t light_led_b;
} light_led_t;

typedef enum {
    LIGHT_TYPE_IDLE = 0,
    LIGHT_TYPE_CTL,
    LIGHT_TYPE_HSL,
    LIGHT_TYPE_MAX,
} light_type_e;

typedef struct{
    light_led_t light_led;
    light_type_e light_type;

} led_board_env_t;

typedef struct{
    struct k_timer timer;
    uint16_t temp_cur;
    uint16_t temp_tar;
    uint16_t actual_start;
    uint16_t actual_cur;
    uint16_t actual_tar;
    uint32_t time_end;
} led_flash_t;

/* unprovision device beacon adv time */
#define MESH_PBADV_TIME               60//600 //unit:s
#define DEFAULT_MESH_GROUP1           0xC000
#define DEFAULT_MESH_GROUP2           0xCFFF

typedef enum {
    LED_R_CHANNEL = 0,
    LED_G_CHANNEL,
    LED_B_CHANNEL,
    LED_CHANNEL_MAX
} light_channel_e;

typedef enum {
    LED_FUNC_PASS = 0,
    LED_FUNC_EXEC,

} led_func_ret_e;

extern S_MODEL_POWERUP g_powerup[MESH_ELEM_STATE_COUNT];
extern S_ELEM_STATE g_elem_state[MESH_ELEM_STATE_COUNT];


int led_startup(void);
void _init_light_para(void);
void _led_flash(uint8_t times, uint8_t reset);

void set_light_board_type(light_type_e type);
light_type_e get_light_board_type(void);

#endif //__LIGHT_BOARD_H