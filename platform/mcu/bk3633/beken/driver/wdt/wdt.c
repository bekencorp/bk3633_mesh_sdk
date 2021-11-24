#include "include.h"
#include "arm_arch.h"

#include "wdt_pub.h"
#include "wdt.h"
#include "icu_pub.h"

#include "drv_model_pub.h"
#include "ll.h"

static SDD_OPERATIONS wdt_op = {
            wdt_ctrl
};
static uint32_t g_wdt_period = 0;

/*******************************************************************/
#if 1
void wdt_init(void)
{
	sddev_register_dev(WDT_DEV_NAME, &wdt_op);
}

void wdt_exit(void)
{
	sddev_unregister_dev(WDT_DEV_NAME);
}

static void Delay_us(int num)
{
    volatile int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 10; x++);
    }
}

UINT32 wdt_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret;
	UINT32 reg;
	UINT32 parameter;

	ret = WDT_SUCCESS;	
	switch(cmd)
	{		
		case WCMD_POWER_DOWN:
			g_wdt_period = 0;
			
			parameter = CLK_PWR_DEV_WDT;
		    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&parameter);
		    ASSERT(ICU_SUCCESS == ret);	
			break;
			
		case WCMD_POWER_UP:
			parameter = CLK_PWR_DEV_WDT;
		    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&parameter);
		    ASSERT(ICU_SUCCESS == ret);	
			break;
			
		case WCMD_RELOAD_PERIOD: {
		    GLOBAL_INT_DISABLE();
			reg = WDT_1ST_KEY << WDT_KEY_POSI;
			reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			GLOBAL_INT_RESTORE();
			Delay_us(3);
			GLOBAL_INT_DISABLE();
			reg = WDT_2ND_KEY << WDT_KEY_POSI;
			reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			GLOBAL_INT_RESTORE();
		}
			break;

		case WCMD_SET_PERIOD: {				
			ASSERT(param);
			GLOBAL_INT_DISABLE();
			g_wdt_period = (*(UINT32 *)param);
			
			reg = WDT_1ST_KEY << WDT_KEY_POSI;
			reg |= ((*(UINT32 *)param) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			GLOBAL_INT_RESTORE();
			Delay_us(3);
			GLOBAL_INT_DISABLE();
			reg = WDT_2ND_KEY << WDT_KEY_POSI;
			reg |= ((*(UINT32 *)param) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			GLOBAL_INT_RESTORE();
		}
			break;
			
		default:
			break;
	}

    return ret;
}
#endif

// EOF
