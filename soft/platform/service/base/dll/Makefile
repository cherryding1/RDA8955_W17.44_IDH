## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/base/dll

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
			platform \
			platform/base/common \
			platform/base/sx \
			platform/chip/hal \
			platform/chip/boot \
			platform/chip/defs \
			platform/chip/regs/${CT_ASIC} \
			${PLATFORM_SERVICE} \
			${PLATFORM_SYSTEM}/svc/fstraces \
			${PLATFORM_SERVICE}/base \
			${PLATFORM_SYSTEM}/stack \
			platform/edrv/pmd \
			target

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \

# This is a top-level module
IS_TOP_LEVEL := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}}


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
