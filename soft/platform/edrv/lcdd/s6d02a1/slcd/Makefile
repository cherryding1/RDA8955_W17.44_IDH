## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/lcdd/s6d02a1/slcd

LOCAL_API_DEPENDS := \
        platform \
        platform/base/common \
        platform/chip/hal \
        platform/edrv/lcdd \
        platform/edrv/lcdd/s6d02a1 \
        platform/edrv/pmd \
        platform/edrv \
        target \
        platform/base/sx \
        platform/chip/defs

LOCAL_ADD_INCLUDE := platform/edrv/lcdd/src

C_SRC := ${notdir ${wildcard src/*.c}}
S_SRC :=


## --------------Don't change the line below------------------ ##

include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
