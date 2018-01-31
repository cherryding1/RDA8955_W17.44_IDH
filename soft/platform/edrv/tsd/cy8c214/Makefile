## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/tsd/cy8c214

LOCAL_API_DEPENDS ?= \
    target \
	platform \
	platform/base/common \
	platform/base/sx \
	platform/chip/hal \
	platform/chip/defs \
	platform/edrv/tsd \
	platform/edrv/pmd \
        platform/edrv \
	platform/edrv/tsd/src \
	platform/edrv/camd \
	platform/edrv/analogtvd \

C_SRC := tsd.c gpio_iic_pas_sensor.c
S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
