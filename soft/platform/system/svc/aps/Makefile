## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/aps

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/aud \
					 platform/edrv/lcdd \
                                   ${PLATFORM_SYSTEM}/calib \
					 ${PLATFORM_SYSTEM}/vpp/speech \
                     ${PLATFORM_SYSTEM}/vpp/amjp \
                     target \
                     platform/base/sx

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?=

ifeq "${CSW_EXTENDED_API_AUDIO_VIDEO}" "1"
LOCAL_EXPORT_FLAG += ENABLE_DEPRECATED
endif

# TODO: remove this
WITHOUT_WSHADOW := 1

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
