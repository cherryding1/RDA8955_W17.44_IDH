## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/service/net/app_http


# Add includes from other modules we do not wish to link to
# FIXME: Improve the inclusion of target dependencies ! 
LOCAL_API_DEPENDS := \
					platform \
					platform/service \
					platform/service/net \
					platform/service/net/http \
					platform/service/net/http/http \
					platform/service/net/http/net \
					platform/service/net/http/util \
					platform/base/sx \
					platform/chip/defs \
					platform/chip/hal \
					platform/service/net/lwip/src \
					platform/service/net/lwip/fuzz \
					platform/service/net/lwip/util \
					application/coolmmi/mmi/wapmms/jdd \
					at/ats/http \
					at \
					at/ate \

LOCAL_ADD_INCLUDE := platform/service/net/lwip/src/apps/altcp_tls
LOCAL_ADD_INCLUDE += platform/service/net/mbedtls/src/mbedtls/include

					
# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=


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

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
