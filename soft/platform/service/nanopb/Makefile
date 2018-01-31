## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/nanopb

# This is a top-level module
IS_TOP_LEVEL := yes

LOCAL_API_DEPENDS := platform

# list all modules APIs that are neccessary to compile this module
LOCAL_MODULE_DEPENDS := ${PLATFORM_SERVICE}/nanopb/core

C_SRC := $(notdir $(wildcard src/*.c))

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
