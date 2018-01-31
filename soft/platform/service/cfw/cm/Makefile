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
LOCAL_NAME := ${PLATFORM_SERVICE}/cfw/cm

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/chip/hal \
	platform/base/common \
	platform/chip/defs \
	${PLATFORM_SYSTEM}/stack \
	platform/base/sx \
	${PLATFORM_SERVICE} \
	${PLATFORM_SERVICE}/cfw \
	${PLATFORM_SERVICE}/cfw/bal \
	platform/chip/pal \
	platform/phy \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \


# This is a top-level module
IS_TOP_LEVEL := no

# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG += NEED_BOOL

include ../mk

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

# Simulation specific inputs
INPUT_FILES :=  
## or include a local make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
