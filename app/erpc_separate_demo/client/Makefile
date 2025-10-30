APPLICATION = erpc_calculator_client

BOARD ?= native

# Path to RIOT base directory
RIOTBASE ?= $(CURDIR)/../../../RIOT

# This has to be the absolute path to the RIOT base directory:
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../modules

# Add eRPC module
USEMODULE += erpc

# Enable C++ support
FEATURES_REQUIRED += cpp

# Add needed C++ flags
CXXEXFLAGS += -std=c++11
# Add UART driver and transport
USEMODULE += periph_uart

# Add transport source
SRCS += riot_uart_transport.cpp

# Add generated client sources (from top-level generated files)
SRCS += calculator_client.cpp
SRCS += calculator_interface.cpp
SRCS += ../c_calculator_client.cpp

# Add path to shared files
INCLUDES += -I$(CURDIR)/..

# Ensure C++ source files are compiled
SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include