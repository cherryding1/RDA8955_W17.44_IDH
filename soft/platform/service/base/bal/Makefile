## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/base/bal

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := ${PLATFORM_SERVICE}/tcpip\
			platform \
			platform/base/common \
			platform/base/sx \
			platform/chip/hal \
			platform/chip/boot \
			platform/chip/defs \
			platform/chip/regs/$(CT_ASIC) \
			${PLATFORM_SERVICE} \
			${PLATFORM_SERVICE}/base \
			${PLATFORM_SERVICE}/dm \
			${PLATFORM_SERVICE}/spiffs \
			${PLATFORM_SERVICE}/cfw \
			platform/edrv/aud \
			platform/edrv/pmd \
			${PLATFORM_SYSTEM}/stack \
			${PLATFORM_SYSTEM}/svc/fstraces \
			${PLATFORM_SYSTEM}/svc/ars \
			${PLATFORM_SYSTEM}/svc/vois \
			${PLATFORM_SERVICE}/spiffs \
			target

ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_API_DEPENDS += platform/edrv/wifi/rdawifi
endif
# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \

# This is a top-level module
IS_TOP_LEVEL := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}}


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
