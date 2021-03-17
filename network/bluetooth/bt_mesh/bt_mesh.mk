NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh stack.

ifeq ($(bt_mesh_standalone_deploy),1)
GLOBAL_DEFINES += CONFIG_MESH_STACK_ALONE
$(NAME)_COMPONENTS += bluetooth.bt_mesh.util
else
$(NAME)_COMPONENTS += bluetooth.bt_host.bt_common
endif

$(NAME)_COMPONENTS += bluetooth.bt_mesh.ref_impl

$(NAME)_INCLUDES += ./inc/ \
                    ./inc/api/ \
                    ./inc/api/mesh/ \
                    ./inc/port/ \

$(NAME)_INCLUDES += ../../../network/bluetooth/bt_mesh/inc \
                    ../../../network/bluetooth/bt_mesh/inc/api \
					../../../network/bluetooth/bt_mesh/mesh_model \
					../../../network/bluetooth/bt_mesh/mesh_model/inc \
					../../../network/bluetooth/bt_mesh/vendor_model \
                    ../../../genie_app/ \
                    ../../../genie_app/bluetooth/mesh/mesh_model	\
					../../../genie_app/bluetooth/mesh/mesh_model/inc	\
					../../../genie_app/base \
					../../../genie_app/bluetooth/host \
					../../../genie_app/bluetooth/mesh \

                    
$(NAME)_SOURCES  :=  ./src/access.c \
                     ./src/adv.c \
                     ./src/beacon.c \
                     ./src/crypto.c \
                     ./src/cfg_srv.c \
                     ./src/cfg_cli.c \
                     ./src/health_srv.c \
                     ./src/health_cli.c \
                     ./src/main.c \
                     ./src/net.c \
                     ./src/prov.c \
                     ./src/proxy.c \
                     ./src/transport.c \
                     ./src/friend.c \
                     ./src/lpn.c \
                     ./src/shell.c \
                     ./src/provisioner_beacon.c \
                     ./src/provisioner_main.c \
                     ./src/provisioner_prov.c \
                     ./src/provisioner_proxy.c \


GLOBAL_INCLUDES += ./inc/ \
                   ./inc/api


GLOBAL_DEFINES += CRC16_ENABLED

## BLE Mesh subsystem debug log control macro
## Enable below macros if component-specific debug needed
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ACCESS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_TRANS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_NET
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROV
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROXY
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_FRIEND
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_LOW_POWER
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ADV
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_BEACON
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_MODEL
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_VENDOR_MODEL
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_CRYPTO
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROVISIONER_BEACON
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROVISIONER_MAIN
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROVISIONER_PROV
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROVISIONER_PROXY