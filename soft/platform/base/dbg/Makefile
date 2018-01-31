## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/base/dbg

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := 	platform \
						platform/chip/hal \
						platform/chip/defs \
						platform/base/sx \

ifeq (${CT_ASIC},jade)
LOCAL_API_DEPENDS += platform/phy
else
LOCAL_API_DEPENDS += platform/chip/hal
endif

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

# Generate the CoolProfile header automatically.
AUTO_XCP2H ?= no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

