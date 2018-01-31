## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/memd/flash_sram_drv

LOCAL_API_DEPENDS := \
					platform \
					platform/base/common \
					platform/chip/hal \
					platform/chip/regs/${CT_ASIC} \
					platform/chip/defs \
					platform/edrv/memd \
					platform/edrv \
					platform/base/sx \
					platform/base/std \
					platform/edrv/pmd \
					target \

C_SRC := drv_flsh.c  drv_ddr.c ddr_config.c
S_SRC :=
%/ddr_config.o: CT_MIPS16_CFLAGS=
%/ddr_config.o: MYCFLAGS=-O2

LOCAL_EXPORT_FLAG += USER_DATA_BLOCK_SIZE=${USER_DATA_BLOCK_SIZE}
LOCAL_EXPORT_FLAG += FLASH_BASE=${FLASH_BASE}
## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
