## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/base

# Compile the sub-modules, except when the "base" must be used as a library.
ifeq ($(findstring platform/base/packed,${GLOBAL_BINARY_LIBS}),)

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
					    platform/base/dbg \
					    platform/base/sap \
					    platform/base/std \
					    platform/base/sx \
					    platform/base/fdlibm \

else # GLOBAL_BINARY_LIBS

SIM_STEMS := _singlesim _dualsim _threesim _foursim

PS_STEM :=
ifeq "${GPRS_SUPPORT}" "y"
PS_STEM := _gprs
endif
ifeq "${EGPRS_SUPPORT}" "y"
PS_STEM := _egprs
endif

PACKED_LIB := $(notdir ${LOCAL_NAME})$(word ${NUMBER_OF_SIM},${SIM_STEMS})${PS_STEM}

PACKED_LIB_PATH := ${LOCAL_NAME}/packed/${PACKED_LIB}

LOCAL_BINARY_LIBS    += ${PACKED_LIB_PATH}
LOCAL_MODULE_DEPENDS := ${PACKED_LIB_PATH}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
