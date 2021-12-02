NAME := board_bk3633

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board bk3633
MODULE             := BK3633
HOST_ARCH          := ARM968E-S
HOST_MCU_FAMILY    := bk3633
SUPPORT_MBINS      := no

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=1 BOARD_BK3633DEVKIT
#GLOBAL_DEFINES  += FLASH_SIZE_8M

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_BK3633
CONFIG_SYSINFO_DEVICE_NAME   := BK3633

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)/board/bk3633devkit

# Extra build target in aos_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk

ifeq (, $(findstring FLASH_SIZE_8M, $(GLOBAL_DEFINES)))
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_ota_crc_bin.mk
else
CONFIG_FLASH_SIZE_8M := 1
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_ota_crc_bin_8M.mk
endif
