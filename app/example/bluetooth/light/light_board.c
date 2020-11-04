/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <hal/soc/pwm.h>
#include <hal/soc/gpio.h>
#include "light_board.h"



static led_board_env_t led_env;

/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void _led_init(void)
{
    light_led_t led = led_env.light_led;

    led.light_led_r.port = GPIO_TO_PWM(LED_R);
    led.light_led_r.config.duty_cycle = 0;
    led.light_led_r.config.freq = LIGHT_PERIOD;
    hal_pwm_init(&led.light_led_r);
    hal_pwm_start(&led.light_led_r);


    led.light_led_g.port = GPIO_TO_PWM(LED_G);
    led.light_led_g.config.duty_cycle = 0;
    led.light_led_g.config.freq = LIGHT_PERIOD;
    hal_pwm_init(&led.light_led_g);
    hal_pwm_start(&led.light_led_g);


    led.light_led_b.port = GPIO_TO_PWM(LED_B);
    led.light_led_b.config.duty_cycle = 0;
    led.light_led_b.config.freq = LIGHT_PERIOD;
    hal_pwm_init(&led.light_led_b);
    hal_pwm_start(&led.light_led_b);

    set_light_board_type(LIGHT_TYPE_IDLE);

}

int led_startup(void)
{
    _led_init();
    _init_light_para();
#if defined(CONFIG_GENIE_OTA)
    uint8_t ota_flag = ais_get_ota_indicat();
    if (!(ota_flag && g_powerup[0].last_onoff == 0)) {
        _led_flash(1, 0);
        //g_elem_state[0].state.onoff[T_CUR] = g_elem_state[0].state.onoff[T_TAR] = 1;
    }
#endif //CONFIG_GENIE_OTA
}

void set_light_board_type(light_type_e type)
{
    led_env.light_type = type;
}

light_type_e get_light_board_type(void)
{
    return led_env.light_type;
}

