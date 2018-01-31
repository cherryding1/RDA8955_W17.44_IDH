## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform

# Compile the sub-modules, except when the "platform" must be used as a library.
ifeq ($(findstring platform/packed,${GLOBAL_BINARY_LIBS}),)

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
                        platform/base \
                        ${PLATFORM_SYSTEM}/calib \
                        platform/chip \
                        platform/edrv \
                        ${PLATFORM_SERVICE}

ifneq "${CHIP_HAS_AP}" "1"
ifneq "${PLATFORM_WITH_MDI}" "no"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi
endif
endif

LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/stack \
                        ${PLATFORM_SYSTEM}/svc \
                        ${PLATFORM_SYSTEM}/vpp \
                        target \

ifeq "${LTE_NBIOT_SUPPORT}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/nbiot
endif

else # GLOBAL_BINARY_LIBS

LOCAL_BINARY_LIBS    += platform/packed/platform_${CT_TARGET}
LOCAL_MODULE_DEPENDS := platform/packed/platform_${CT_TARGET}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

# This module is a packed module.
BINLIB_PATH      := ${SOFT_WORKDIR}/platform/packed/platform_${CT_TARGET}/lib
LOCAL_BINLIBRARY := ${BINLIB_PATH}/libplatform_${CT_TARGET}_${CT_RELEASE}.a

## ------------------------------------------------------------------------- ##
# Documentation configuration
## ------------------------------------------------------------------------- ##

EXTRA_DOC_INPUTS +=     platform/base/common
EXTRA_DOC_INPUTS +=     platform/base/dbg
EXTRA_DOC_INPUTS +=     platform/base/sap
EXTRA_DOC_INPUTS +=     platform/base/std
#EXTRA_DOC_INPUTS +=     platform/base/sx        # Not documented in Doxygen fmt.
#EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/stack          # Not documented in Doxygen fmt.
#EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/stack/xxx      # Not documented in Doxygen fmt.
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/fmg         # Only for test purpose.
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/imgs        # Will be soon deprecated.
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/mps
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/mrs
#EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/gls        # Not ready yet.
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/uctls
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/umss
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/utraces

DOC_ADD_INTERNAL_MODULES ?= 1

ifeq "${DOC_ADD_INTERNAL_MODULES}" "1"
# Internal modules.
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/calib
EXTRA_DOC_INPUTS +=     platform/chip/bcpu/spal
EXTRA_DOC_INPUTS +=     platform/chip/bcpu/spc
EXTRA_DOC_INPUTS +=     platform/chip/bcpu/spp
EXTRA_DOC_INPUTS +=     platform/chip/defs
EXTRA_DOC_INPUTS +=     platform/chip/hal
EXTRA_DOC_INPUTS +=     platform/chip/boot
EXTRA_DOC_INPUTS +=     platform/chip/pal
EXTRA_DOC_INPUTS +=     platform/edrv
EXTRA_DOC_INPUTS +=     platform/edrv/aud
EXTRA_DOC_INPUTS +=     platform/edrv/btd        # API needs to be reworked.
EXTRA_DOC_INPUTS +=     platform/edrv/camd
EXTRA_DOC_INPUTS +=     platform/edrv/fmd
EXTRA_DOC_INPUTS +=     platform/edrv/lcdd
EXTRA_DOC_INPUTS +=     platform/edrv/mcd
EXTRA_DOC_INPUTS +=     platform/edrv/memd
EXTRA_DOC_INPUTS +=     platform/edrv/pmd
EXTRA_DOC_INPUTS +=     platform/edrv/simd
EXTRA_DOC_INPUTS +=     platform/edrv/rfd
EXTRA_DOC_INPUTS +=     platform/edrv/tsd
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/avps
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/avrs
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/svc/vois
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/amjp
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/amjr
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/decoder
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/h263_dec
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/mpeg4_dec
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/speech
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/speech/romz
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/speech/soft
EXTRA_DOC_INPUTS +=     ${PLATFORM_SYSTEM}/vpp/speech/std
endif # "${DOC_ADD_INTERNAL_MODULES}" "1"

DOCUMENT_DEPENDENCIES := 1

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

# Copying the Def files and the ROM useful when using the Platform as lib.
ifneq "$(wildcard $(PLATFORM_LIB_DEST))" ""

copydefs: force
	@${ECHO} "COPYDEFS          $(PLATFORM_LIB_DEST)"
	mkdir -p $(PLATFORM_LIB_DEST)/chip/defs
	mkdir -p $(PLATFORM_LIB_DEST)/chip/defs/${CT_ASIC}
	# We copy all Def files because of inclusions.
	cp ${SOFT_WORKDIR}/platform/chip/defs/${CT_ASIC}_*.def \
		$(PLATFORM_LIB_DEST)/chip/defs
	cp ${SOFT_WORKDIR}/platform/chip/defs/${CT_ASIC}/rom_symbols_to_strip \
		$(PLATFORM_LIB_DEST)/chip/defs/${CT_ASIC}
	mkdir -p $(PLATFORM_LIB_DEST)/edrv/memd/$(FLSH_MODEL)
	cp $(SOFT_WORKDIR)/platform/edrv/memd/$(FLSH_MODEL)/memd.def \
		$(PLATFORM_LIB_DEST)/edrv/memd/$(FLSH_MODEL)
	mkdir -p $(PLATFORM_LIB_DEST)/chip/rom/${CT_ASIC}/lib
	cp ${SOFT_WORKDIR}/platform/chip/rom/${CT_ASIC}/lib/mem_bridge_rom_CHIP.elf \
		$(PLATFORM_LIB_DEST)/chip/rom/${CT_ASIC}/lib

endif # "$(wildcard $(PLATFORM_LIB_DEST))" ""

