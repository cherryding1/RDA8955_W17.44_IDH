## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/simd

LOCAL_API_DEPENDS := \
	platform \
	platform/base/common \
	platform/chip/chip/hal \
	platform/base/sx \
	platform/base/std \
	platform/edrv \
	platform/edrv/simd/${SIMIF_MODEL} \
	
ifneq (${SIMIF_MODEL},)
LOCAL_MODULE_DEPENDS := platform/edrv/simd/${SIMIF_MODEL}
endif # (${SIMIF_MODEL},)


# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
ifneq (${SIMIF_MODEL},)
LOCAL_DEPEND_LIBS := \
			platform/edrv/simd/${SIMIF_MODEL} \

endif # (${SIMIF_MODEL},)
endif #RELEASE_MODULE

# Flags exported throughout the build
LOCAL_EXPORT_FLAG += SIMD_SIMIF_MODEL=${SIMIF_MODEL}


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


