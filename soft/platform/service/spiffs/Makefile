## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := $(PLATFORM_SERVICE)/spiffs

# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv

LOCAL_API_DEPENDS := \
	platform \
	platform/service \
	platform/base/sx \
	platform/chip/hal \
	platform/chip/defs

IS_TOP_LEVEL := yes
LOCAL_MODULE_DEPENDS := $(LOCAL_NAME)/spiffs_core

C_SRC := $(notdir $(wildcard src/*.c))

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
