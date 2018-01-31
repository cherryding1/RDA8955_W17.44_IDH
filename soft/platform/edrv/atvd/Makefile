## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/atvd

LOCAL_API_DEPENDS := platform \
                     platform/base/common \
                     platform/chip/hal \
                     platform/base/sx \
                     platform/base/std \

ifneq "${ATV_MODEL}" ""
ifneq "$(findstring rda5888,${ATV_MODEL})" ""
LOCAL_MODULE_DEPENDS := platform/edrv/atvd/rda5888
else # not rda5888
LOCAL_MODULE_DEPENDS := platform/edrv/atvd/${ATV_MODEL}
endif # not rda5888
endif # ATV_MODEL

# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
LOCAL_DEPEND_LIBS := platform/edrv/atvd/rda5888 	\

endif #RELEASE_MODULE

# 32K clock connection
ifeq "${TV_USE_32K_CLK_PIN}" "1"
LOCAL_EXPORT_FLAG += TV_USE_32K_CLK_PIN
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


