## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/base

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/chip/hal \
	platform/base/common \
	platform/chip/defs \
	${PLATFORM_SYSTEM}/stack \
	${PLATFORM_SERVICE}

# list all the modules that need to be compiled prior to using this module

LOCAL_MODULE_DEPENDS := \
	${PLATFORM_SERVICE}/base/bal \
	${PLATFORM_SERVICE}/base/cos \
	${PLATFORM_SERVICE}/base/db \
	${PLATFORM_SERVICE}/base/ffs \
	${PLATFORM_SERVICE}/base/ml \
	${PLATFORM_SERVICE}/base/nvram \
	${PLATFORM_SERVICE}/base/sul \
	${PLATFORM_SERVICE}/base/tm \
	${PLATFORM_SERVICE}/base/vds \
	${PLATFORM_SERVICE}/base/ver \
	${PLATFORM_SERVICE}/base/remain \
	${PLATFORM_SERVICE}/base/psm \
	${PLATFORM_SERVICE}/base/factory
	

#
ifeq "${DLL_SUPPORT}" "yes"
LOCAL_MODULE_DEPENDS +=${PLATFORM_SERVICE}/base/dll 
endif

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
