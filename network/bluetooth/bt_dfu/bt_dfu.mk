NAME := bt_dfu

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh stack.

#$(NAME)_COMPONENTS += bluetooth.bt_mesh

$(NAME)_INCLUDES += ./inc/ \
                      ./core/gatt_ota/inc/ \
					../../kernel/rhino/ \
					./core/ \
					./inc/api/ \
					./inc/api/mesh/ \
					../../../genie_app/ \
					../bt_mesh/mesh_model/ \
					../bt_mesh/mesh_model/inc/ \
					../bt_mesh/vendor_model/ \
					../bt_mesh/inc/

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES := ./core/gatt_ota/src/ais_btmesh_service.c \
					./hal/ali_dfu.c \
					./hal/ali_dfu_port.c \
					./core/gatt_ota/src/ais_btmesh.c \
					./core/bt_dfu_plat.c \
					./core/bt_dfu_service.c \
					./core/bt_dfu_transport.c \

GLOBAL_INCLUDES += ./


GLOBAL_DEFINES += CONFIG_BT_DFU
GLOBAL_DEFINES += CONFIG_BT_MESH_GATT_OTA

