## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/cams

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					 platform \
					 platform/base/common \
					 ${PLATFORM_SERVICE} \
					 platform/base/sx \
					 platform/chip/defs \
					 platform/chip/hal \
					 platform/edrv/lcdd \
					 platform/edrv/camd \
					 ${PLATFORM_SYSTEM}/svc/imgs \
					 ${PLATFORM_SYSTEM}/svc/fmg \


# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


