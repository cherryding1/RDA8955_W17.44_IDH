## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/usbhost/usbhost_mass

LOCAL_API_DEPENDS := \
                     platform \
                     platform/chip/hal \
                     platform/chip/defs \
					 platform/chip/regs/8809p \
                     platform/edrv/usbhost \
                     platform/edrv \
                     platform/edrv/pmd \
                     target \
                     platform/base/sx  \
					 ${PLATFORM_SYSTEM}/svc/uctls \


C_SRC :=   mcd_usb.c usb_hcd.c usbmsc.c scsi2.c bot.c cbit.c
S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
