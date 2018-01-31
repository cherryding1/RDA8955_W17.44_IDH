## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/pmd


# include the target definition here to know whic libs to link to
LOCAL_MODULE_DEPENDS := platform/edrv/pmd/${PM_MODEL}

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
                     platform/base/common \
                     target \
                     platform/base/sx \
                     platform/chip/hal \
                     platform/chip/defs \
                     platform/edrv \
                     platform/edrv/pmd/${PM_MODEL} \

# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the pm modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/pmd/rda1203_gallite \
        platform/edrv/pmd/rda1203_gallite_CT1129 \
        platform/edrv/pmd/pmu_8909 \
        platform/edrv/pmd/pmu_8809 \
		platform/edrv/pmd/pmu_8955 \

endif #RELEASE_MODULE

#for doc to include each model's target structure
EXTRA_DOC_INPUTS := \
        platform/edrv/pmd/rda1203_gallite \
        platform/edrv/pmd/rda1203_gallite_CT1129 \
        platform/edrv/pmd/pmu_8909 \
        platform/edrv/pmd/pmu_8809 \
        platform/edrv/pmd/pmu_8955 \

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

# Compilation mode for the version string.
LOCAL_EXPORT_FLAG += PMD_PM_MODEL=${PM_MODEL}

# Whether to bypass charger battery check
ifeq "${NO_CHARGER_BATTERY_CHECK}" "1"
LOCAL_EXPORT_FLAG += NO_CHARGER_BATTERY_CHECK
endif

## ------------------------------------- ##
##      List all your sources here       ##
## ------------------------------------- ##
S_SRC :=
C_SRC := pmd_charger_common.c pmd_map.c
INPUT_FILES := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
