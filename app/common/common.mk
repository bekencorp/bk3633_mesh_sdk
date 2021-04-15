NAME := common

$(NAME)_INCLUDES += ../

$(NAME)_SOURCES += 	common_module.c

ifeq ($(button),1)
GLOBAL_DEFINES += CONFIG_GPIO_BUTTONS
$(NAME)_SOURCES += 	button_control.c	\
					button_module.c
endif

ifeq ($(uart_test_cmd),1)
GLOBAL_DEFINES += CONFIG_UART_TEST_CMD
$(NAME)_SOURCES += 	uart_test_cmd.c
endif