# Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/fota/fota_core

LOCAL_API_DEPENDS := platform


ifeq ($(findstring ${PLATFORM_SERVICE}/fota/fota_core/packed,${GLOBAL_BINARY_LIBS}),)

IS_TOP_LEVEL := no
C_SRC := $(notdir $(wildcard src/*.c))
LOCAL_LIB_INCR_LINK := yes

else # use packed lib

IS_TOP_LEVEL := yes
LOCAL_LIBS += $(LOCAL_NAME)/packed/lib/libfota_core_$(CT_RELEASE).a

endif

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
