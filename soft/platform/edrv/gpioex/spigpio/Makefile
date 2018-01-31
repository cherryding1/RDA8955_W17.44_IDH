## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/gpioex/spigpio

LOCAL_API_DEPENDS := platform \
                    platform/base/common \
                    platform/base/std \
                    platform/base/sx \
                    platform/edrv/gpioex/spigpio \
                    platform/chip/hal \
                    target

LOCAL_MODULE_DEPENDS := platform/edrv/gpioex/spigpio/


# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
LOCAL_DEPEND_LIBS := \
			platform/edrv/gpioex/spigpio/ \

endif #RELEASE_MODULE

#for doc to include each model's target structure
EXTRA_DOC_INPUTS := \
        platform/edrv/gpioex/spigpio/ \

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= no

# Generates the CoolProfile headers automatically.
#AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := no

# Compilation mode for the version string.


# Flags exported throughout the build
LOCAL_EXPORT_FLAG +=


## ------------------------------------------------------------------------- ##
##	                       Source files                                      ##
## ------------------------------------------------------------------------- ##
# No source files in the top-level itself...
S_SRC := 
C_SRC := 
INPUT_FILES := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


