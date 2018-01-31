## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/service/net/dlna/upnp

LOCAL_MODULE_DEPENDS :=     \
        platform/service/net/dlna/upnp/control   \
        platform/service/net/dlna/upnp/event   \
        platform/service/net/dlna/upnp/ssdp   \

         
LOCAL_API_DEPENDS := \
		platform \
    platform/rtos/sx \
    platform/chip/hal \
    platform/chip/${CT_ASIC}/hal \
    platform/chip/comn/hal \
    platform/chip/comn\
    platform/service \
    platform/service/net/dlna \
    platform/service/net/dlna/xml \
    platform/service/net/dlna/io \
    platform/service/net/dlna/util \
    platform/service/net/dlna/net \
    platform/service/net/dlna/soap \
    platform/service/net/dlna/http \
	platform/service/net/dlna/upnp/ssdp \
    platform/service/net/dlna/upnp/event \
	platform/service/net/dlna/upnp/control

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG +=

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
C_SRC := ${notdir ${wildcard src/*.c}}

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
