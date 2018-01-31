## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := target

LOCAL_ADD_ABS_INCLUDE := ${TARGET_DIR}/${CT_TARGET}/include

LOCAL_API_DEPENDS :=  platform                          \
                      ${PLATFORM_SYSTEM}/stack                    \
                      ${PLATFORM_SYSTEM}/vpp                      \
                      platform/base/common              \
                      ${PLATFORM_SYSTEM}/calib                    \
                      platform/base/sx                  \
                      platform/base/std                 \
                      platform/chip/hal                 \
                      platform/chip/hal/${CT_ASIC}      \
                      platform/chip/regs                \
                      platform/chip/regs/${CT_ASIC}     \
                      platform/chip/defs                \
                      platform/chip/bcpu                \
                      platform/chip/boot                \
                      platform/chip/pal                 \
                      platform/edrv/pmd                 \
                      platform/edrv/pmd/${PM_MODEL}     \
                      platform/edrv/rfd                 \
                      platform/edrv/memd                \
                      ${PLATFORM_SERVICE}                      \
                      ${PLATFORM_SERVICE}/base                 \
                      platform/edrv/memd/${FLSH_MODEL}  \
                      platform/edrv/lcdd                \
                      ${PLATFORM_SYSTEM}/mdi/                     \
                      ${PLATFORM_SYSTEM}/svc/uat                  \
                      ${PLATFORM_SYSTEM}/svc/urndis                  \
                      ${PLATFORM_SYSTEM}/svc/uctls                \
                      ${PLATFORM_SYSTEM}/svc/umss                 \
                      ${PLATFORM_SYSTEM}/svc/utraces              \
                      ${PLATFORM_SYSTEM}/svc/uvideos              \
                      ${PLATFORM_SYSTEM}/svc/uwifi                \
                      ${PLATFORM_SYSTEM}/svc/ars                  \
                      ${PLATFORM_SYSTEM}/svc/vois                 \
                      platform/edrv/aud                 \
                      platform/edrv/aud/bt              \
                      platform/edrv/aud/codec_gallite   \
                      platform/edrv/aud/fm              \
                      platform/edrv/aud/null            \
                      platform/edrv/aud/ti              \
                      platform/edrv/aud/tv              \
                      platform/edrv/btd/rdabt_adapter \  

ifeq "${CSW_EXTENDED_API_AUDIO_VIDEO}" "1"
LOCAL_API_DEPENDS +=  ${PLATFORM_SYSTEM}/svc/avps
LOCAL_API_DEPENDS +=  ${PLATFORM_SYSTEM}/svc/mps
else
LOCAL_API_DEPENDS +=  ${PLATFORM_SYSTEM}/svc/aps
endif

#camera as optional config:
ifneq (${CAMERA_MODEL},)
LOCAL_API_DEPENDS +=  platform/edrv/camd             \
                      ${PLATFORM_SYSTEM}/svc/cams

ifeq "${CSW_EXTENDED_API_CAMERA}" "1"
LOCAL_API_DEPENDS +=  platform/edrv/camd
endif

LOCAL_EXPORT_FLAG += TGT_WITH_CAMERA
endif

#deprecated MCD_MODEL
ifneq (${MCD_MODEL},)
$(warning MCD_MODEL is deprecated, use MCD_IF instead)
endif # MCD_MODEL

#mcd as optional config:
ifneq (${MCD_IF},)
LOCAL_API_DEPENDS += platform/edrv/mcd                  \
                     platform/edrv/mcd/mcd_${MCD_IF}

LOCAL_EXPORT_FLAG += TGT_WITH_MCD TGT_WITH_MCD_IF_${MCD_IF}
endif # MCD_IF

#fm as optional config:
ifneq (${FM_MODEL},)
ifneq ($(findstring rda5888,${FM_MODEL}), )

LOCAL_API_DEPENDS += platform/edrv/fmd                  \
                     platform/edrv/fmd/rda5888

LOCAL_EXPORT_FLAG += TGT_WITH_FM TGT_WITH_FM_MODEL_${FM_MODEL}

else

LOCAL_API_DEPENDS += platform/edrv/fmd                  \
                     platform/edrv/fmd/${FM_MODEL}

LOCAL_EXPORT_FLAG += TGT_WITH_FM TGT_WITH_FM_MODEL_${FM_MODEL}

endif

endif # FM_MODEL

#gsensor as optional config:
ifneq (${GSENSOR_MODEL},)
LOCAL_API_DEPENDS += platform/edrv/gsensor                  \
                     platform/edrv/gsensor/${GSENSOR_MODEL}

LOCAL_EXPORT_FLAG += TGT_WITH_GSENSOR TGT_WITH_GSENSOR_MODEL_${GSENSOR_MODEL}
endif # GSENSOR_MODEL

