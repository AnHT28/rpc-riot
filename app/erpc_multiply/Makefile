APPLICATION = erpc_multiply
BOARD ?= native
RIOTBASE ?= $(HOME)/RIOT
# or, if your tree is: <project>/../RIOT
# RIOTBASE ?= $(CURDIR)/../../RIOT

# Tell RIOT where your external modules live (adjust path if different)
EXTERNAL_MODULE_DIRS += /home/an/rpc-riot/modules

# Pull in our external eRPC module
USEMODULE += erpc

# We'll use UART later for a transport
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += cpp

# We have C++ sources in this app
CXXEXFLAGS += -std=c++11

# Make sure our local headers (erpc_config.h) are found first
CPPFLAGS += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include
