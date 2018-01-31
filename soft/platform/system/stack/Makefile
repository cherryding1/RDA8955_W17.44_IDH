## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## ------------------------------------- ##
##	Include Stack Dependency Macro   ##
## ------------------------------------- ##
include stack.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_STACK}

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/chip/hal \
	platform/chip/pal \
	platform/base/common \
	platform/chip/defs \

# Compile the sub-modules, except when the "stack" must be used as a library.
ifeq ($(findstring ${PLATFORM_STACK}/packed,${GLOBAL_BINARY_LIBS}),)

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \
	${PLATFORM_STACK}/api \
	${PLATFORM_STACK}/cc \
	${PLATFORM_STACK}/cod \
	${PLATFORM_STACK}/e2p \
	${PLATFORM_STACK}/fsm \
	${PLATFORM_STACK}/lap \
	${PLATFORM_STACK}/mm \
	${PLATFORM_STACK}/rr \
	${PLATFORM_STACK}/sms \
	${PLATFORM_STACK}/ss \
	${PLATFORM_STACK}/l1 \
	${PLATFORM_STACK}/sim

ifeq "${GPRS_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += \
	${PLATFORM_STACK}/llc \
	${PLATFORM_STACK}/rld \
	${PLATFORM_STACK}/rlu \
	${PLATFORM_STACK}/sm \
	${PLATFORM_STACK}/snd
endif

ifeq (,$(filter "1" "y",$(CB_SUPPORT)))
LOCAL_MODULE_DEPENDS += \
	${PLATFORM_STACK}/cb
endif
# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

C_SRC += stack_map.c stk_profile.c

else # GLOBAL_BINARY_LIBS

SIM_STEMS := _singlesim _dualsim _threesim _foursim

ifeq "${STACK_NO_PRINTF}" "y"
PS_STEM := 
ifeq "${GPRS_SUPPORT}" "y"
PS_STEM := _gprs
endif
ifeq "${EGPRS_SUPPORT}" "y"
PS_STEM := _egprs
endif
else
PS_STEM := _trace
ifeq "${GPRS_SUPPORT}" "y"
PS_STEM := _gprs_trace
endif
ifeq "${EGPRS_SUPPORT}" "y"
PS_STEM := _egprs_trace
endif
endif

PACKED_LIB := $(notdir ${LOCAL_NAME})$(word ${NUMBER_OF_SIM},${SIM_STEMS})${PS_STEM}

PACKED_LIB_PATH := ${LOCAL_NAME}/packed/${CT_ASIC}/${PACKED_LIB}

LOCAL_BINARY_LIBS    += ${PACKED_LIB_PATH}
LOCAL_MODULE_DEPENDS := ${PACKED_LIB_PATH}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

AUTO_XCP2H ?= yes

LOCAL_EXPORT_FLAG += __XCPU__ 

export WITHOUT_WSHADOW := 1
export WITHOUT_WSIGN_COMPARE := 1

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
