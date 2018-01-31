## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/service/net/mqttsn/test

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS :=

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS :=  platform \
          platform/service/net   \
          platform/service/net/mqttsn \
          platform/service/net/lwip \
          platform/service/net/lwip/src \
          platform/service \
          platform/chip/defs \
          platform/base/sx \
          platform/chip/hal


LOCAL_EXPORT_FLAG +=

## ------------------------------------- ##
##      List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
# uncomment to take all test .c files
C_SRC := ${notdir ${wildcard src/*.c}} \


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


