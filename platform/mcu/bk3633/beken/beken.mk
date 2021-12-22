NAME := beken

$(NAME)_TYPE := kernel

#$(NAME)_COMPONENTS += platform.mcu.bk3633.beken.ble
#$(NAME)_COMPONENTS += platform.mcu.bk3633.beken.controller

GLOBAL_DEFINES  += ENABLE_BLE_CONTROLLER

### global include ###
GLOBAL_INCLUDES +=  plf/refip/src/arch
GLOBAL_INCLUDES +=  plf/refip/src/arch/ll
GLOBAL_INCLUDES +=  plf/refip/import/reg
GLOBAL_INCLUDES +=  plf/rwip/api
GLOBAL_INCLUDES +=  plf/rwip/import/reg
GLOBAL_INCLUDES +=  plf/plf
GLOBAL_INCLUDES +=  arch
GLOBAL_INCLUDES +=  arch/app
GLOBAL_INCLUDES +=  arch/config
GLOBAL_INCLUDES +=  func
GLOBAL_INCLUDES +=  func/include
GLOBAL_INCLUDES +=  src/system
GLOBAL_INCLUDES +=  plf/refip/src/arch/compiler
GLOBAL_INCLUDES +=  driver/include
GLOBAL_INCLUDES +=  modules/common/api
GLOBAL_INCLUDES +=  modules/common
GLOBAL_INCLUDES +=  ip/ble/ll/api
GLOBAL_INCLUDES +=  ip/ble/ll/import/reg
GLOBAL_INCLUDES +=  ip/sch/import/
GLOBAL_INCLUDES += ip/em/api
### include ###
$(NAME)_INCLUDES := driver/icu
$(NAME)_INCLUDES += driver/uart
$(NAME)_INCLUDES += driver/sys_ctrl
$(NAME)_INCLUDES += driver/gpio
$(NAME)_INCLUDES += driver/intc
$(NAME)_INCLUDES += driver/pwm
$(NAME)_INCLUDES += driver/common
$(NAME)_INCLUDES += driver/rf
$(NAME)_INCLUDES += driver/wdt
$(NAME)_INCLUDES += driver/aon_wdt
$(NAME)_INCLUDES += driver/dma
$(NAME)_INCLUDES += driver/reg
$(NAME)_INCLUDES += driver/aon_rtc

$(NAME)_INCLUDES += modules/ke/api
$(NAME)_INCLUDES += ip/ble/ll/src/llc
$(NAME)_INCLUDES += ip/ble/ll/src/lld
$(NAME)_INCLUDES += ip/ble/ll/src/llm
$(NAME)_INCLUDES += ip/ble/ll/src
$(NAME)_INCLUDES += ip/ble/ll/api
$(NAME)_INCLUDES += ip/hci/api
$(NAME)_INCLUDES += ip/em/api
$(NAME)_INCLUDES += modules/ke/src
$(NAME)_INCLUDES += ip/platform/driver/rf
$(NAME)_INCLUDES += ip/platform/driver/reg
$(NAME)_INCLUDES += ip/platform/driver/efuse


### source ###
$(NAME)_SOURCES	 := system/boot_vectors.S
$(NAME)_SOURCES	 += system/boot_handlers.S
$(NAME)_SOURCES	 += plf/refip/src/arch/ll/ll.S
$(NAME)_SOURCES	 += arch/arch_main.c
$(NAME)_SOURCES  += arch/mem_arch.c
$(NAME)_SOURCES  += arch/str_arch.c
$(NAME)_SOURCES  += arch/app/app.c
$(NAME)_SOURCES  += arch/app/app_task.c

$(NAME)_SOURCES  += func/func.c
$(NAME)_SOURCES  += func/misc/fake_clock.c
$(NAME)_SOURCES  += func/power_save/mcu_ps.c
$(NAME)_SOURCES  += func/power_save/idle_mode.c

$(NAME)_SOURCES  += driver/driver.c
$(NAME)_SOURCES  += driver/common/dd.c
$(NAME)_SOURCES  += driver/common/drv_model.c
$(NAME)_SOURCES  += driver/icu/icu.c
$(NAME)_SOURCES  += driver/intc/intc.c
$(NAME)_SOURCES  += driver/flash/flash.c
$(NAME)_SOURCES  += driver/efuse/efuse.c
$(NAME)_SOURCES  += driver/gpio/gpio.c
$(NAME)_SOURCES  += driver/uart/uart.c
$(NAME)_SOURCES  += driver/wdt/wdt.c
$(NAME)_SOURCES  += driver/aon_wdt/aon_wdt.c
$(NAME)_SOURCES  += driver/pwm/pwm.c

ifeq ($(hw_timer),1)
$(NAME)_INCLUDES += driver/timer
$(NAME)_SOURCES  += driver/timer/timer.c
endif

ifeq ($(spi_driver),1)
$(NAME)_INCLUDES += driver/spi
$(NAME)_SOURCES  += driver/spi/spi.c
endif

ifeq ($(deep_sleep),1)
GLOBAL_INCLUDES += driver/deep_sleep
$(NAME)_SOURCES  += driver/deep_sleep/deep_sleep.c
endif

$(NAME)_SOURCES  += driver/
$(NAME)_SOURCES  += ip/platform/driver/rf/rf_xvr.c

$(NAME)_SOURCES  += modules/common/src/co_list.c
$(NAME)_SOURCES  += modules/common/src/co_utils.c

