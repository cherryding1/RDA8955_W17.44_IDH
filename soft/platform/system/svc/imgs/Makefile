## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/imgs

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					 platform \
					 platform/chip/defs \
					 platform/chip/hal \
					 platform/base/sx \
					 ${PLATFORM_SERVICE} \
					 platform/edrv/lcdd \


## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
