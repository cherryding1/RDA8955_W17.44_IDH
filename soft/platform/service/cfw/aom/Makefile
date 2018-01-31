## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/cfw/aom

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
				platform \
				platform/chip/hal \
				platform/base/common \
				platform/chip/defs \
				${PLATFORM_SYSTEM}/stack \
				platform/base/sx \
				${PLATFORM_SERVICE} \
				${PLATFORM_SERVICE}/cfw \
				platform/chip/pal \
				platform/phy \
				platform/system/svc/uctls \
				platform/system/svc/urndis/data

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \

# This is a top-level module
IS_TOP_LEVEL := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += \
		HAVE_CFW_CONFIG \
		MF32_BASE_REG_DISABLE \
		MF32_NO_RFM_RH \
		MF32_NO_RFM \
		MF32_CFW_MMI_TWO_TASKS

ifeq "${CT_RELEASE}" "cool_profile"
	LOCAL_EXPORT_FLAG += CSW_PROFILING
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}}


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
