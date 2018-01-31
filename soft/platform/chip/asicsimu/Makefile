## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip/asicsimu

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform platform/base/common \
					 platform/chip/regs/${CT_ASIC} \
					 platform/chip/defs \
					 platform/chip/bcpu \
					 platform/chip/boot \
					 platform/chip/hal \
                     

# This is a top-level module
IS_TOP_LEVEL = no

## ------------------------------------- ##
##      List all your sources here       ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
