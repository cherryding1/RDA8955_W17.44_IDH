## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/voice_express




LOCAL_MODULE_DEPENDS :=                                 \
                     ${PLATFORM_SYSTEM}/svc/voice_express/DTMF     \
                     ${PLATFORM_SYSTEM}/svc/voice_express/magic_sound \
                      ${PLATFORM_SYSTEM}/svc/voice_express/DTMFGEN \
                      
# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG +=
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
