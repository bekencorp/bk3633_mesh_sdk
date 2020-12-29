NAME := hal

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES     := wifi.c
$(NAME)_SOURCES     += hal_ota.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES      += AOS_HAL
GLOBAL_INCLUDES     += include ../rhino ../rhino/hal/soc ./include/hal/
						
