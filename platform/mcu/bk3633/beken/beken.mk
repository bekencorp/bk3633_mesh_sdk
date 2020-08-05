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
$(NAME)_INCLUDES += driver/dma
$(NAME)_INCLUDES += driver/reg
$(NAME)_INCLUDES += driver/aon_rtc

$(NAME)_INCLUDES += modules/ke/api
#$(NAME)_INCLUDES += ble/ble_stack_inc/ke/src
#$(NAME)_INCLUDES += ble/ble_stack_inc/ble/ll/src/em
$(NAME)_INCLUDES += ip/ble/ll/src/llc
$(NAME)_INCLUDES += ip/ble/ll/src/lld
$(NAME)_INCLUDES += ip/ble/ll/src/llm
$(NAME)_INCLUDES += ip/ble/ll/src
#$(NAME)_INCLUDES += ip/ble/ll/api
$(NAME)_INCLUDES += ip/hci/api
$(NAME)_INCLUDES += ip/em/api
$(NAME)_INCLUDES += ip/ble/ll/import/reg
$(NAME)_INCLUDES += ip/sch/import
#$(NAME)_INCLUDES += ble/ble_stack_inc/hci/src
#$(NAME)_INCLUDES += ble/ble_stack_inc/nvds/api
#$(NAME)_INCLUDES += ble/ble_stack_inc/mesh/api
#$(NAME)_INCLUDES += ble/ble_stack_inc/mesh/m_al/lld

$(NAME)_INCLUDES += modules/ke/src
$(NAME)_INCLUDES += ip/platform/driver/rf
$(NAME)_INCLUDES += ip/platform/driver/reg

#$(NAME)_INCLUDES += ble/ble_stack_com/rwble
#$(NAME)_INCLUDES += ble/ble_stack_com/rwip/api
#$(NAME)_INCLUDES += ble/ble_stack_com/core_modules/rf/api
#$(NAME)_INCLUDES += ble/ble_stack_com/core_modules/dbg/api

#$(NAME)_INCLUDES += ble/plactform/src/core_modules/rf/api
#$(NAME)_INCLUDES += ble/plactform/src/core_modules/dbg/api
#$(NAME)_INCLUDES += ble/plactform/src/driver/icu
#$(NAME)_INCLUDES += ble/plactform/src/driver/wdt
#$(NAME)_INCLUDES += ble/plactform/src/driver/emi

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

$(NAME)_SOURCES  += driver/driver.c
$(NAME)_SOURCES  += driver/common/dd.c
$(NAME)_SOURCES  += driver/common/drv_model.c
$(NAME)_SOURCES  += driver/icu/icu.c
$(NAME)_SOURCES  += driver/intc/intc.c
$(NAME)_SOURCES  += driver/flash/flash.c
$(NAME)_SOURCES  += driver/gpio/gpio.c
#$(NAME)_SOURCES  += driver/i2s/i2s.c
#$(NAME)_SOURCES  += driver/pwm/pwm.c
$(NAME)_SOURCES  += driver/spi/spi.c
#$(NAME)_SOURCES  += driver/uart/Retarget.c
$(NAME)_SOURCES  += driver/uart/uart.c
$(NAME)_SOURCES  += driver/wdt/wdt.c
$(NAME)_SOURCES  += driver/pwm/pwm.c

#$(NAME)_SOURCES  += func/func.c
#$(NAME)_SOURCES  += func/debug_uart.c
#$(NAME)_SOURCES  += func/misc/fake_clock.c
#$(NAME)_SOURCES  += func/power_save/mcu_ps.c

$(NAME)_SOURCES  += modules/common/src/co_list.c
$(NAME)_SOURCES  += modules/common/src/co_utils.c
#$(NAME)_SOURCES  += common/core_modules/src/RomCallFlash.c

#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke_event.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke_msg.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke_queue.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke_task.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ke/src/ke_timer.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ea/src/ea.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/em/em_buf.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc.c
##$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc_ch_asses.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc_hci.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc_llcp.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc_task.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llc/llc_util.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld_evt.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld_pdu.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld_sleep.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld_util.c
##$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/lld/lld_wlcoex.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llm/llm.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llm/llm_hci.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llm/llm_task.c
#$(NAME)_SOURCES  += ble/ble_stack_src/ble/ll/src/llm/llm_util.c
#$(NAME)_SOURCES  += ble/ble_stack_src/hci/src/hci.c
#$(NAME)_SOURCES  += ble/ble_stack_src/hci/src/hci_fc.c
#$(NAME)_SOURCES  += ble/ble_stack_src/hci/src/hci_msg.c
#$(NAME)_SOURCES  += ble/ble_stack_src/hci/src/hci_tl.c
#$(NAME)_SOURCES  += ble/ble_stack_src/hci/src/hci_util.c
##$(NAME)_SOURCES  += ble/ble_stack_src/nvds/src/nvds.c

#$(NAME)_SOURCES  += ble/ble_stack_com/rwip/src/rwip.c
#$(NAME)_SOURCES  += ble/ble_stack_com/rwble/rwble.c
#$(NAME)_SOURCES  += ble/ble_stack_com/core_modules/rf/src/rf_xvr.c
#$(NAME)_SOURCES  += ble/ble_stack_com/core_modules/dbg/src/dbg.c
#$(NAME)_SOURCES  += ble/ble_stack_com/core_modules/dbg/src/dbg_task.c

#$(NAME)_SOURCES  += ble/plactform/src/driver/wdt/wdt.c
#$(NAME)_SOURCES  += ble/plactform/src/driver/emi/emi.c
