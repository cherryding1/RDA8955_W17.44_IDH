## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip/hal

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform                       \
                    platform/base/common            \
                    platform/base/sx                \
                    platform/base/std               \
                    ${PLATFORM_SYSTEM}/calib                  \
                    platform/chip/bcpu              \
                    platform/chip/boot              \
                    platform/chip/boot/$(CT_ASIC)   \
                    platform/chip/defs              \
                    platform/chip/hal/$(CT_ASIC)    \
                    platform/chip/pal               \
                    platform/chip/patch             \
                    platform/chip/regs/$(CT_ASIC)   \
                    platform/edrv/aud               \
                    platform/service/base \
                    platform/edrv/dualsimd          \
                    platform/edrv/memd              \
                    platform/edrv/pmd               \
                    platform/edrv/pmd/${PM_MODEL}   \
                    platform/edrv/rfd               \
                    target                          \

# List other APIs that are neccessary to compile this module
LOCAL_ADD_INCLUDE := $(LOCAL_NAME)/$(CT_ASIC)/src \
                     platform/chip/boot/$(CT_ASIC)/src

ifeq "${CT_ASIC}" "8909"
LOCAL_ADD_INCLUDE += $(LOCAL_NAME)/src/$(CT_ASIC)
endif

# list all the modules that need to be compiled and added to this module library
LOCAL_MODULE_DEPENDS := platform/chip/regs/$(CT_ASIC)

# This is a top-level module
IS_TOP_LEVEL := yes

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# Forces the trace to be sent over USB Host.
#LOCAL_EXPORT_FLAG += HAL_TRACE_USB

# Enable some drivers to be more or less verbose.
#LOCAL_EXPORT_FLAG += HAL_USB_PRINTF
#LOCAL_EXPORT_FLAG += HAL_CAMERA_PRINTF
#LOCAL_EXPORT_FLAG += HAL_NO_PRINTF
# USB trace is not reliable now. Maybe sxs_SerialDesc Array access violation 
LOCAL_EXPORT_FLAG += HAL_TRACE_FORCE_HOST

# Compilation mode of HAL used, for the version string.
LOCAL_EXPORT_FLAG += HAL_RELEASE_MODE=${CT_RELEASE}
LOCAL_EXPORT_FLAG += HAL_ASIC_CAPD=${CT_ASIC_CAPD}_${CT_ASIC_CFG}

ifeq "${CT_RELEASE}" "cool_profile"
LOCAL_EXPORT_FLAG += ENABLE_PXTS
LOCAL_EXPORT_FLAG += PXTS_MAX_SUPPORTED_FREQUENCY=52000000
# FIXME: This is a bit permissive. Should only be used in SX...
# Necessary here as a part of the jobs are profiled in HAL.
LOCAL_EXPORT_FLAG += SX_PROFILING
endif

# Configurable exception behaviour:
# User can choose whether to restart system or enter GDB when exception occurs.
# Only check this flag when CT_RELEASE is "release"
HAL_GDB_CONFIG ?= 1
ifeq "${CT_RELEASE}" "release"
ifeq "${HAL_GDB_CONFIG}" "1"
LOCAL_EXPORT_FLAG += HAL_GDB_CONFIG
endif
endif

# Whether to skip checking BCPU error info
ifeq "${NO_BCPU_IMAGE}" "1"
LOCAL_EXPORT_FLAG += NO_BCPU_IMAGE
endif


# MIC capless mode
ifeq "${MIC_CAPLESS_MODE}" "y"
LOCAL_EXPORT_FLAG += MIC_CAPLESS_MODE
endif

# FM line-in record
ifeq "${FM_LINEIN_RECORD}" "1"
LOCAL_EXPORT_FLAG += FM_LINEIN_RECORD
endif

# Headphone charge pump
ifeq "${HEADPHONE_CHARGE_PUMP}" "1"
LOCAL_EXPORT_FLAG += HEADPHONE_CHARGE_PUMP=1
else
LOCAL_EXPORT_FLAG += HEADPHONE_CHARGE_PUMP=0
endif

ifeq ($(CT_ASICSIMU_DRIVERS),1)
LOCAL_API_DEPENDS += platform/chip/asicsimu
endif # CT_ASICSIMU_DRIVERS=1

ifneq (${CAMERA_MODEL},)
LOCAL_EXPORT_FLAG += HAL_HAS_CAMERA
endif

ifneq (${LCD_MODEL},)
LOCAL_EXPORT_FLAG += HAL_HAS_LCD
endif

## ------------------------------------- ##
##  List all your sources here           ##
## ------------------------------------- ##
# This vpath is used to get the sources from the chip directory.
vpath %.c . ${CT_ASIC}
vpath %.S . ${CT_ASIC}
ifeq "${CT_ASIC}" "8909"
include src/8909/make.srcs
USE_DIFFERENT_SOURCE_LAYOUT := yes
else
# Include the sources, depending on chips
include ${CT_ASIC}/src/make.srcs
include src/make.srcs
endif

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
