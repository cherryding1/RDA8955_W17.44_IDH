## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/aud/codec_gallite

LOCAL_API_DEPENDS := \
		    target \
		    platform \
		    platform/base/common \
		    platform/base/sx \
		    ${PLATFORM_SYSTEM}/calib \
		    platform/chip/hal \
		    platform/chip/hal/${CT_ASIC} \
		    platform/chip/defs \
		    platform/edrv \
		    platform/edrv/aud \
		    platform/edrv/pmd \

ifneq "$(strip ${BT_MODEL})" ""
LOCAL_API_DEPENDS += \
                     platform/edrv/btd/rdabt_adapter
endif


## Include source files
include src/make.srcs

# C_SRC := ${notdir ${wildcard *.c}}
# S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

