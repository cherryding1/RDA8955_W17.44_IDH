## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_WORKDIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/rfd/xcv_8955

LOCAL_API_DEPENDS := platform \
                     platform/base/common \
                     platform/chip/hal \
                     platform/chip/defs \
                     platform/chip/regs/${CT_ASIC} \
                     platform/edrv \
                     platform/edrv/rfd \
                     platform/base/sx \
                     ${PLATFORM_SYSTEM}/calib \
		     platform/edrv/pmd  

LOCAL_MODULE_DEPENDS := 

ifeq "${USE_EXT_XTAL_32K}" "y"
LOCAL_EXPORT_FLAG += USE_EXT_XTAL_32K
endif

LOCAL_EXPORT_FLAG += RFD_NO_COOLPROFILE  __CALIB_ENABLED__ 

#RFD_CALIB RFD_NO_TRACE RFD_NO_ASSERT


# This vpath is used to get the base/common header file.
vpath %.c . ../src
# ../ would be enough ?

C_SRC := drv_xcv.c
S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
