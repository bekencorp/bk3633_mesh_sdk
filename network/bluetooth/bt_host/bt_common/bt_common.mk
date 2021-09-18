NAME := bt_common

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Common utilities used in BLE components.

$(NAME)_SOURCES :=   ./tinycrypt/source/cmac_mode.c \
                     ./tinycrypt/source/aes_encrypt.c \
                     ./tinycrypt/source/aes_decrypt.c \
                     ./tinycrypt/source/utils.c \
                     ./tinycrypt/source/sha256.c \
                     ./tinycrypt/source/hmac.c \
                     ./tinycrypt/source/hmac_prng.c \
                     ./tinycrypt/source/ecc.c \
                     ./tinycrypt/source/ecc_dh.c \
                     port/aos_port.c

$(NAME)_INCLUDES := include

GLOBAL_INCLUDES += include \
                     tinycrypt/include \
                     port/include \
                     ../../bt_mesh/inc
                     