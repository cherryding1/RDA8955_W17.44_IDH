## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

######################################
# Name of the module
######################################
LOCAL_NAME := platform/chip/bcpu

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
					platform/chip/bcpu/spal \
					platform/chip/bcpu/spp \
					platform/chip/bcpu/spc \

LOCAL_API_DEPENDS := platform \
					 platform/chip/hal \

# This is a top-level module
IS_TOP_LEVEL := yes

# Overwrite the normal optimisation to fit in the internal SRAM
# and to avoid inlining.
# (Inlining is the patching hell.)
export MYCFLAGS = -O2 -fno-inline-functions 

# To set the CHIP_BYPASS_ROM flag
ifeq "${CHIP_BYPASS_ROM}" "1"
LOCAL_EXPORT_FLAG  += CHIP_BYPASS_ROM
endif

## ------------------------------------------------------------------------- ##
##                         Source files                                      ##
## ------------------------------------------------------------------------- ##
S_SRC := 
C_SRC := 
INPUT_FILES :=  

ifneq "${BUILD_ROM}" "1"
ifneq "${CHIP_HAS_BCPU_ROM}-${BUILD_BCPU_IMAGE}" "1-1"
C_SRC += bcpu_registermap.c
endif
endif

## ------------------------------------- ##
## Do Not touch below this line          ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
