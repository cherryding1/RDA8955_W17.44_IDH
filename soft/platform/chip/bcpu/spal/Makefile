## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/chip/bcpu/spal

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/chip/regs/${CT_ASIC} \
					 platform/chip/bcpu
                      

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

# To set the ROM version
ifeq "${ROM_VERSION}" ""
ROM_VERSION_MAJOR ?= 1
ROM_VERSION_MINOR ?= 1

ROM_BUILD_DATE = $(shell echo $(BUILD_DATE) | sed 's/^20//')
ROM_VERSION ?= 0x$(ROM_VERSION_MAJOR)$(ROM_BUILD_DATE)$(ROM_VERSION_MINOR)
endif
LOCAL_EXPORT_FLAG += ROM_VERSION_NUMBER=${ROM_VERSION}

# To set the CT_ASIC_CAPD
LOCAL_EXPORT_FLAG += CT_ASIC_CAPD=${CT_ASIC_CAPD}

# By default, we want the profiling.
LOCAL_EXPORT_FLAG += SPAL_IRQ_PROFILING
LOCAL_EXPORT_FLAG += SPAL_PROFILING

ifeq ($(CT_ASICSIMU_DRIVERS),1)
LOCAL_API_DEPENDS += platform/chip/asicsimu
endif # CT_ASICSIMU_DRIVERS=1


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