static const uint32_t temperature_map[] =
{
    0xff2000, 0xff2700, 0xff3300, 0xff4500, 0xff5200, 0xff5d00, 0xff6600, 0xff6f00,
    0xff7600, 0xff7c00, 0xff8200, 0xff8700, 0xff8d0b, 0xff921d, 0xff9829, 0xff9d33,
    0xffa23c, 0xffa645, 0xffaa4d, 0xffae54, 0xffb25b, 0xffb662, 0xffb969, 0xffbd6f,
    0xffc076, 0xffc37c, 0xffc682, 0xffc987, 0xffcb8d, 0xffce92, 0xffd097, 0xffd39c,
    0xffd5a1, 0xffd7a6, 0xffd9ab, 0xffdbaf, 0xffddb4, 0xffdfb8, 0xffe1bc, 0xffe2c0,
    0xffe4c4, 0xffe5c8, 0xffe7cc, 0xffe8d0, 0xffead3, 0xffebd7, 0xffedda, 0xffeede,
    0xffefe1, 0xfff0e4, 0xfff1e7, 0xfff3ea, 0xfff4ed, 0xfff5f0, 0xfff6f3, 0xfff7f5,
    0xfff8f8, 0xfff9fb, 0xfff9fd, 0xfefaff, 0xfcf8ff, 0xfaf7ff, 0xf7f5ff, 0xf5f4ff,
    0xf3f3ff, 0xf1f1ff, 0xeff0ff, 0xeeefff, 0xeceeff, 0xeaedff, 0xe9ecff, 0xe7eaff,
    0xe5e9ff, 0xe4e9ff, 0xe3e8ff, 0xe1e7ff, 0xe0e6ff, 0xdfe5ff, 0xdde4ff, 0xdce3ff,
    0xdbe2ff, 0xdae2ff, 0xd9e1ff, 0xd8e0ff, 0xd7dfff, 0xd6dfff, 0xd5deff, 0xd4ddff,
    0xd3ddff, 0xd2dcff, 0xd1dcff, 0xd0dbff, 0xcfdaff, 0xcfdaff, 0xced9ff, 0xcdd9ff,
    0xccd8ff, 0xccd8ff, 0xcbd7ff, 0xcad7ff, 0xcad6ff, 0xc9d6ff, 0xc8d5ff, 0xc8d5ff,
    0xc7d4ff, 0xc6d4ff, 0xc6d4ff, 0xc5d3ff, 0xc5d3ff, 0xc4d2ff, 0xc4d2ff, 0xc3d2ff,
    0xc3d1ff, 0xc2d1ff, 0xc2d0ff, 0xc1d0ff, 0xc1d0ff, 0xc0cfff, 0xc0cfff, 0xbfcfff,
    0xbfceff, 0xbeceff, 0xbeceff, 0xbeceff, 0xbdcdff, 0xbdcdff, 0xbccdff, 0xbcccff,
    0xbcccff, 0xbbccff, 0xbbccff, 0xbbcbff, 0xbacbff, 0xbacbff, 0xbacbff, 0xb9caff,
    0xb9caff, 0xb9caff, 0xb8caff, 0xb8c9ff, 0xb8c9ff, 0xb8c9ff, 0xb7c9ff, 0xb7c9ff,
    0xb7c8ff, 0xb6c8ff, 0xb6c8ff, 0xb6c8ff, 0xb6c8ff, 0xb5c7ff, 0xb5c7ff, 0xb5c7ff,
    0xb5c7ff, 0xb4c7ff, 0xb4c6ff, 0xb4c6ff, 0xb4c6ff, 0xb3c6ff, 0xb3c6ff, 0xb3c6ff,
    0xb3c5ff, 0xb3c5ff, 0xb2c5ff, 0xb2c5ff, 0xb2c5ff, 0xb2c5ff, 0xb2c4ff, 0xb1c4ff,
    0xb1c4ff, 0xb1c4ff, 0xb1c4ff, 0xb1c4ff, 0xb0c4ff, 0xb0c3ff, 0xb0c3ff, 0xb0c3ff,
    0xb0c3ff, 0xb0c3ff, 0xafc3ff, 0xafc3ff, 0xafc2ff, 0xafc2ff, 0xafc2ff, 0xafc2ff,
    0xaec2ff, 0xaec2ff, 0xaec2ff, 0xaec2ff, 0xaec2ff, 0xaec1ff, 0xaec1ff, 0xadc1ff,
    0xadc1ff
};

static float _hue_2_rgb(float v1, float v2, float h)
{
    if (h < 0)
    {
        h += 1;
    }
    else if (h > 1)
    {
        h -= 1;
    }
    if (6 * h < 1)
    {
        return v1 + (v2 - v1) * 6 * h;
    }
    else if (2 * h < 1)
    {
        return v2;
    }
    else if (3 * h < 2)
    {
        return v1 + (v2 - v1) * (4 - 6 * h);
    }
    else
    {
        return v1;
    }
}

static uint8_t _color_16to8(uint16_t color)
{
    return color / 65535.0 * 255;
}

static  uint16_t _color_8to16(uint8_t color)
{
    return color / 255.0 * 65535;
}

static uint32_t _temperature_to_rgb(uint16_t temperature, int16_t delta_uv)
{
    // UNUSED(delta_uv);
    uint16_t index = round((temperature - 800) / 100.0);
    if(index > sizeof(temperature_map)/sizeof(uint32_t))
    {
        index = sizeof(temperature_map)/sizeof(uint32_t) - 1;
    }

    return temperature_map[index];
}

