## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/mps

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
                    platform \
                     platform/base/common \
                     platform/chip/hal    \
                     platform/chip/defs   \
                     platform/edrv/lcdd   \
                     platform/edrv/aud    \
                     ${PLATFORM_SYSTEM}/svc/avps    \
                     ${PLATFORM_SYSTEM}/stack       \
                     ${PLATFORM_SERVICE}         \
                     target platform/base/sx

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

AUTO_XCP2H ?= yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
