## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/mcd

ifneq (${MCD_IF},)
LOCAL_MODULE_DEPENDS := platform/edrv/mcd/mcd_${MCD_IF}
endif # (${MCD_IF},)

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform platform/chip platform/sx \
                     platform/chip/hal target 

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the mcd modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/mcd/mcd_${MCD_IF} \

endif #RELEASE_MODULE

## ------------------------------------ ##
##  Add your custom flags here          ##
## ------------------------------------ ##

#for doc to include each interface's target structure
EXTRA_DOC_INPUTS := \
                    platform/edrv/mcd/mcd_null \
                    platform/edrv/mcd/mcd_${MCD_IF} \

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