static void _hsl_2_rgb(uint16_t rgb[LED_CHANNEL_MAX], uint16_t hsl[LED_CHANNEL_MAX])
{
    if (hsl[1] == 0)
    {
        rgb[0] = rgb[1] = rgb[2] = hsl[2];
    }
    else
    {
        float h, s, l, v1, v2;
        h = (float)hsl[0] / 65535.0;
        s = (float)hsl[1] / 65535.0;
        l = (float)hsl[2] / 65535.0;

        if (l < 0.5f)
        {
            v2 = l * (1.0 + s);
        }
        else
        {
            v2 = (l + s) - (s * l);
        }

        v1 = 2.0 * l - v2;

        rgb[0] = ceil(65535 * _hue_2_rgb(v1, v2, h + 1.0 / 3));
        rgb[1] = ceil(65535 * _hue_2_rgb(v1, v2, h));
        rgb[2] = ceil(65535 * _hue_2_rgb(v1, v2, h - 1.0 / 3));
    }
}


static void _light_lighten(light_channel_e channel, uint16_t state)
{
    uint32_t led_pwm_count = 16000000/LIGHT_PERIOD;
    uint32_t high_count;
    #if 0
    uint16_t state_cal = LN_RATIO(state);
	#else   ///for test led onoff state 200928
	uint16_t state_cal = 0;
    if(state==0){
        state_cal=0;
	}
    else{  
		state_cal= LN_RATIO(state);
	}
	#endif

    high_count = led_pwm_count * state_cal / LIGHTNESS_MAX;

    LIGHT_DBG("channel %d, led_pwm_count 0x%x high_count = 0x%x\r\n", channel, led_pwm_count, high_count);
    hal_pwm_duty_cycle_chg(channel, led_pwm_count, high_count);
}

static void _light_set_rgb(uint16_t rgb[LED_CHANNEL_MAX])
{
    light_channel_e channel;

    LIGHT_DBG("rgb[0] = 0x%x, rgb[1] = 0x%x, rgb[2] = 0x%x\r\n", rgb[0], rgb[1], rgb[2]);

    light_channel_e ledId;

    for (ledId = LED_R_CHANNEL; ledId < LED_CHANNEL_MAX; ledId++)
    {
        _light_lighten(ledId, rgb[ledId]);
    } 
}

void led_ctl_set_handler(uint16_t ctl_lightness, uint16_t temperature, uint16_t ctl_UV)
{
    uint16_t rgb_cal[LED_CHANNEL_MAX];

    LIGHT_DBG("ctl_lightness = 0x%x, temperature = 0x%x, delta_uv = 0x%x\n", ctl_lightness, temperature, ctl_UV);

    if(ctl_lightness > LIGHTNESS_MAX || temperature > LIGHT_CTL_TEMP_MAX)
    {
        LIGHT_DBG("Wrong parameter!!!!!!\n");
        return;
    }

    uint32_t rgb = _temperature_to_rgb(temperature, ctl_UV);

    rgb_cal[0] = _color_8to16(rgb >> 16) * ctl_lightness / LIGHTNESS_MAX;
    rgb_cal[1] = _color_8to16(rgb >> 8) * ctl_lightness / LIGHTNESS_MAX;
    rgb_cal[2] = _color_8to16(rgb) * ctl_lightness / LIGHTNESS_MAX;
    _light_set_rgb(rgb_cal);

}

void led_hsl_set_handler(uint16_t hue, uint16_t saturation, uint16_t lightness)
{
    LIGHT_DBG("hue = 0x%x, sat = 0x%x, ln = 0x%x\n", hue, saturation, lightness);

    uint16_t rgb_cal[LED_CHANNEL_MAX];
    uint16_t hsl[LED_CHANNEL_MAX];
    uint16_t rgb[LED_CHANNEL_MAX];

    hsl[0] = hue;
    hsl[1] = saturation;
    hsl[2] = lightness;

    _hsl_2_rgb(rgb, hsl);
    LIGHT_DBG("r = %d,g = %d,b = %d\n", rgb[0], rgb[1], rgb[2]);

    rgb_cal[0] = rgb[0] * lightness / LIGHTNESS_MAX;
    rgb_cal[1] = rgb[1] * lightness / LIGHTNESS_MAX;
    rgb_cal[2] = rgb[2] * lightness / LIGHTNESS_MAX;
    _light_set_rgb(rgb_cal);
}


