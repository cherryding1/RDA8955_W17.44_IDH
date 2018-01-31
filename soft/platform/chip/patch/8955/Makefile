## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/chip/patch/${CT_ASIC}

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform platform/base/common \
                     platform/chip/regs/${CT_ASIC} \
                     platform/chip/defs \
                     platform/chip/patch \
                     platform/chip/bcpu \
                     platform/chip/bcpu/spc \
                     platform/chip/bcpu/spp \
                     platform/chip/bcpu/spal \
                     platform/chip/hal \
                     ${PLATFORM_SYSTEM}/vpp 

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 

# other dependencies
LOCAL_ADD_INCLUDE ?= /platform/chip/bcpu/spc/src \
                     /platform/chip/bcpu/spp/src


# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# By default, we want the profiling.
LOCAL_EXPORT_FLAG += SPC_FUNC_PROFILING
LOCAL_EXPORT_FLAG += SPC_WIN_PROFILING
LOCAL_EXPORT_FLAG += SPC_PULSE_PROFILING

## ------------------------------------- ##
##      List all your sources here       ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
