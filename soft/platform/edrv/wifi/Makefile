## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/wifi

ifneq (${WIFI_MODEL},)
LOCAL_MODULE_DEPENDS := platform/edrv/wifi/${WIFI_MODEL}
endif # (${WIFI_MODEL},)

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= \
                platform \
                platform/chip \
                platform/chip/hal \
                platform/sx \
                target

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the mcd modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/wifi/rdawifi \

endif #RELEASE_MODULE

## ------------------------------------ ##
##  Add your custom flags here          ##
## ------------------------------------ ##

#for doc to include each interface's target structure
EXTRA_DOC_INPUTS := \
                    platform/edrv/wifi/${WIFI_MODEL} \

AUTO_XCP2H ?= no

# 32K clock connection
ifeq "${WIFI_USE_32K_CLK_PIN}" "1"
LOCAL_EXPORT_FLAG += WIFI_USE_32K_CLK_PIN
endif

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
