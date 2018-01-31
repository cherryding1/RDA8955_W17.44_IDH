## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv

# Compile the sub-modules, except when the "chip" must be used as a library.
ifeq ($(findstring platform/edrv/packed,${GLOBAL_BINARY_LIBS}),)

# When releaselib_all.sh is used to update all libs, no need to compile edrv except rda btd.

LOCAL_MODULE_DEPENDS := 

ifneq (${NO_EDRV},)

ifneq "${CHIP_HAS_AP}" "1"
LOCAL_MODULE_DEPENDS += platform/edrv/btd
endif

else # !NO_EDRV

LOCAL_MODULE_DEPENDS += \
                        platform/edrv/aud \
                        platform/edrv/dualsimd \
                        platform/edrv/gpioex \
                        platform/edrv/memd \
                        platform/edrv/pmd \
                        platform/edrv/rfd \
                        platform/edrv/tsd

ifneq "${CHIP_HAS_AP}" "1"
LOCAL_MODULE_DEPENDS += \
                        platform/edrv/atvd \
                        platform/edrv/fmd \
                        platform/edrv/gpioex \
                        platform/edrv/gsensor \
                        platform/edrv/lcdd \
                        platform/edrv/mcd \
                        platform/edrv/cammd

ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_MODULE_DEPENDS += platform/edrv/wifi
endif

ifneq "${BT_MODEL}" " "
LOCAL_MODULE_DEPENDS += platform/edrv/btd
endif

ifneq "${CSW_EXTENDED_API_CAMERA}" "1"
LOCAL_MODULE_DEPENDS += platform/edrv/camd
else
LOCAL_MODULE_DEPENDS += platform/edrv/camd
endif

ifneq "${GPSD_MODEL}" ""
LOCAL_MODULE_DEPENDS += platform/edrv/gpsd
endif

endif # CHIP_HAS_AP != 1

endif # !NO_EDRV

else # GLOBAL_BINARY_LIBS

LOCAL_BINARY_LIBS    += platform/edrv/packed/edrv_${CT_TARGET}
LOCAL_MODULE_DEPENDS := platform/edrv/packed/edrv_${CT_TARGET}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

# This module is a packed module.
BINLIB_PATH      := ${SOFT_WORKDIR}/platform/edrv/packed/edrv_${CT_TARGET}/lib
LOCAL_BINLIBRARY := ${BINLIB_PATH}/libedrv_${CT_TARGET}_${CT_RELEASE}.a

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
