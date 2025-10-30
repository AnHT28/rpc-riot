MODULE := erpc_tcp_transport

# eRPC directory
ERPC_DIR := $(RIOTBASE)/../modules/erpc/erpc

# TCP Transport module requires:
# - eRPC core files 
# - TCP/IP stack
FEATURES_REQUIRED += cpp
FEATURES_REQUIRED_ANY += native_sockets

# Make OUR config win for every translation unit
CFLAGS   += -I$(ERPC_DIR)/erpc_c/transports -I$(ERPC_DIR)/erpc_c/setup
CXXFLAGS += -I$(ERPC_DIR)/erpc_c/transports -I$(ERPC_DIR)/erpc_c/setup

# Sources for TCP transport
SRCXX += $(ERPC_DIR)/erpc_c/transports/erpc_tcp_transport.cpp
SRCXX += $(ERPC_DIR)/erpc_c/setup/erpc_setup_tcp.cpp
SRCXX += $(ERPC_DIR)/erpc_c/port/erpc_threading_pthreads.cpp

include $(RIOTBASE)/Makefile.base