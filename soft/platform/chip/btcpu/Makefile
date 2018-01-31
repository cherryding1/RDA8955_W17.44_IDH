## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/chip/btcpu

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/chip/regs/${CT_ASIC} \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

# This is a top-level module
IS_TOP_LEVEL := yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# Not to generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := no

#LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
