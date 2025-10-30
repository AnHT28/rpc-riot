APPLICATION = erpc_calculator_client

BOARD ?= native

# Path to RIOT base directory
RIOTBASE ?= $(CURDIR)/../../../RIOT

# This has to be the absolute path to the RIOT base directory:
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../modules

# Add eRPC module
USEMODULE += erpc

# Enable TCP transport module
USEMODULE += erpc_tcp_transport

# Enable C++ support
FEATURES_REQUIRED += cpp

# Add needed C++ flags
CXXEXFLAGS += -std=c++11
# Add UART driver and transport
USEMODULE += periph_uart

# Add transport source
SRCS += riot_uart_transport.cpp
# TCP transport module is used instead of local files
# Add pthreads threading implementation for native
SRCS += erpc_threading_pthreads.cpp

# Add generated client sources (from top-level generated files)
SRCS += calculator_client.cpp
SRCS += calculator_interface.cpp
SRCS += ../c_calculator_client.cpp

# Add path to shared files
INCLUDES += -I$(CURDIR)/..
# Add eRPC setup and transports include paths
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/setup
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/transports
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/port
# Ensure eRPC uses dynamic allocation policy for host builds
CXXEXFLAGS += -DERPC_ALLOCATION_POLICY=ERPC_ALLOCATION_POLICY_DYNAMIC

# Ensure C++ source files are compiled
SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include