## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/syscmds

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform                      \
                     platform/base/sx              \
                     platform/base/common          \
                     ${PLATFORM_SYSTEM}/calib                \
                     platform/chip/defs            \
                     platform/chip/hal             \
                     platform/chip/hal/${CT_ASIC}  \
                     platform/edrv/aud             \
                     platform/edrv/aud/codec_gallite \
                     platform/edrv/i2cgpio         \
                     platform/edrv/pmd             \
                     ${PLATFORM_SYSTEM}/svc                  \
                     ${PLATFORM_SYSTEM}/svc/vois             \
                     platform/edrv/tsd             \
                     target                        \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=

ifeq "${WITH_GIT}" "1"
LOCAL_EXPORT_FLAG += WITH_GIT
endif
ifeq "${TSD_MODEL}" ""
LOCAL_EXPORT_FLAG += _TGT_MODEM_NO_TS_
endif
ifneq "${I2C_MODEL}" ""
LOCAL_EXPORT_FLAG += GPIO_I2C
endif

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
