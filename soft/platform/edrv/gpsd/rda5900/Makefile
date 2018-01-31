## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/gpsd/rda5900

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS :=	\
					platform \
					platform/chip \
					platform/stack \
					platform/base/common \
					platform/base/sx \
					platform/chip/hal \
					platform/chip/defs \
					platform/edrv/gpsd \
					platform/edrv/pmd \
					platform/edrv \
					target 
# This is a top-level module
IS_TOP_LEVEL := no
## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += 
## ------------------------------------------------------------------------- ##
##	                       Source files                                      ##
## ------------------------------------------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC :=  ${notdir ${wildcard src/*.c}}
INPUT_FILES :=  

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
