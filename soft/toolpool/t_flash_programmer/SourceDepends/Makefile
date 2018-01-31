## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := toolpool/t_flash_programmer/SourceDepends

# list all modules APIs that are necessary to compile this module
LOCAL_API_DEPENDS := \
                    platform \
                    platform/base/common \
                    ${PLATFORM_SYSTEM}/calib \
                    ${PLATFORM_SERVICE} \
                    ${PLATFORM_SERVICE}/driver \
                    ${PLATFORM_SERVICE}/base \
                    ${PLATFORM_SERVICE}/base/tm \
                    platform/chip/bcpu \
                    platform/chip/hal \
                    platform/chip/defs \
                    platform/chip/regs/${CT_ASIC} \
                    platform/chip/boot/${CT_ASIC} \
                    platform/chip/pal \
                    platform/chip/patch \
                    platform/edrv/dualsimd \
                    platform/edrv/lcdd \
                    platform/edrv/mcd \
                    platform/edrv/memd \
                    platform/edrv/pmd \
                    platform/edrv/rfd \
                    ${PLATFORM_SYSTEM}/stack \
                    target \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \
		platform/base/dbg \
		platform/base/std \
		platform/base/sx \
		platform/chip/boot\
		platform/chip/hal \
		${PLATFORM_SERVICE}/base\
		${PLATFORM_SERVICE}/driver\
		${PLATFORM_SERVICE}/tools \
		${PLATFORM_SYSTEM}/svc/fmg \

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
#S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
#C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files
S_SRC := 
C_SRC := 

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += \
	HAVE_CFW_CONFIG \
	MF32_BASE_REG_DISABLE \
	MF32_NO_RFM_RH \
	MF32_NO_RFM \
	MF32_CFW_MMI_TWO_TASKS \
	CPP_KEYWORD_SUPPORT
	
LOCAL_EXPORT_FLAG += _T_UPGRADE_PROGRAMMER

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
