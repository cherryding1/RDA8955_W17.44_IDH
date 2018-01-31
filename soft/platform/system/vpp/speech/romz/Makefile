## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := ${PLATFORM_SYSTEM}/vpp/speech/romz

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform platform/base/common platform/chip/${CT_ASIC} \
                     platform/chip/hal ${PLATFORM_SYSTEM}/vpp/speech 

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# To set the CHIP_BYPASS_ROM flag
ifeq "${CHIP_BYPASS_ROM}" "1"
LOCAL_EXPORT_FLAG  += CHIP_BYPASS_ROM
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
