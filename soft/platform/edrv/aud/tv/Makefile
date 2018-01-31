## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/aud/tv

LOCAL_API_DEPENDS := \
					platform \
					platform/edrv \
					platform/base/common \
					platform/base/sx \
					platform/chip/hal \
					platform/chip/defs \
					platform/edrv/aud \
					platform/edrv/aud/codec_gallite \
					${PLATFORM_SYSTEM}/calib \

ifeq "${TV_USING_I2S}" "y"
LOCAL_EXPORT_FLAG += TV_USING_I2S
endif

## Include source files
include src/make.srcs

# C_SRC := ${notdir ${wildcard *.c}}
# S_SRC := ${notdir ${wildcard *.S}}


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

