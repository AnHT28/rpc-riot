APPLICATION = erpc_uart_multiply_device
BOARD ?= nrf52840dk
RIOTBASE ?= $(HOME)/RIOT

EXTERNAL_MODULE_DIRS += /home/an/rpc-riot/modules

USEMODULE += erpc
USEMODULE += xtimer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += cpp

# Disable STDIO completely to avoid interference with eRPC UART traffic
DISABLE_MODULE += stdio_available
DISABLE_MODULE += auto_init_debug
DISABLE_MODULE += printf_float
DISABLE_MODULE += puts
USEMODULE += stdio_null

INCLUDES += -I$(CURDIR)/../shared
CXXEXFLAGS += -std=c++11

SRCXX += main.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_client.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_interface.cpp
SRCXX += $(CURDIR)/../shared/riot_uart_transport.cpp
SRCXX += $(CURDIR)/../shared/riot_client_shimp.cpp

SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include
