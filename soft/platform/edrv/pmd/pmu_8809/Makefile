## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/pmd/pmu_8809

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
                     ${PLATFORM_SYSTEM}/calib \
                     platform/base/common \
                     platform/base/sx \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/chip/boot \
                     platform/chip/hal/${CT_ASIC} \
                     platform/chip/regs/${CT_ASIC} \
                     platform/edrv \
                     platform/edrv/dualsimd \
                     platform/edrv/pmd \
                     platform/edrv/rfd \
                     target \

LOCAL_ADD_INCLUDE := platform/edrv/pmd/src


# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 

ifeq "$(strip ${FM_MODEL})" "rdafm_8809"
LOCAL_EXPORT_FLAG += RDAFM_8809
endif

ifeq "$(strip ${FM_MODEL})" "rdafm_8809e"
LOCAL_EXPORT_FLAG += RDAFM_8809
endif

ifeq "$(strip ${BT_MODEL})" "rdabt_8809"
LOCAL_EXPORT_FLAG += RDABT_8809
endif

ifeq "${LCD_BACKLIGHT_CHARGE_PUMP}" "y"
LOCAL_EXPORT_FLAG += LCD_BACKLIGHT_CHARGE_PUMP=1
else
LOCAL_EXPORT_FLAG += LCD_BACKLIGHT_CHARGE_PUMP=0
endif

ifeq "${AUD_SPK_LINE_USAGE}" "left"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_ONLY=1 AUD_SPK_LINE_R_ONLY=0
else
ifeq "${AUD_SPK_LINE_USAGE}" "right"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_ONLY=0 AUD_SPK_LINE_R_ONLY=1
else
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_ONLY=0 AUD_SPK_LINE_R_ONLY=0
endif
endif

# The default value of L&R double mode depends on class modes
ifeq "${AUD_SPK_LINE_L_R_DOUBLE}" "y"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_R_DOUBLE=1
else
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_R_DOUBLE=0
endif

ifeq "${AUD_SPK_LINE_CLASS_MODE}" "class_ab"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_CLASS_AB=1 AUD_SPK_LINE_R_CLASS_AB=1 AUD_SPK_LINE_NO_CLASS_K=1
endif
ifeq "${AUD_SPK_LINE_CLASS_MODE}" "class_d"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_CLASS_AB=0 AUD_SPK_LINE_R_CLASS_AB=0 AUD_SPK_LINE_NO_CLASS_K=1
endif
ifeq "${AUD_SPK_LINE_CLASS_MODE}" "class_k"
LOCAL_EXPORT_FLAG += AUD_SPK_LINE_L_CLASS_AB=0 AUD_SPK_LINE_R_CLASS_AB=0 AUD_SPK_LINE_NO_CLASS_K=0
endif

LOCAL_EXPORT_FLAG += 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

## ------------------------------------- ##
##      List all your sources here       ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


