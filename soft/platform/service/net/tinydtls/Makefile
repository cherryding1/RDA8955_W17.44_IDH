## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/service/net/tinydtls


# Add includes from other modules we do not wish to link to
# FIXME: Improve the inclusion of target dependencies ! 
LOCAL_API_DEPENDS := \
					platform \
					platform/chip/hal \
					platform/chip/defs \
					platform/chip/hal/${CT_ASIC} \
					platform/base/sx \
					platform/service \
					platform/service/base \
					platform/service/net \
					target \
					target/${CT_TARGET} \
					${API_PLATFORM_DEPENDS}


# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \


# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := no


# For a toplevel module, define which tests to include in delivery
# Skip the 'tests' in the names. Beware that everything from the 
# given tests directories will be included...
TOPLEVEL_DELIVER_TESTS := 

# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG +=

#include ../mk

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
include make.srcs
C_SRC := ${SOURCE_FILES}
LOCAL_ADD_INCLUDE := ${SOURCE_INCLUDE}

USE_DIFFERENT_SOURCE_LAYOUT := yes
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