#gpiospi as optional config:
ifeq (${GPIOSPI_MODEL}, y)
LOCAL_API_DEPENDS += platform/edrv/gpioex/spigpio                  \
                     platform/edrv/gpioex/spigpio/${GPIOSPI_MODEL}

LOCAL_EXPORT_FLAG += TGT_WITH_GPIOSPI TGT_WITH_GPIOSPI_MODEL_${GPIOSPI_MODEL}
endif # GPIOSPI_MODEL



#gpioi2c as optional config:
ifeq (${GPIOI2C_MODEL}, y)
LOCAL_API_DEPENDS += platform/edrv/gpioex/i2cgpio                  \
                     platform/edrv/gpioex/i2cgpio/${GPIOI2C_MODEL}

LOCAL_EXPORT_FLAG += TGT_WITH_GPIOI2C TGT_WITH_GPIOI2C_MODEL_${GPIOI2C_MODEL}
endif # GPIOI2C_MODEL

#bt as optional config:
ifneq (${BT_MODEL},)
LOCAL_API_DEPENDS += platform/edrv/btd
LOCAL_API_DEPENDS += platform/edrv/btd/rdaCommon/rda_bt_lib

LOCAL_EXPORT_FLAG += TGT_WITH_BT
endif # BT_MODEL

#atv as optional config:
ifneq (${ATV_MODEL},)
LOCAL_API_DEPENDS += platform/edrv/atvd

LOCAL_EXPORT_FLAG += TGT_WITH_ATV TGT_WITH_ATV_MODEL_${ATV_MODEL}
endif # ATV_MODEL

#ts as optional config:
ifneq (${TSD_MODEL},)
LOCAL_API_DEPENDS += platform/edrv/tsd                  \
                     platform/edrv/tsd/${TSD_MODEL}

LOCAL_EXPORT_FLAG += TGT_WITH_TS TGT_WITH_TS_MODEL_${TSD_MODEL}
endif # TSD_MODEL

LOCAL_MODULE_DEPENDS := ${TARGET_MODULE_DEPENDS}

# This is a top-level module
IS_TOP_LEVEL := yes

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Ask target to register the extra modules.
ifeq "${TGT_REGISTER_PLATFORM_VERSIONS}" "1"
LOCAL_EXPORT_FLAG += TGT_REGISTER_PLATFORM_VERSIONS
endif #TGT_REGISTER_PLATFORM_VERSIONS

LOCAL_EXPORT_FLAG += FLASH_BASE=${FLASH_BASE}

ifneq "${STANDALONE_RAMINIT_IMAGE}-${BUILD_RAMINIT_IMAGE}" "1-1"
ifneq "${CT_PRODUCT}" ""
LOCAL_EXPORT_FLAG += CT_PRODUCT=${CT_PRODUCT}
# Always compile the file using CT_PRODUCT
LOCAL_SRC_FORCE_BUILD += target_config.c
endif
ifeq "${CHIP_HAS_AP}" "1"
# Always compile the file saving the build date
LOCAL_SRC_FORCE_BUILD += target_config.c
endif
endif

ifeq "${BATT_CURVE_IN_FACTORY_DATA}" "1"
LOCAL_EXPORT_FLAG += BATT_CURVE_IN_FACTORY_DATA
endif

# Add build time define
LOCAL_EXPORT_FLAG += BUILD_TIME=${BUILD_TIME}

# Add module define
LOCAL_EXPORT_FLAG += CT_TARGET=${CT_TARGET}
LOCAL_EXPORT_FLAG += CT_ASIC=${CT_ASIC}
LOCAL_EXPORT_FLAG += CT_CHIP_DIE=${CT_CHIP_DIE}
LOCAL_EXPORT_FLAG += PA_MODEL=${PA_MODEL}
LOCAL_EXPORT_FLAG += FLSH_MODEL=${FLSH_MODEL}
LOCAL_EXPORT_FLAG += FM_MODEL=${FM_MODEL}
LOCAL_EXPORT_FLAG += ATV_MODEL=${ATV_MODEL}
LOCAL_EXPORT_FLAG += CT_RELEASE=${CT_RELEASE}

# For modules that support driver automatic selection
empty :=
space := $(empty) # end of the line
LOCAL_EXPORT_FLAG += LCD_MODEL=$(subst ${space},/,$(strip ${LCD_MODEL}))
LOCAL_EXPORT_FLAG += CAMERA_MODEL=$(subst ${space},/,$(strip ${CAMERA_MODEL}))
LOCAL_EXPORT_FLAG += BT_MODEL=$(subst ${space},/,$(strip ${BT_MODEL}))


## ------------------------------------------------------------------------- ##
##                         Source files                                      ##
## ------------------------------------------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

