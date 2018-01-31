## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/tsd

ifneq (${TSD_MODEL},)
LOCAL_MODULE_DEPENDS := platform/edrv/tsd/${TSD_MODEL}
endif # (${TSD_MODEL},)

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
                     platform/chip \
                     platform/sx \
                     platform/chip/hal \
                     platform/edrv/i2cgpio \
                     target 

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the mcd modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/tsd/tsc2046 \
        platform/edrv/tsd/${TSD_MODEL} \
        platform/edrv/tsd/rda1203_gallite \

endif #RELEASE_MODULE

#for doc to include each model's target structure
EXTRA_DOC_INPUTS := \
        platform/edrv/tsd/tsc2046 \
        platform/edrv/tsd/${TSD_MODEL} \
        platform/edrv/tsd/rda1203_gallite \

LOCAL_EXPORT_FLAG += 

# Generates the CoolWatcher headers automatically.
#AUTO_XMD2H ?= yes

# Generates the CoolProfile headers automatically.
AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
#AUTO_GEN_REVISION_HEADER := yes

# Compilation mode for the version string.
#LOCAL_EXPORT_FLAG += TSD_TS_MODEL=${TSD_MODEL}


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
S_SRC :=
C_SRC := tsdp_calib.c
INPUT_FILES := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
