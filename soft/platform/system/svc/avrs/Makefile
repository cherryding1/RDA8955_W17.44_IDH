## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/avrs

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
					 ${PLATFORM_SERVICE} \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/aud \
					 platform/edrv/camd \
					 platform/edrv/lcdd \
                     ${PLATFORM_SYSTEM}/vpp/speech \
                     ${PLATFORM_SYSTEM}/vpp/amjr \
                     ${PLATFORM_SYSTEM}/svc \
                     ${PLATFORM_SYSTEM}/svc/imgs \
                     ${PLATFORM_SYSTEM}/svc/fss \
                     ${PLATFORM_SYSTEM}/svc/fmg \
                     target \
					 platform/base/sx \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := target

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

