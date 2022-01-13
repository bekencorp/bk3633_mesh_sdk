NAME := bk3633

HOST_OPENOCD := bk3633

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk3633

$(NAME)_COMPONENTS := libc rhino hal rhino.pwrmgmt rhino.fs.kv network.bluetooth.bt_host
$(NAME)_COMPONENTS += platform.arch.arm.armv5
$(NAME)_COMPONENTS += platform.mcu.$(HOST_MCU_FAMILY).hal_init
$(NAME)_COMPONENTS += platform.mcu.$(HOST_MCU_FAMILY).aos.framework_runtime
$(NAME)_COMPONENTS += platform.mcu.$(HOST_MCU_FAMILY).aos.app_runtime

ifeq ($(ble), 1)
### BLE resource allocation
GLOBAL_DEFINES  += CONFIG_BT_CONTROLLER_NAME="BK3633"
GLOBAL_DEFINES  += CONFIG_BT_WORK_QUEUE_STACK_SIZE=512-256
#GLOBAL_DEFINES  += CONFIG_BT_MESH_ADV_BUF_COUNT=16
#GLOBAL_DEFINES  += CONFIG_BT_HCI_RX_STACK_SIZE=512-64
#GLOBAL_DEFINES  += CONFIG_BT_RX_STACK_SIZE=512-128
#GLOBAL_DEFINES  += CONFIG_BT_RX_BUF_COUNT=10+6
endif

GLOBAL_DEFINES  += CONFIG_ARM
GLOBAL_DEFINES  += CONFIG_NO_TCPIP
GLOBAL_DEFINES  += CFG_SUPPORT_ALIOS=1

#GLOBAL_DEFINES  += CONFIG_AOS_KV_MULTIPTN_MODE
#GLOBAL_DEFINES  += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES  += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES  += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES  += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES  += CONFIG_AOS_UOTA_BREAKPOINT
GLOBAL_DEFINES  += CONFIG_AOS_CLI_STACK_SIZE=2048
GLOBAL_DEFINES  += CONFIG_AOS_CLI_BOARD

GLOBAL_CFLAGS   += -mcpu=arm968e-s
GLOBAL_CFLAGS   += -march=armv5te
GLOBAL_CFLAGS   += -mthumb -mthumb-interwork
GLOBAL_CFLAGS   += -mlittle-endian
GLOBAL_CFLAGS   += -w

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_LDFLAGS  += -mcpu=arm968e-s
GLOBAL_LDFLAGS  += -march=armv5te
GLOBAL_LDFLAGS  += -mthumb -mthumb-interwork
GLOBAL_LDFLAGS  += -mlittle-endian
GLOBAL_LDFLAGS  += --specs=nosys.specs
GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += $(CLIB_LDFLAGS_NANO_FLOAT)

ifeq ($(CONFIG_FLASH_SIZE_8M),1)
GLOBAL_LDFLAGS  += -T $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bk3633_8M.ld
else
GLOBAL_LDFLAGS  += -T $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bk3633.ld
endif

PING_PONG_OTA := 0

GLOBAL_INCLUDES +=  beken/arch
GLOBAL_INCLUDES +=  beken/arch/ll
GLOBAL_INCLUDES +=  beken/arch/compiler
GLOBAL_INCLUDES +=  beken/arch/include
GLOBAL_INCLUDES +=  beken/arch/config
GLOBAL_INCLUDES +=  beken/arch/app
#GLOBAL_INCLUDES +=  beken/common
GLOBAL_INCLUDES +=  beken/func
GLOBAL_INCLUDES +=  beken/func/include
GLOBAL_INCLUDES +=  beken/driver/include
GLOBAL_INCLUDES +=  modules/common/api
GLOBAL_INCLUDES +=  modules/common

$(NAME)_INCLUDES +=  aos

$(NAME)_SOURCES  :=  aos/aos_main.c
$(NAME)_SOURCES  +=  aos/soc_impl.c
$(NAME)_SOURCES  +=  aos/trace_impl.c

$(NAME)_SOURCES  +=  hal/hci_driver.c
$(NAME)_SOURCES	 +=  hal/hal_icu.c
$(NAME)_SOURCES	 +=  hal/hal_aon_wdt.c
$(NAME)_SOURCES	 +=  hal/gpio.c
$(NAME)_SOURCES  +=  hal/wdg.c
$(NAME)_SOURCES  +=  hal/hw.c
$(NAME)_SOURCES  +=  hal/flash.c
$(NAME)_SOURCES  +=  hal/uart.c
$(NAME)_SOURCES  +=  hal/pwm.c
$(NAME)_SOURCES  +=  hal/beken_rhino.c
$(NAME)_SOURCES  +=  port/static_partition.c

ifeq ($(hw_timer),1)
$(NAME)_SOURCES	 +=  hal/hw_timer.c
GLOBAL_DEFINES += __HW_TIMER_DRIVER__
endif

ifeq ($(spi_driver),1)
$(NAME)_SOURCES  +=  hal/hal_spi.c
GLOBAL_DEFINES += __SPI_DRIVER__
endif

ifeq ($(ble), 1)
ifeq ($(beken_ota),1)
$(NAME)_SOURCES  +=  port/ota_port.c
$(NAME)_SOURCES  +=  port/ota_crc.c
endif
endif

ifeq ($(deep_sleep), 1)
GLOBAL_DEFINES += __DEEP_SLEEP__
endif

ifneq ($(filter rhino.pwrmgmt,$(COMPONENTS)),)
$(NAME)_SOURCES  +=  hal/pwrmgmt_hal/board_cpu_pwr.c
$(NAME)_SOURCES  +=  hal/pwrmgmt_hal/board_cpu_pwr_systick.c
$(NAME)_SOURCES  +=  hal/pwrmgmt_hal/board_cpu_pwr_timer.c
endif

$(NAME)_COMPONENTS += platform.mcu.$(HOST_MCU_FAMILY).beken

#GLOBAL_LDFLAGS += -Wl,--wrap=boot_undefined
#GLOBAL_LDFLAGS += -Wl,--wrap=boot_pabort
#GLOBAL_LDFLAGS += -Wl,--wrap=boot_dabort

GLOBAL_DEFINES += BLE_4_2 CONFIG_ARM
