## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/rfd

# List of submodules which contain code we need to include in the final lib

LOCAL_MODULE_DEPENDS := platform/edrv/rfd/${XCV_MODEL}

ifneq "${PA_MODEL}" ""
LOCAL_MODULE_DEPENDS += platform/edrv/rfd/${PA_MODEL}
endif #PA_MODEL

ifneq "${SW_MODEL}" ""
LOCAL_MODULE_DEPENDS += platform/edrv/rfd/${SW_MODEL} 
endif #SW_MODEL

LOCAL_API_DEPENDS ?= platform \
					 platform/base/common \
					 platform/chip/hal \
					 platform/chip/defs \
					 platform/edrv/rfd \
                     platform/base/sx \
					 ${PLATFORM_SYSTEM}/calib	 \
                     platform/edrv/pmd \		 

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the rf modules 
LOCAL_MODULE_DEPENDS := \
			platform/rfd/sw_dummy \
			platform/rfd/pa_dummy \
			platform/rfd/pasw_rda6231 \
			platform/rfd/xcv_rda6220 \
			platform/rfd/xcv_rda6220_gallite \
			platform/rfd/xcv_rda6220_gallite_CT1129 \
			platform/rfd/xcv_8808 \
			platform/rfd/xcv_8809e2 \
                        platform/rfd/xcv_8955

endif  #RELEASE_MODULE

# Overwrite the normal optimisation to fit in the internal SRAM
export MYCFLAGS = -O2

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

# Compilation mode for the version string.
LOCAL_EXPORT_FLAG += RFD_XCV_MODEL=${XCV_MODEL}
LOCAL_EXPORT_FLAG += RFD_PA_MODEL=${PA_MODEL}
LOCAL_EXPORT_FLAG += RFD_SW_MODEL=${SW_MODEL}

ifeq "${PA_VRAMP_220PF}" "y"
LOCAL_EXPORT_FLAG += PA_VRAMP_220PF
endif # PA_VRAMP_220PF

## ------------------------------------------------------------------------- ##
##	                       Source files                                      ##
## ------------------------------------------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC := rfd_map.c
INPUT_FILES :=  

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
