## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/fmd

LOCAL_API_DEPENDS := platform \
				 	 platform/base/common \
					 platform/chip/hal \
                     platform/base/sx \
					 platform/base/std \
					 target \

ifneq ($(findstring rda5888,${FM_MODEL}), )
LOCAL_MODULE_DEPENDS := platform/edrv/fmd/rda5888
else
ifneq (${FM_MODEL},)
LOCAL_MODULE_DEPENDS := platform/edrv/fmd/${FM_MODEL}
endif
endif # rda5888

# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
ifneq (${FM_MODEL},)
LOCAL_DEPEND_LIBS := platform/edrv/fmd/rda5888
LOCAL_DEPEND_LIBS += platform/edrv/fmd/rda5802e_gallite 
endif # (${FM_MODEL},)
endif #RELEASE_MODULE

#for doc to include each model's target structure
EXTRA_DOC_INPUTS := \
        platform/edrv/fmd/rda5800 \
        platform/edrv/fmd/rda5802 \

# 32K clock connection
ifeq "${FM_USE_32K_CLK_PIN}" "1"
LOCAL_EXPORT_FLAG += FM_USE_32K_CLK_PIN
endif

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


