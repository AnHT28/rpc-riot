APPLICATION = erpc_calculator_server

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

# Force sources to only our clean main + transport (avoid compiling broken main.cpp if present)
SRCS := main_clean.cpp
# Add pthreads threading implementation for native
SRCS += erpc_threading_pthreads.cpp

# Add generated shim sources so the service vtables/ctors are linked
SRCS += ../calculator_server.cpp ../calculator_interface.cpp

# Add path to shared files
INCLUDES += -I$(CURDIR)/..
# Add eRPC setup and transports include paths
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/setup
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/transports
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/port
# Ensure eRPC uses dynamic allocation policy for host builds
CXXEXFLAGS += -DERPC_ALLOCATION_POLICY=ERPC_ALLOCATION_POLICY_DYNAMIC

# Add path to shared files
#INCLUDES += -I$(CURDIR)/..

# Ensure C++ source files are compiled
SRCXXEXT = cpp

include $(RIOTBASE)/Makefile.include