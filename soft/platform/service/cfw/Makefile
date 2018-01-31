## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/cfw

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/chip/hal \
					platform/base/common \
					platform/chip/defs \
					${PLATFORM_SYSTEM}/stack \
					${PLATFORM_SERVICE} \
					${API_PLATFORM_DEPENDS} \

ifeq "${IOT_SUPPORT}" "1"
LOCAL_API_DEPENDS += at/iot
endif

# list all the modules that need to be compiled prior to using this module

LOCAL_MODULE_DEPENDS := \
						${PLATFORM_SERVICE}/cfw/cm \
						${PLATFORM_SERVICE}/cfw/sim \
						${PLATFORM_SERVICE}/cfw/aom \
						${PLATFORM_SERVICE}/cfw/shell \
						${PLATFORM_SERVICE}/cfw/bal \
						${PLATFORM_SERVICE}/cfw/sms \
						${PLATFORM_SERVICE}/cfw/cfg \
						${PLATFORM_SERVICE}/cfw/nw \
						${PLATFORM_SERVICE}/cfw/emod \

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += \
		HAVE_CFW_CONFIG \
		MF32_BASE_REG_DISABLE \
		MF32_NO_RFM_RH \
		MF32_NO_RFM \
		MF32_CFW_MMI_TWO_TASKS

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
