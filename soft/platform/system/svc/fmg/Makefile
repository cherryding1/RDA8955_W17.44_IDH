## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/fmg

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/base/common \
					platform/chip/hal\
					platform/chip/defs\
					target \
					platform/edrv/lcdd \
					platform/base/sx

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += FMG_PROFILE

ifeq "${CHIP_HAS_AP}" "1"
LOCAL_EXPORT_FLAG += NO_FMG_DISPLAY
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
