##############################################################################
# Include customvars.mk if it exists
##############################################################################
ifneq "$(wildcard $(SOFT_WORKDIR)/env/compilation/customvars.mk)" ""
  include ${SOFT_WORKDIR}/env/compilation/customvars.mk
endif

ifeq ($(MAKELEVEL),0)
$(info $(shell $(SOFT_WORKDIR)/env/mkconfig.sh $(CT_TARGET)))
endif

ifneq "$(wildcard $(SOFT_WORKDIR)/target/${CT_TARGET}/config.mk)" ""
  include $(SOFT_WORKDIR)/target/${CT_TARGET}/config.mk
endif

ifneq "$(wildcard $(SOFT_WORKDIR)/target/${CT_TARGET}/target.def)" ""
  include $(SOFT_WORKDIR)/target/${CT_TARGET}/target.def
endif
#############################################################################
# inlcude corresponding target_common.def
#############################################################################
ifeq "${TARGET_DIR}" ""
    TARGET_DIR := ${SOFT_WORKDIR}/target
endif
include ${TARGET_DIR}/target_common.def

#############################################################################
# Check if CT_ASIC is correctly defined and include corresponding ${CT_ASIC}.def
# FIXME: That section should disappear for the MMI.
#############################################################################
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/chip/defs/$(CT_ASIC)_$(CT_ASIC_CFG).def)" ""
    $(error "!!!! No ASIC named: CT_ASIC=$(CT_ASIC)_$(CT_ASIC_CFG) !!!!")
else
# Include the ASIC definition
include ${SOFT_WORKDIR}/platform/chip/defs/${CT_ASIC}_${CT_ASIC_CFG}.def
endif

#############################################################################
# Include the file which defines which modules must be used as lib.
#############################################################################
include ${SOFT_WORKDIR}/env/compilation/teamvars.mk
GLOBAL_BINARY_LIBS += ${TEAM_BINARY_LIBS}

#############################################################################
# Include the file which defines some properties for the platform
#############################################################################
ifneq "$(wildcard $(SOFT_WORKDIR)/platform/platform.def)" ""
include $(SOFT_WORKDIR)/platform/platform.def
endif

########################################################################
# MAKELEVEL=0 Things to do only once
# Variables that are defined only once should be in this section and 
# exported to sub-make
########################################################################
ifeq ($(MAKELEVEL),0)
ifeq "${MULTIPLE_APPLICATIONS}" "1"
	SHELL := bash
    APP_PWD := $(shell pwd -P)
	# resolve symlinks
    APP_SWD := $(shell cd ${SOFT_WORKDIR}/application; pwd -P)
    APP_PWD_NO_ROOT := $(patsubst $(APP_SWD)/%,%,$(APP_PWD))
    APP_PWD_NO_ROOT_SPLIT := $(subst /, ,$(APP_PWD_NO_ROOT))
    APP_PWD_NO_ROOT_SPLIT := $(subst \, ,$(APP_PWD_NO_ROOT_SPLIT))
    export CT_APP := $(firstword $(APP_PWD_NO_ROOT_SPLIT))
else  # !MULTIPLE_APPLICATIONS
	export CT_APP := _
endif # !MULTIPLE_APPLICATIONS
endif
########################################################################
# End MAKELEVEL=0 Things to do only once
########################################################################

#############################################################################
# Check if CT_MMI is correctly defined and include corresponding ${CT_MMI}.def
#############################################################################
ifneq "${CT_ERES}" ""
  MMI_CUSTOMER := application/target_res/${CT_ERES}/mmi_customer
  MMI_CONFIG   := application/target_res/${CT_ERES}/mmi_cfg
else #CT_ERES
  ifneq "${CT_MODEM}" "1"
    ifneq "${CT_MODEM}" "2"
      MMI_CUSTOMER := application/mmi_customer
      MMI_CONFIG   := application/mmi_cfg
    endif
  endif
endif #CT_ERES

MMI_DEFAULT_CUSTOMER := application/mmi_customer
include ${SOFT_WORKDIR}/env/compilation/api_platform_depends.mk


ifneq "${CT_MODEM}" "1"
#include ${TARGET_DIR}/${CT_TARGET}/mmi_config.def
include ${SOFT_WORKDIR}/${MMI_CONFIG}/conversion.mk
endif

#############################################################################
# Compile all the lists of flags (except for the local list).
#############################################################################
ALL_EXPORT_FLAG := ${GLOBAL_EXPORT_FLAG} ${TARGET_EXPORT_FLAG} ${CHIP_EXPORT_FLAG}

COMPILEVARS_DEFINED:=yes
