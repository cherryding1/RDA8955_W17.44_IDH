## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/avps

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/aud \
					 platform/edrv/lcdd \
                     ${PLATFORM_SYSTEM}/vpp/speech \
                     ${PLATFORM_SYSTEM}/vpp/amjp \
                     ${PLATFORM_SYSTEM}/stack \
                     target platform/base/sx

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
AUTO_XCP2H ?= yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
