## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/gpsd


# include the target definition here to know which libs to link to
LOCAL_MODULE_DEPENDS := \

ifneq (${GPSD_MODEL},)
LOCAL_MODULE_DEPENDS += platform/edrv/gpsd/${GPSD_MODEL}
endif # (${GPSD_MODEL},)
			
# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					at	\
					at/ats/gps	\
					at/ate	\
					at/utility	\
					at/slimmmi/main	\
					platform \
					platform/chip \
					platform/chip/regs/$(CT_ASIC) \
					platform/stack \
					platform/base/common \
					platform/base/sx \
					platform/chip/hal \
					platform/chip/defs \
					platform/csw \
					platform/edrv/gpsd \
					platform/edrv/gpsd/rda5900 \
					platform/edrv/pmd \
					platform/edrv/memd \
					platform/edrv \
					platform/edrv/i2cgpio \
					platform/service \
					target/$(CT_TARGET) \
					target 

# This is a top-level module
IS_TOP_LEVEL := yes


ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
LOCAL_DEPEND_LIBS := platform/edrv/gpsd/rda5900
endif #RELEASE_MODULE

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += 

#LOCAL_LIB += platform/edrv/gpsd/lib/.a
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


