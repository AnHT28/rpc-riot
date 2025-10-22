APPLICATION = erpc_multiply
BOARD ?= native
RIOTBASE ?= $(HOME)/RIOT
# or, if your tree is: <project>/../RIOT
# RIOTBASE ?= $(CURDIR)/../../RIOT

# Tell RIOT where your external modules live (adjust path if different)
EXTERNAL_MODULE_DIRS += /home/an/rpc-riot/modules

# Pull in our external eRPC module
USEMODULE += erpc
USEMODULE += xtimer

# We'll use UART later for a transport
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += cpp


# Make sure our local headers (erpc_config.h) are found first
CPPFLAGS += -I$(APPDIR)
CXXEXFLAGS += -std=c++11
include $(RIOTBASE)/Makefile.include
