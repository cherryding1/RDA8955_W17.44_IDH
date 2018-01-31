## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/tsd/bl6130

LOCAL_API_DEPENDS ?= platform \
                     platform/chip/hal \
					 platform/chip/regs/$(CT_ASIC)   \
                     platform/chip/defs \
                     platform/edrv/lcdd \
                     platform/edrv/tsd \
                     platform/edrv \
                     target \
                     platform/base/sx \
                     platform/edrv/gpioex/i2cgpio \
                     platform/edrv/pmd

C_SRC := tsd.c bl_fw.c fw_update.c
S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
