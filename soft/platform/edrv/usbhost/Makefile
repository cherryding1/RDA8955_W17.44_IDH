## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/usbhost

ifneq (${USBHOST_IF},)
LOCAL_MODULE_DEPENDS := platform/edrv/usbhost/usbhost_${USBHOST_IF}
endif # (${USBHOST_IF},)

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform platform/chip platform/sx \
                     platform/chip/hal ${PLATFORM_SYSTEM}/svc/uctls/include target 

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the usbhost modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/usbhost/usbhost_${USBHOST_IF} \

endif #RELEASE_MODULE

## ------------------------------------ ##
##  Add your custom flags here          ##
## ------------------------------------ ##

#for doc to include each interface's target structure
EXTRA_DOC_INPUTS := \
                    platform/edrv/usbhost/usbhost_null \
                    platform/edrv/usbhost/usbhost_${USBHOST_IF} \

AUTO_XCP2H ?= no

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##  List all your sources here           ##
## ------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC := 
INPUT_FILES := 
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
