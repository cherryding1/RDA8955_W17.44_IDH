## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/camd

ifneq (${CAMERA_MODEL},)
LOCAL_MODULE_DEPENDS := $(foreach cam,${CAMERA_MODEL},platform/edrv/camd/${cam})
endif # (${CAMERA_MODEL},)

ifneq (${ATV_MODEL},)
LOCAL_MODULE_DEPENDS += platform/edrv/camd/atv_interface
endif # (${ATV_MODEL},)

comma :=,
empty :=#
space := $(empty) $(empty)#

CAM_REGINIT := ${foreach cam, ${CAMERA_MODEL},camera_${cam}_RegInit$(comma)}

CAM_REGINIT_LIST:=$(subst $(space),$(empty),$(CAM_REGINIT))
LOCAL_EXPORT_FLAG += CAM_REGINIT_LIST=$(CAM_REGINIT_LIST)


# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
                     platform/base/common \
                     platform/base/sx \
                     platform/chip \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/edrv/pmd \
                     platform/service/base \
                     target 

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the camera modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/camd/ov7660 \
        platform/edrv/camd/ov7670 \
        platform/edrv/camd/siv100b \
        platform/edrv/camd/sp80818
        
        
endif #RELEASE_MODULE

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC :=  ${notdir ${wildcard src/*.c}}
INPUT_FILES := 
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
