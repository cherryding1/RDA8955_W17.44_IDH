## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/camd/gc6103forgc0329

LOCAL_API_DEPENDS ?= \
    target \
	platform \
	platform/base/common \
	platform/base/sx \
	platform/chip/hal \
	platform/chip/defs \
	platform/edrv/camd \
	platform/edrv/pmd \
	platform/edrv/camd \
	platform/edrv/analogtvd \
	platform/edrv/i2cgpio \
	

C_SRC := ${notdir ${wildcard src/*.c}}
S_SRC := ${notdir ${wildcard src/*.S}}


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

