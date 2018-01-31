## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/base/sap

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/base/common \
	platform/base/sx \
	platform/base/std \
	${PLATFORM_SYSTEM}/stack \
	platform/chip/hal \
	platform/chip/defs \
	platform/chip/pal \
	${PLATFORM_SYSTEM}/svc/syscmds

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
ifeq "${MULTI_RAT}" "1"
LOCAL_EXPORT_FLAG += __MULTI_RAT__
endif

ifeq "${USIM_SUPPORT}" "y"
LOCAL_EXPORT_FLAG += USIM_SUPPORT
endif

ifeq "${NO_PROTOSTACK}" "1"
LOCAL_EXPORT_FLAG += NO_PROTOSTACK
endif

ifeq "${SIM_LIMIT_RF_TX_POWER}" "1"
LOCAL_EXPORT_FLAG += SIM_LIMIT_RF_TX_POWER
endif

LOCAL_EXPORT_FLAG += 


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

# Include the flags of the stack so that sap matches the stack configuration
include ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/stack/stack.mk

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
