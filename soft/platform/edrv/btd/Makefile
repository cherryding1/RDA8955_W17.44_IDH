## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/btd

LOCAL_API_DEPENDS := platform \
	 	 platform/base/common \
		 platform/chip/hal \
		 platform/base/sx \
		 platform/base/std \
		 platform/service/base \
		 target

ifneq ($(findstring rda,${BT_MODEL}),)

LOCAL_MODULE_DEPENDS += platform/edrv/btd/rdabt_adapter
LOCAL_MODULE_DEPENDS += platform/edrv/btd/rdabt_driver
LOCAL_LIB += platform/edrv/btd/rdabt_stack/librdabthost.a


LOCAL_EXPORT_FLAG 	 += __BT_RDABT__
endif # rda

ifneq ($(findstring rda586,${BT_MODEL}),)
ifneq ($(findstring rda587,${BT_MODEL}),)
$(error You can NOT put rda586x and rda587x in BT_MODEL list at the same time!!!)
endif # rda587
endif # rda586

ifneq ($(findstring rda5875,${BT_MODEL}),)
ifneq ($(filter rdabt_8809,${BT_MODEL}),)
$(error You can NOT put rda5875 and rdabt_8809 in BT_MODEL list at the same time!!!)
endif # rda587
endif # rda586

ifneq ($(findstring rda586,${BT_MODEL}),)

	LOCAL_EXPORT_FLAG += __RDA_BT_5868_SERIES__
	
	ifneq ($(findstring rda5868e,${BT_MODEL}),)
		LOCAL_EXPORT_FLAG += __RDA_CHIP_R10_5868E__
	endif # rda5868e
	
endif # rda587x

ifneq ($(findstring rda587,${BT_MODEL}),)

	LOCAL_EXPORT_FLAG += __RDA_BT_587x_SERIES__

	ifneq ($(filter rda5870,${BT_MODEL}),)
		LOCAL_EXPORT_FLAG += __RDA_CHIP_R8_5870__
	endif # rda5870

	ifneq ($(filter rda5872,${BT_MODEL}),)
		LOCAL_EXPORT_FLAG += __RDA_CHIP_R10_5872__
	endif # rda5872

	ifneq ($(filter rda5872p,${BT_MODEL}),)
		LOCAL_EXPORT_FLAG += __RDA_CHIP_R11_5872P__
	endif # rda5872p

	ifneq ($(filter rda5872h,${BT_MODEL}),)
		LOCAL_EXPORT_FLAG += __RDA_CHIP_R10_5872H__
	endif # rda5872h

endif # rda587x

ifneq ($(findstring rda5875,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_BT_5875_SERIES__
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R11_5875__
endif # rda5872

ifneq ($(findstring rda5876,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_BT_5875_SERIES__
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R12_5876__
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R16_5876P__
endif # rda5876

ifneq ($(findstring rda5876m,${BT_MODEL}),)
LOCAL_EXPORT_FLAG += __RDA_BT_5876M_SERIES__
LOCAL_EXPORT_FLAG += __RDA_CHIP_R12_5876M__
endif # rda5876m

ifneq ($(findstring rda599,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_BT_587x_SERIES__	
ifneq ($(findstring rda5990,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_BT_5875_SERIES__
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R12_5990__
endif # rda5990
endif #rda599x

ifneq ($(filter rdabt_8809,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R18_8809E__	
	LOCAL_EXPORT_FLAG += __RDA_BT_587x_SERIES__	
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R17_8809__
endif #rda599x

ifneq ($(filter rdabt_8809e,${BT_MODEL}),)
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R18_8809E__	
	LOCAL_EXPORT_FLAG += __RDA_BT_587x_SERIES__	
	LOCAL_EXPORT_FLAG += __RDA_CHIP_R17_8809__
endif #rda599x

ifeq ($(BT_SHARE_CRYSTAL),y)
	LOCAL_EXPORT_FLAG += __RDA_SHARE_CRYSTAL__
endif # BT_CRYSTAL_SHARED

# This is a top-level module
IS_TOP_LEVEL = yes

# This needs to deliver environment related files also
DELIV_ENV_FILES := 0

ifeq "${RELEASE_MODULE}" "1"
# In case of LIB_RELEASE, list all the modules
ifneq (${BT_MODEL},)
LOCAL_DEPEND_LIBS := \
			platform/edrv/btd/rdaCommon \
			
endif # (${BT_MODEL},)
endif #RELEASE_MODULE

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generates the CoolProfile headers automatically.
#AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

empty :=
space := $(empty) # end of the line
# Compilation mode for the version string.
LOCAL_EXPORT_FLAG += BTD_BT_MODEL=$(subst ${space},/,$(strip ${BT_MODEL}))

# 32K clock connection
ifeq "${BT_USE_32K_CLK_PIN}" "1"
LOCAL_EXPORT_FLAG += BT_USE_32K_CLK_PIN
endif

ifeq ($(findstring __8890P_COMBIAN_BTWIFI_WITH_26M_CONTROLLER__,${GLOBAL_EXPORT_FLAG}),)
# Break interrupt wakeup mode
ifeq "${BT_UART_BREAK_INT_WAKEUP}" "1"
LOCAL_EXPORT_FLAG += BT_UART_BREAK_INT_WAKEUP
endif
endif

# Macro for BT stack build
LOCAL_EXPORT_FLAG += COMBINED_HOST=0 \
                     PRH_BS_CFG_SYS_TRACE_DEBUG_VIA_UART_SUPPORTED=0 \
                     _REENTRANT \
                     COOLSAND

# Macros for different CT_RELEASE. Should be in env, put here temporarily
ifeq ("$(CT_RELEASE)", "debug")
LOCAL_EXPORT_FLAG += RDA_BT_DEBUG
LOCAL_EXPORT_FLAG += DEBUG=1
else
LOCAL_EXPORT_FLAG += DEBUG=0
endif

# Flags exported throughout the build


## ------------------------------------------------------------------------- ##
##	                       Source files                                      ##
## ------------------------------------------------------------------------- ##
# No source files in the top-level itself...
S_SRC :=
C_SRC := btd_map.c
INPUT_FILES :=  

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


