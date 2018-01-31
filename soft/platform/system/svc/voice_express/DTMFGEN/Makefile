## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/voice_express/DTMFGEN

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/aud \
					 ${PLATFORM_SERVICE} \
           ${PLATFORM_SYSTEM}/vpp/speech\
           ${PLATFORM_SYSTEM}/svc/vois \
           ${PLATFORM_SYSTEM}/svc \
           target \
           platform/base/sx \
					 platform/base/std \
					 ${PLATFORM_SYSTEM}/calib \
					 application/thirdpartylibs/xf_magic_sound \
					 application/thirdpartylibs/magic_sound \

					 
#					 ${PLATFORM_SYSTEM}/mdi/alg/noise_suppress
					 


# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= no

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

# Magic AE
ifeq "${MAGIC_AE_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MAGIC_AE_SUPPORT
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
