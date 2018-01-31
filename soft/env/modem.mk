#**************************************************************#
# COPY THIS FILE AS "Makefile" IN THE "src" DIR OF YOUR MODULE #
# AND CUSTOMIZE IT TO FIT YOUR NEEDS.                          #
#**************************************************************#

# Pretending a top-level make if this is the entrance make
# of a modem build
ifeq "${MODEM_BUILD_ENTRANCE}" "1"
ifneq "$(origin MODEM_BUILD_ENTRANCE)" "environment"
$(error MODEM_BUILD_ENTRANCE should be an enviroment variable and the '-e' option should never be turned on at the same time)
endif
MODEM_BUILD_ENTRANCE := 0
MAKELEVEL := 0
endif

## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := ${CT_ASIC}

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"

LOCAL_MODULE_DEPENDS := 
LOCAL_MODULE_DEPENDS += at
LOCAL_MODULE_DEPENDS += platform

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := 



# Set this to any non-null string to signal a module which
# generates a binary (must contain a "main" entry point). 
# If left null, only a library will be generated.
IS_ENTRY_POINT := yes

# Set lod base name prefix to nothing
export CT_APP ?= _

# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
#LOCAL_EXPORT_FLAG += PAL_NO_DBGTRC PAL_NO_PRINTF
#LOCAL_EXPORT_FLAG += AERO2
#LOCAL_EXPORT_FLAG += USE_CT_CALIB
#LDFLAGS += --wrap sxs_BackTrace 
#LOCAL_EXPORT_FLAG += __GPRS__


# The following are only usable with the romulator
#LOCAL_EXPORT_FLAG += PAL_PROFILING
#LOCAL_EXPORT_FLAG += HAL_PROFILING
#LOCAL_EXPORT_FLAG += PAL_NO_TRACE
#LOCAL_EXPORT_FLAG += RFD_NO_TRACE

#Add by lixp for WAP/MMS
#LOCAL_EXPORT_FLAG += WAP_MMS_SUPPORT
# Start the Stack ?
#LOCAL_EXPORT_FLAG += __SXS_ONLY__

# Ask target to register the extra modules.
export TGT_REGISTER_PLATFORM_VERSIONS := 1

# Version of the MMI.
#LOCAL_EXPORT_FLAG += COOLMMI_VERSION_STRING=${CT_TARGET}

#LOCAL_EXPORT_FLAG += __DIRECT_ENTRY_FACTORY_MODE_ON_BOOTUP__

WITH_FLASH:=1


## --------------------------- ##
## Override toplevel settings  ##
## --------------------------- ##

# Set more dependencies as binary : they will not be compiled from source.
LOCAL_BINARY_LIBS ?=

# Set dependencies as source : the library will be generated from source
# This supersedes the BINARY (local+global) setting in case of conflict
LOCAL_SRC_LIBS ?=

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##

# Assembly / C code
S_SRC := 
C_SRC := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##

include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


