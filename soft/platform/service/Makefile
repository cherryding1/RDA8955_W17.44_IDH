## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}

# Compile the sub-modules, except when the "csw" must be used as a library.
ifeq ($(findstring ${PLATFORM_SERVICE}/packed,${GLOBAL_BINARY_LIBS}),)

# list all modules APIs that are necessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/chip/hal \
					platform/base/common \
					platform/chip/defs \
					${PLATFORM_SYSTEM}/stack \

ifeq "${_AT_USB_MODEM_NO_MMMI_}" "y"
LOCAL_API_DEPENDS += ${PLATFORM_SYSTEM}/svc/uat
endif

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \
			${PLATFORM_SERVICE}/base \
			${PLATFORM_SERVICE}/cfw \
			${PLATFORM_SERVICE}/dm \
			${PLATFORM_SERVICE}/pm \
			${PLATFORM_SERVICE}/tools \
			${PLATFORM_SERVICE}/stub \
			${PLATFORM_SERVICE}/mci \
			${PLATFORM_SERVICE}/net \
			${PLATFORM_SERVICE}/fota \
			$(PLATFORM_SERVICE)/spiffs

LOCAL_MODULE_DEPENDS += $(PLATFORM_SERVICE)/nanopb

ifeq "${CHIP_HAS_AP}" "1"
# CT_MODEM is forced to 1
override CT_MODEM := 1
# No flash operation in FS/REG/SMS/DSM
LOCAL_EXPORT_FLAG += _DSM_SIMUFLASH_MEMORY
# No t-card support on modem side
LOCAL_EXPORT_FLAG += MMC_SIMU_NULL
else
LOCAL_MODULE_DEPENDS += ${PLATFORM_SERVICE}/driver
endif

ifeq "${IOT_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += IOT_SUPPORT
endif

ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SERVICE}/wifi
endif

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes
ifeq "$(strip ${PLATFORM_SERVICE})" "platform/service"
C_SRC += service_map.c
else
C_SRC += csw_map.c
endif
## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += \
	HAVE_CFW_CONFIG \
	MF32_BASE_REG_DISABLE \
	MF32_NO_RFM_RH \
	MF32_NO_RFM \
	MF32_CFW_MMI_TWO_TASKS \
	CPP_KEYWORD_SUPPORT \

# A mechanism to disable the trace compression.
ifndef SXS_NO_STR_TO_ID
LOCAL_EXPORT_FLAG += __SXS_STR_TO_ID__
endif

ifeq "${CALL_AMR_SUPPORT}" "y"	
LOCAL_EXPORT_FLAG += CFW_AMR_SUPPORT
endif

ifeq "${GPRS_SUPPORT}" "y"
ifneq "${USE_KCONFIG}" "y"
LOCAL_EXPORT_FLAG += CFW_GPRS_SUPPORT
ifeq "${PPP_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += CFW_PPP_SUPPORT
endif
endif
endif

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

ifeq "${CT_MODEM}" "2"
LOCAL_EXPORT_FLAG += AT_MMI_SUPPORT
endif

ifeq "${CT_MODEM}" "1"
LOCAL_EXPORT_FLAG += ONLY_AT_SUPPORT
endif

ifneq "${CT_MODEM}" "2"
ifneq "${CT_MODEM}" "1"
LOCAL_EXPORT_FLAG += ONLY_MMI_SUPPORT
endif
endif

else # GLOBAL_BINARY_LIBS

SIM_STEMS := _singlesim _dualsim _threesim _foursim

PS_STEM :=
ifeq "${GPRS_SUPPORT}" "y"
PS_STEM := _egprs
endif
ifeq "${EGPRS_SUPPORT}" "y"
PS_STEM := _egprs
endif

PACKED_LIB := $(notdir ${LOCAL_NAME})$(word ${NUMBER_OF_SIM},${SIM_STEMS})${PS_STEM}

ifeq "${CHIP_HAS_AP}" "1"
PACKED_LIB_PATH := ${LOCAL_NAME}/packed/smartphone/${PACKED_LIB}
else
PACKED_LIB_PATH := ${LOCAL_NAME}/packed/${CT_ASIC}/${PACKED_LIB}
endif

LOCAL_BINARY_LIBS    += ${PACKED_LIB_PATH}
LOCAL_MODULE_DEPENDS := ${PACKED_LIB_PATH}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
