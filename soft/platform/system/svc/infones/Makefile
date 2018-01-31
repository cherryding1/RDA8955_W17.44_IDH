## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/infones

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/aud \
					 ${PLATFORM_SERVICE} \
                     ${PLATFORM_SYSTEM}/vpp/speech \
                     target \
                     platform/base/sx \
					 platform/base/std \
					 ${PLATFORM_SYSTEM}/stack \
					 ${PLATFORM_SYSTEM}/mdi/alg/noise_suppress \
					 


# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

# Overwrite the normal optimisation to fit in the internal SRAM
#MYCFLAGS = -O3 
#MYCFLAGS += -fmove-all-movables 
#MYCFLAGS += -freduce-all-givs
%/%.o: CT_MIPS16_CFLAGS=
export MYCFLAGS=-O3

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
