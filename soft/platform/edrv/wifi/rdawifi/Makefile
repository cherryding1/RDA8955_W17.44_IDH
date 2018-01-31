## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/wifi/rdawifi

LOCAL_API_DEPENDS := \
                     platform \
                     ${PLATFORM_SERVICE} \
                     platform/chip/hal \
                     platform/chip/hal/include \
                     platform/chip/defs \
                     platform/edrv/wifi \
                     platform/edrv \
                     platform/edrv/pmd \
                     platform/chip/regs/$(CT_ASIC)\
                     target \
                     platform/base/sx


ifneq "${WIFI_CHIP}"  "5991h"
C_SRC :=  sdio.c wifi_init.c wifi_test.c rda5890_scan.c
else
C_SRC :=  sdio_91h.c wifi_init_91h.c wifi_test_91h.c rda5890_scan_91h.c wifi_fw_download.c
LOCAL_OBJS += code_rda5991h.bin.o  data_rda5991h.bin.o
endif

S_SRC :=

## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
