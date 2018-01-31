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
LOCAL_NAME := at/ats/sms

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_DEPEND_LIBS := 

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := at at/ats at/utility at/ats/cc at/ats/gc at/ats/gprs at/ats/id at/ats/nw at/ats/pbk at/ats/sat at/ats/sim at/ats/sms at/ats/special at/ats/ss at/ats/tcpip at/ats/wbxml at/ats/xml at/ats/charset at/ats/jdd at/ats/jdi \
                     application/systeminc/mmi \
                     application/coolmmi \
                     at/ate \
                     ${API_PLATFORM_DEPENDS} \
                    

# Set this to any non-null string to signal a module which 
# generates a binary (must contain a "main" entry point). 
# If left null, only a library will be generated.
IS_ENTRY_POINT := no

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
IS_TOP_LEVEL := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += \
		HAVE_CFW_CONFIG \
		MF32_BASE_REG_DISABLE \
		MF32_NO_RFM_RH \
		MF32_NO_RFM \
		MF32_CFW_MMI_TWO_TASKS

ifeq "${CT_RELEASE}" "cool_profile"
	LOCAL_EXPORT_FLAG += CSW_PROFILING
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}}


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
