## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/aud/bt

LOCAL_API_DEPENDS := \
					platform \
					platform/edrv \
					platform/base/common \
					${PLATFORM_SYSTEM}/calib\
					platform/base/sx \
					platform/chip/hal \
					platform/chip/defs \
					platform/edrv/aud \
					${PLATFORM_SYSTEM}/svc/vois \
					platform/edrv/aud/codec_gallite \


## Include source files
include src/make.srcs

# C_SRC := ${notdir ${wildcard *.c}}
# S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

