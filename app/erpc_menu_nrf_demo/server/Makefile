APPLICATION = erpc_menu_nrf_server
BOARD ?= nrf52840dk
RIOTBASE ?= $(CURDIR)/../../../RIOT
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../modules

USEMODULE += erpc
USEMODULE += periph_uart
USEMODULE += tsrb
USEMODULE += sema
USEMODULE += xtimer

FEATURES_REQUIRED += cpp
CXXEXFLAGS += -std=c++11
SRCXXEXT = cpp

# Add shared module
EXTERNAL_MODULE_DIRS += $(CURDIR)/..
USEMODULE += erpc_menu_nrf_shared

INCLUDES += -I$(CURDIR)/../erpc_menu_nrf_shared
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/setup
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/transports
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/port

SRCXX += main.cpp

CXXEXFLAGS += -DERPC_ALLOCATION_POLICY=ERPC_ALLOCATION_POLICY_DYNAMIC

# Explicitly configure STDIO to use UART_DEV(0) (USB console)
CFLAGS += -DSTDIO_UART_DEV=UART_DEV\(0\)

include $(RIOTBASE)/Makefile.include
