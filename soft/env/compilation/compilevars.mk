##############################################################################
# Include customvars.mk if it exists
##############################################################################

ifneq "$(IGNORE_CUSTOM_VARS)" "1"
  ifneq "$(wildcard $(SOFT_WORKDIR)/env/compilation/customvars.mk)" ""
    include ${SOFT_WORKDIR}/env/compilation/customvars.mk
  endif
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
# inlcude corresponding target.def
#############################################################################
ifeq "${CT_SP}" "1"
  TARGET_DIR := ${SOFT_WORKDIR}/target-sp
else
  ifeq "${TARGET_DIR}" ""
    TARGET_DIR := ${SOFT_WORKDIR}/target
  endif
endif

ifeq "$(wildcard ${TARGET_DIR}/target_common.def)" ""
  $(error "!!!! CT_TARGET not valid - TARGET_DIR=${TARGET_DIR} or CT_TARGET=${CT_TARGET} workdir =${SOFT_WORKDIR}is not supported or not well defined !!!!")
else
  include ${TARGET_DIR}/target_common.def
endif

#############################################################################
# Check if CT_ASIC is correctly defined and include corresponding ${CT_ASIC}.def
#############################################################################
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/chip/defs/$(CT_ASIC)_$(CT_ASIC_CFG).def)" ""
  $(error "!!!! No ASIC named: CT_ASIC=$(CT_ASIC)_$(CT_ASIC_CFG) !!!!")
else
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

include ${SOFT_WORKDIR}/env/compilation/api_platform_depends.mk
#############################################################################
# Compile all the lists of flags (except for the local list).
#############################################################################

ifeq "${SPI_REG_DEBUG}" "y"
TARGET_EXPORT_FLAG += SPI_REG_DEBUG
endif

ifeq "${I2C_REG_DEBUG}" "y"
TARGET_EXPORT_FLAG += I2C_REG_DEBUG
endif

ALL_EXPORT_FLAG := ${GLOBAL_EXPORT_FLAG} ${TARGET_EXPORT_FLAG} ${CHIP_EXPORT_FLAG}

COMPILEVARS_DEFINED:=yes
