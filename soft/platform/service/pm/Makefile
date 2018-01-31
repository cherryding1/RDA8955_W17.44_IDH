#**************************************************************#
# COPY THIS FILE AS "Makefile" IN THE "src" DIR OF YOUR MODULE #
# AND CUSTOMIZE IT TO FIT YOUR NEEDS.                          #
#**************************************************************#


## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := ${PLATFORM_SERVICE}/pm


# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
					platform \
					platform/base/sx \
					${PLATFORM_SERVICE} \
					platform/chip/defs \
					platform/chip/hal \
					${PLATFORM_SYSTEM}/stack \
					platform/phy \
					platform/edrv/pmd  \

ifeq "${CHIP_HAS_AP}" "1"
LOCAL_API_DEPENDS += ${PLATFORM_SYSTEM}/svc/syscmds
endif

ifeq "${AT_USB_MODEM_NO_MMMI}" "y"
LOCAL_API_DEPENDS +=  \
                     ${PLATFORM_SYSTEM}/svc/uctls  \
                     ${PLATFORM_SYSTEM}/svc/umss \
                     ${PLATFORM_SYSTEM}/svc/uat \
                     ${PLATFORM_SYSTEM}/svc/uwifi \
                     ${PLATFORM_SYSTEM}/svc/uvideos
endif

LOCAL_MODULE_DEPENDS := \
#                   platform/base/dbg

# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := yes


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.


# Those were there before and seem useless
# LOCAL_EXPORT_FLAG += __NEW_ENV__
# TODO remove once checked
# LOCAL_EXPORT_FLAG += NEED_BOOL
# LOCAL_EXPORT_FLAG += ENABLE_CSW_ATCOM
# LOCAL_EXPORT_FLAG += HAVE_CFW_CONFIG
# LOCAL_EXPORT_FLAG += MF32_MIPS_PLATFORM 
# LOCAL_EXPORT_FLAG += MF32_PLATFORM
# LOCAL_EXPORT_FLAG += MF32_BASE_REG_DISABLE
# LOCAL_EXPORT_FLAG += MF32_HAVE_NO_SUL_LIB
# LOCAL_EXPORT_FLAG += MF32_HAVE_CFW_TS
# LOCAL_EXPORT_FLAG += MF32_NO_RFM_RH
# LOCAL_EXPORT_FLAG += MF32_NO_RFM
# LOCAL_EXPORT_FLAG += MF32_CFW_MMI_TWO_TASKS

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


