APPLICATION = erpc_uart_multiply_host
BOARD ?= native
RIOTBASE ?= $(HOME)/RIOT

EXTERNAL_MODULE_DIRS += /home/an/rpc-riot/modules

USEMODULE += erpc
USEMODULE += xtimer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += cpp

INCLUDES += -I$(CURDIR)/../shared
CXXEXFLAGS += -std=c++11

SRCXX += main.cpp
SRCXX += $(CURDIR)/../shared/multiply_impl.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_interface.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_server.cpp
SRCXX += $(CURDIR)/../shared/multiply_demo_client.cpp
SRCXX += $(CURDIR)/../shared/c_multiply_demo_server.cpp
SRCXX += $(CURDIR)/../shared/riot_uart_transport.cpp

SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include
