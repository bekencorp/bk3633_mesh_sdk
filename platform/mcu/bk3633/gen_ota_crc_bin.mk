ifeq ($(HOST_OS),Win32)
ENCRYPT := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/BinConvert.exe
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/BinConvert_3633_OAD_Linux
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/BinConvert_3633_OAD_Linux
else # Linux64
$(error not surport for $(HOST_OS))
endif # Linux64
endif # Linux32
endif # Win32

PARTITION_STACK_CPU_ADDR = 0xF20
PARTITION_APP_CPU_ADDR = 0x16A60
img_ver = 0x1111
rom_ver    = 0x0006
BIM_BIN_FILE   := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/bk3633_boot.bin
STACK_BIN_FILE := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/bk3633_controller.bin
APP_BIN_FILE   := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/bk3633_ble_app.bin
OAD_BIN_FILE   := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/bk3633_ble_app_oad.bin
MERGE_BIN_FILE := $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/bk3633_ble_app_merge_crc.bin
CRC_BIN_OUTPUT_FILE :=$(SOURCE_ROOT)$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE :=$(SOURCE_ROOT)$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += clean_file gen_ota_crc_bin copy_bin_to_out #copy_out_to_burn copy_ota_to_burn

clean_file:
	$(eval OUT_MSG := $(shell rm $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/*.out))
	$(eval OUT_MSG := $(shell rm $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/*_oad.bin))
	$(eval OUT_MSG := $(shell rm $(SOURCE_ROOT)platform/mcu/$(HOST_MCU_FAMILY)/bin/*_crc.bin))

gen_ota_crc_bin:
	$(ECHO) 4M bits flash
	$(eval OUT_MSG := $(shell cp $(BIN_OUTPUT_FILE) $(APP_BIN_FILE)))
	$(eval OUT_MSG := $(shell $(ENCRYPT) -oad $(BIM_BIN_FILE) $(STACK_BIN_FILE) $(APP_BIN_FILE) -m $(PARTITION_STACK_CPU_ADDR) -l $(PARTITION_APP_CPU_ADDR) -v $(img_ver) -rom_v $(rom_ver) -e  00000000 00000000 00000000 00000000))

copy_bin_to_out:
	$(eval OUT_MSG := $(shell cp $(MERGE_BIN_FILE) $(CRC_BIN_OUTPUT_FILE)))
	$(eval OUT_MSG := $(shell cp $(OAD_BIN_FILE) $(OTA_BIN_OUTPUT_FILE)))

copy_out_to_burn:
	$(eval OUT_MSG := $(shell cp $(CRC_BIN_OUTPUT_FILE) r:/))

copy_ota_to_burn:
	$(eval OUT_MSG := $(shell cp $(OTA_BIN_OUTPUT_FILE) r:/))
