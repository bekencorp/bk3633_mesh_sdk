#include "include.h"
#include "fake_clock_pub.h"
#include "pwm_pub.h"
#include "pwm_init.h"

#include "icu_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

#if CFG_SUPPORT_ALIOS
#include "rtos_pub.h"
#include "ll.h"
#include "k_api.h"
#endif

static volatile UINT32 current_clock = 0;
static volatile UINT32 current_seconds = 0;
static UINT32 second_countdown;
static UINT32 fclk_second;
static UINT16 ticks_per_second;

UINT32 fclk_update_tick(UINT32 tick)
{
    current_clock += tick;

    if(second_countdown <= tick)
    {
        current_seconds ++;
        second_countdown = fclk_second - (tick - second_countdown);
    }
    else
    {
        second_countdown -= tick;
    }

    return 0;
}

void fclk_hdl(UINT8 param)
{
    fclk_update_tick(1ul);

#if CFG_SUPPORT_ALIOS
    krhino_tick_proc();
#endif
}

UINT32 fclk_get_tick(void)
{
    uint32_t tick;
    
    GLOBAL_INT_DISABLE();
    tick = current_clock;
    GLOBAL_INT_RESTORE();

    return tick;
}

UINT32 fclk_get_second(void)
{
    return current_seconds;
}

UINT32 fclk_from_sec_to_tick(UINT32 sec)
{
    return sec * ticks_per_second;
}

UINT32 fclk_tick_a_second(void)
{
    return ticks_per_second;
}

void fclk_reset_count(void)
{
    current_clock = 0;
    current_seconds = 0;
}


void fclk_disable(UINT8 pwm_id)
{
    pwm_param_t pwm_drv_desc;
    memset(&pwm_drv_desc, 0, sizeof(pwm_param_t));
    pwm_drv_desc.channel         = pwm_id;
    sddev_control(PWM_DEV_NAME, CMD_PWM_DEINIT_PARAM, &pwm_drv_desc);
}

void fclk_init(UINT8 pwm_id, UINT16 ticks_per_sec)
{
	pwm_param_t pwm_drv_desc;
	ticks_per_second = ticks_per_sec;

    /*initial timer*/
    pwm_drv_desc.channel         = pwm_id;
    pwm_drv_desc.cfg.bits.en     = PWM_ENABLE;
    pwm_drv_desc.cfg.bits.int_en = PWM_INT_EN;
    pwm_drv_desc.cfg.bits.mode   = PMODE_TIMER;
    pwm_drv_desc.p_Int_Handler   = fclk_hdl;

    ///select base clock

    fclk_second = 16000000ul/ticks_per_second;
    pwm_drv_desc.cfg.bits.clk = PWM_CLK_16M;
    pwm_drv_desc.end_value = 160000;
    pwm_drv_desc.contiu_mode = 0;
    pwm_drv_desc.cpedg_sel = 1;
    pwm_drv_desc.pre_divid = 0;

    pwm_drv_desc.duty_cycle = pwm_drv_desc.end_value/2;
    second_countdown = fclk_second;

    // os_printf("PWM%d OS, clock:%dKMz, %d ticks/s\r\n",
    //            pwm_id, fclk_second/10000, ticks_per_sec);

    sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &pwm_drv_desc);
}

// eof

