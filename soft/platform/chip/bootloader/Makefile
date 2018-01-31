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
##  Include Stack Dependency Macro       ##
## ------------------------------------- ##

# Name of the module
LOCAL_NAME := platform/chip/bootloader

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/chip/hal \
	platform/chip/defs \
	platform/chip/boot \
	platform/chip/regs/$(CT_ASIC) \
	platform/edrv/pmd


LOCAL_MODULE_DEPENDS := platform/chip/bootloader/$(CT_ASIC)

LOCAL_MODULE_DEPENDS += platform/service/spiffs/spiffs_core

# This is a top-level module
IS_TOP_LEVEL := yes

C_SRC := $(notdir $(wildcard src/*.c))
S_SRC := $(notdir $(wildcard src/*.S))
C++_SRC := $(notdir $(wildcard src/*.cpp))

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
