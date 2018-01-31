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

## ------------------------------------- ##
##	Include Stack Dependency Macro   ##
## ------------------------------------- ##

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/fota

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/base/sx \
	platform/service \
	platform/service/net \
	platform/chip/hal \
	platform/chip/defs \
	platform/service/spiffs

LOCAL_MODULE_DEPENDS := $(PLATFORM_SERVICE)/fota/fota_core

# This is a top-level module
IS_TOP_LEVEL := yes

# include src/make.srcs
C_SRC := fota_helper.c

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
