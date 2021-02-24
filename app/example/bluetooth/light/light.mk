NAME := light

$(NAME)_COMPONENTS  += genie_app bluetooth.bt_host bluetooth.bt_mesh yloop cli

$(NAME)_INCLUDES += ../ \
					../../../../genie_app \
					../../../../genie_app/base \
					../../../../genie_app/bluetooth/host \
					../../../../genie_app/bluetooth/mesh \
					../../../../genie_app/bluetooth/mesh/mesh_model

$(NAME)_SOURCES  := light.c	\
					light_board.c \
					uart_cmd_handler.c

#ble = 1
#bt_mesh = 1
en_bt_smp = 0

bt_host_tinycrypt = 1

# Host configurations
GLOBAL_DEFINES += CONFIG_BLUETOOTH
GLOBAL_DEFINES += CONFIG_BT_CONN
GLOBAL_DEFINES += CONFIG_BT_PERIPHERAL
#GLOBAL_DEFINES += CONFIG_BLE_50
#GLOBAL_DEFINES += CONFIG_BT_HCI_VS_EXT

# Mesh function select
GLOBAL_DEFINES += CONFIG_BT_MESH
GLOBAL_DEFINES += CONFIG_BT_MESH_PROV
GLOBAL_DEFINES += CONFIG_BT_MESH_PB_ADV
GLOBAL_DEFINES += CONFIG_BT_MESH_PB_GATT
GLOBAL_DEFINES += CONFIG_BT_MESH_PROXY
GLOBAL_DEFINES += CONFIG_BT_MESH_GATT_PROXY
GLOBAL_DEFINES += CONFIG_BT_MESH_RELAY
#GLOBAL_DEFINES += CONFIG_BT_MESH_FRIEND
#GLOBAL_DEFINES += CONFIG_BT_MESH_LOW_POWER
#GLOBAL_DEFINES += CONFIG_BT_MESH_SHELL
GLOBAL_DEFINES += CONFIG_BT_MESH_ALI_TMALL_GENIE
#GLOBAL_DEFINES += CONFIG_BT_MESH_TELINK
#GLOBAL_DEFINES += CONFIG_BT_MESH_JINGXUN

# Mesh foundation model select
GLOBAL_DEFINES += CONFIG_BT_MESH_CFG_SRV
GLOBAL_DEFINES += CONFIG_BT_MESH_HEALTH_SRV
GLOBAL_DEFINES += MESH_MODEL_HSL_SRV
# Mesh debug message enable
#GLOBAL_DEFINES += USE_BT_MESH_CUSTOM_LOG
#GLOBAL_DEFINES += GENIE_DEBUG_COLOR
#GLOBAL_DEFINES += MESH_DEBUG_RX
#GLOBAL_DEFINES += MESH_DEBUG_TX
#GLOBAL_DEFINES += MESH_DEBUG_PROV
#GLOBAL_DEFINES += CONFIG_INIT_STACKS
#GLOBAL_DEFINES += CONFIG_PRINTK
GLOBAL_DEFINES += CONFIG_INFO_DISABLE

GLOBAL_INCLUDES += ../

MESH_MODEL_GEN_ONOFF_SRV = 1
MESH_MODEL_LIGHTNESS_SRV = 1
MESH_MODEL_GEN_ONPOWERUP_SRV = 1
MESH_MODEL_CTL_SRV = 1
MESH_MODEL_VENDOR_SRV = 1
MESH_MODEL_HSL_SRV = 1
BLE_MESH_TIME_SCENE_SERVER = 1
GLOBAL_DEFINES += CONFIG_BT_DEVICE_NAME=\"Mesh_JX_light\"

GLOBAL_DEFINES += PROJECT_SW_VERSION=0x00010104
#GLOBAL_DEFINES += PROJECT_SECRET_KEY=\"00112233445566770011223344556677\"

# Feature configurations
#GLOBAL_DEFINES += GENIE_OLD_AUTH
#GLOBAL_DEFINES += CONIFG_OLD_FLASH_PARA
#GLOBAL_DEFINES += GENIE_ULTRA_PROV

genie_ota = 1
#genie_vendor_timer = 1

#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_LIGHT
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_FLASH
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_EVENT
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_OTA

uart_test_cmd = 1

ifeq ($(uart_test_cmd),1)
GLOBAL_DEFINES += CONFIG_UART_TEST_CMD
$(NAME)_SOURCES += 	uart_test_cmd.c
endif