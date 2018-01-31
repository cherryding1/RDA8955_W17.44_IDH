## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/dualsimd

LOCAL_API_DEPENDS := platform \
                    platform/base/common \
                    platform/chip/hal \
                    platform/base/sx \
                    platform/base/std \
                    platform/edrv

ifneq (${DUALSIM_MODEL},)
LOCAL_MODULE_DEPENDS := platform/edrv/dualsimd/${DUALSIM_MODEL}
endif # (${DUALSIMD_MODEL},)


# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
ifneq (${DUALSIM_MODEL},)
LOCAL_DEPEND_LIBS := \
			platform/edrv/dualsimd/${DUALSIM_MODEL} \

endif # (${DUALSIMD_MODEL},)
endif #RELEASE_MODULE

# Flags exported throughout the build
ifeq "${DUALSIM_SWAP}" "1"
LOCAL_EXPORT_FLAG += DUALSIM_SWAP
endif # DUALSIM_SWAP


## ------------------------------------------------------------------------- ##
##	                       Source files                                      ##
## ------------------------------------------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC := 
INPUT_FILES :=  

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


