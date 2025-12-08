APPLICATION = erpc_menu_demo_server

BOARD ?= native

# Path to RIOT base directory
RIOTBASE ?= $(CURDIR)/../../../RIOT

# This has to be the absolute path to the RIOT base directory:
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../modules

# eRPC C sources root
ERPC_C_DIR := $(CURDIR)/../../../modules/erpc/erpc/erpc_c

# Add eRPC module
USEMODULE += erpc
USEMODULE += xtimer

# Enable C++ support
FEATURES_REQUIRED += cpp

# Add needed C++ flags
CXXEXFLAGS += -std=c++11
# Suppress warnings from upstream pthread port
CXXEXFLAGS += -Wno-error=unused-parameter

# Ensure C++ source files are compiled
SRCXXEXT = cpp

# Main application and shared service sources
SRCXX += main.cpp \
	$(CURDIR)/../shared/demo_service_interface.cpp \
	$(CURDIR)/../shared/demo_service_server.cpp

# Pull in TCP transport/setup + pthread threading for native host
SRCXX += $(ERPC_C_DIR)/setup/erpc_setup_tcp.cpp \
	$(ERPC_C_DIR)/transports/erpc_tcp_transport.cpp \
	$(ERPC_C_DIR)/port/erpc_threading_pthreads.cpp

# Add UART driver and transport
USEMODULE += periph_uart

# Add path to shared files for headers (still useful for header includes)
INCLUDES += -I$(CURDIR)/../shared

# Add eRPC setup and transports include paths
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/setup
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/transports
INCLUDES += -I$(CURDIR)/../../../modules/erpc/erpc/erpc_c/port

# Ensure eRPC uses dynamic allocation policy for host builds
CXXEXFLAGS += -DERPC_ALLOCATION_POLICY=ERPC_ALLOCATION_POLICY_DYNAMIC

# Compile generated service shims
SRCXX += ../shared/demo_service_interface.cpp \
	../shared/demo_service_server.cpp

include $(RIOTBASE)/Makefile.include
