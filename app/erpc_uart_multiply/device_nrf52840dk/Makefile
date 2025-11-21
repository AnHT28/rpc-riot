APPLICATION = erpc_uart_multiply_device
BOARD ?= nrf52840dk
RIOTBASE ?= $(HOME)/RIOT

EXTERNAL_MODULE_DIRS += /home/an/rpc-riot/modules

USEMODULE += erpc
USEMODULE += xtimer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += cpp

# Enable STDIO for user interaction on UART_DEV(0) - /dev/ttyACM0
# eRPC will use UART_DEV(1) - /dev/ttyACM1

INCLUDES += -I$(CURDIR)/../shared
CXXEXFLAGS += -std=c++11

SRCXX += main.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_client.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_interface.cpp
SRCXX += $(CURDIR)/../shared/riot_uart_transport.cpp
SRCXX += $(CURDIR)/../shared/riot_client_shimp.cpp

SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include
