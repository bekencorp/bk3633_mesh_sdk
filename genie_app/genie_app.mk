NAME := genie_app

ble = 1

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Genie mesh application.

GLOBAL_CFLAGS += -DSYSINFO_BUILD_TIME=\"$(CURRENT_TIME)\"

$(NAME)_COMPONENTS += bluetooth.bt_mesh yloop cli bluetooth.bt_host crc bluetooth.bt_dfu

$(NAME)_INCLUDES += ../network/bluetooth/bt_mesh/inc \
					../network/bluetooth/bt_mesh/inc/api \
					../network/bluetooth/bt_mesh/mesh_model \
					../network/bluetooth/bt_mesh/vendor_model \
					../genie_app \
					../utility/crc \
					../network/bluetooth/bt_dfu/inc

$(NAME)_SOURCES  += genie_app.c \
					genie_mesh.c \
					base/genie_cmds.c \
					base/genie_reset.c \
					base/genie_flash.c \
					base/tri_tuple.c

GLOBAL_DEFINES += CONFIG_BT_MESH
GLOBAL_DEFINES += CONFIG_GENIE_CLI
GLOBAL_DEFINES += APP_SDK_VERSION=107
#GLOBAL_DEFINES += CONFIG_GENIE_RESET_BY_REPEAT
#GLOBAL_DEFINES += CONFIG_GENIE_RESET_BY_PRESS


