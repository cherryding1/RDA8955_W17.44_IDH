## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip

# Compile the sub-modules, except when the "chip" must be used as a library.
ifeq ($(findstring platform/chip/packed,${GLOBAL_BINARY_LIBS}),)

LOCAL_API_DEPENDS    := 
LOCAL_MODULE_DEPENDS := \
						platform/chip/bcpu \
						platform/chip/boot \
						platform/chip/hal \
						platform/chip/patch \
						platform/chip/pal \

ifeq "${LTE_NBIOT_SUPPORT}" "1"
LOCAL_MODULE_DEPENDS += platform/chip/nbcpu
endif

ifeq ($(CT_ASICSIMU_DRIVERS),1)
LOCAL_MODULE_DEPENDS += platform/chip/asicsimu
endif # CT_ASICSIMU_DRIVERS=1

# To set the REWRITE_ROM_AT_BOOT flag
ifeq "${REWRITE_ROM_AT_BOOT}" "1"
LOCAL_EXPORT_FLAG  += REWRITE_ROM_AT_BOOT
endif

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

PACKED_LIB_PATH := ${LOCAL_NAME}/packed/${CT_ASIC}/${PACKED_LIB}

LOCAL_BINARY_LIBS    += ${PACKED_LIB_PATH}
LOCAL_MODULE_DEPENDS := ${PACKED_LIB_PATH}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module.
IS_TOP_LEVEL := yes
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
