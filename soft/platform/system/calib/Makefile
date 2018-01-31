## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_ADD_ABS_INCLUDE := ${TARGET_DIR}/${CT_TARGET}/include

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/calib

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
                     platform \
                     platform/base/common \
                     platform/base/std\
                     platform/base/sx \
                     platform/chip/bcpu \
                     platform/chip/boot \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/chip/hal/${CT_ASIC} \
                     platform/chip/pal\
                     platform/edrv/aud \
                     platform/edrv/lcdd \
                     platform/edrv/memd\
                     platform/edrv/pmd \
                     platform/edrv/rfd \
                     ${PLATFORM_SERVICE}/base \
                     ${PLATFORM_SYSTEM}/stack \
                     ${PLATFORM_SYSTEM}/svc/fmg\
                     ${PLATFORM_SYSTEM}/svc/syscmds\
                     ${PLATFORM_SYSTEM}/svc/uctls\
                     ${PLATFORM_SYSTEM}/svc/vois \
                     target \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

# Generate the CoolProfile header automatically.
AUTO_XCP2H ?= yes

# Generate the xmd header automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

# Using the small screen driver.
ifeq "${SMALL_SCREEN}" "1"
# Define this on the command line to compile for the small screen (128x128).
LOCAL_EXPORT_FLAG += LCM_SM
endif

# The calib stub is always embedded in the production code.
# To minimize its footprint it is possible to disable the screen
# display by not setting the CES_DISPLAY flag.
#
# Disabling the display will save around 50KBytes on the library
# size, this is 30% of the Calib library size.
#
# This flag is always set by the calib_stub test and the Proto,
# so that we get the display for the standalone calibration stub.
#

# Default value:
ifeq "${CHIP_HAS_AP}" "1"
CES_DISPLAY := n
endif
ifeq "${CES_DISPLAY}" "y"
LOCAL_EXPORT_FLAG += CES_DISPLAY
endif

# Profile in calib
LOCAL_EXPORT_FLAG += CALIB_PROFILING

# USB support in calib
ifeq "${UCTLS_SERVICE}" ""
LOCAL_EXPORT_FLAG += CALIB_WITHOUT_USB
else
ifeq "${UCTLS_SERVICE}" "no"
LOCAL_EXPORT_FLAG += CALIB_WITHOUT_USB
endif
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

