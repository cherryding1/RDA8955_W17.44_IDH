## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/service/net/edp

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
			platform \
			platform/chip/hal \
		    platform/chip/hal/${CT_ASIC} \
			platform/chip/boot \
			platform/chip/defs \
			platform/service \
			platform/service/base \
			platform/edrv/pmd \
			${API_PLATFORM_DEPENDS} \
			/target

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := platform/service/net/edp/test

# This is a top-level module
IS_TOP_LEVEL := yes
# Module lib copy to ./lib/${ASIC}
IS_MODULE_LIB := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
#C_SRC := ${notdir ${wildcard src/*.c}}
include src/make.srcs


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
