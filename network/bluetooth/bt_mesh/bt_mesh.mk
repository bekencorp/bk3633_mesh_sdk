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
                    ../../../genie_app/ \
                    mesh_model/ \
                    mesh_model/inc/ \
                    vendor_model/

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
                     ./src/provisioner_main.c \
					 ./src/provisioner_prov.c \
					 ./src/provisioner_beacon.c \
					 ./src/provisioner_proxy.c   \


GLOBAL_INCLUDES += ./inc/ \
                   ./inc/api

#check config
ifeq ($(MESH_MODEL_CTL_SRV),1)
MESH_MODEL_GEN_ONOFF_SRV = 1
MESH_MODEL_LIGHTNESS_SRV = 1
MESH_MODEL_VENDOR_SRV = 1
MESH_MODEL_CTL_SRV = 1
ifneq ($(ALI_SIMPLE_MODLE),1)
MESH_MODEL_GEN_LEVEL_SRV = 1
endif
endif

ifeq ($(MESH_MODEL_LIGHTNESS_SRV),1)
MESH_MODEL_GEN_ONOFF_SRV = 1
ifneq ($(ALI_SIMPLE_MODLE),1)
MESH_MODEL_GEN_LEVEL_SRV = 1
endif
endif

ifneq ($(MESH_MODEL_DIABLE_TRANS),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_TRANS
endif

ifeq ($(MESH_MODEL_GEN_ONOFF_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_GEN_ONOFF_SRV
$(NAME)_SOURCES += mesh_model/src/gen_onoff_srv.c
endif

ifeq ($(MESH_MODEL_GEN_LEVEL_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_GEN_LEVEL_SRV
$(NAME)_SOURCES += mesh_model/src/gen_level_srv.c
endif

ifeq ($(MESH_MODEL_LIGHTNESS_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_LIGHTNESS_SRV
GLOBAL_DEFINES += CONFIG_MESH_MODEL_LIGHT_CMD
$(NAME)_SOURCES += mesh_model/src/lightness_srv.c
endif

ifeq ($(MESH_MODEL_CTL_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_SRV
GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_SETUP_SRV
#GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_TEMPERATURE_SRV
$(NAME)_SOURCES += mesh_model/src/light_ctl_srv.c
endif

ifeq ($(MESH_MODEL_HSL_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_HSL_SRV=1
GLOBAL_DEFINES += CONFIG_MESH_MODEL_HSL_SETUP_SRV=1
GLOBAL_DEFINES += CONFIG_MESH_MODEL_HSL_HUE_SRV=1
GLOBAL_DEFINES += CONFIG_MESH_MODEL_HSL_SAT_SRV=1
$(NAME)_SOURCES += mesh_model/src/light_hsl_srv.c
endif

ifeq ($(MESH_MODEL_VENDOR_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_VENDOR_SRV
$(NAME)_SOURCES += mesh_model/src/vendor_model_srv.c
endif

ifeq ($(ALI_SIMPLE_MODLE),1)
GLOBAL_DEFINES += CONFIG_ALI_SIMPLE_MODLE
endif

$(NAME)_SOURCES += mesh_model/src/model_bind_ops.c

#GLOBAL_DEFINES += CONFIG_BT_MESH_MULTIADV

GLOBAL_DEFINES += CRC16_ENABLED

## MESH debug log control macro
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ADV
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_BEACON
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROXY
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROV
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_NET
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_CRYPTO
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_TRANS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_FRIEND
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_LOW_POWER
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ACCESS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_FLASH
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_MODEL
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_VENDOR_MODEL
GLOBAL_DEFINES += CONFIG_GENIE_DEBUG_CMD_FLASH

