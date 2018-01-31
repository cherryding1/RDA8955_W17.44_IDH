##############################################################################
#  Coolsand main compilation rules file
##############################################################################
# behavior of compilation can be affected by the following:
# 	customvars.mk : this file included at the beginning of all Makefile
# 		allows to define variables that will modify the behavior of make
# 		ex: GLOBAL_BINARY_LIBS += ${PLATFORM_SYSTEM}/stack will prevent recompilation 
# 		    of the stack and use the lib version instead
# 		    GLOBAL_EXPORT_FLAG += MY_FLAG=my_value will define the flag MY_FLAG 
# 		    at the my_value value in all calls to make
#
# 	cutomrules.mk: this file is included at the end of compilerules.mk
# 		It allows to add rules that are only usefull for a specific team or 
# 		individual
# 		ex: msim: prerequisite
# 				rule_action
# 			this rule used to use a simulator to simulate the HW is only usefull 
# 			for ASIC teams as they have access to the HW source code and simulator
#
#   For explanations about the Makefile variables, please see the SoftEnv 
#   manual in the wiki.
#

# Cancel most of the default suffix/implicit rules (To cancel all of them,
# we have to use the make option -r or --no-builtin-rules)
#.SUFFIXES: .hex .o .c .asm .S .h
.SUFFIXES:

# Do not treat all targets as intermediate files
#.SECONDARY:

########################################################################
# MAKELEVEL=0 Things to do only once
# Variables that are defined only once should be in this section and 
# exported to sub-make
########################################################################
ifeq ($(MAKELEVEL),0)

# force the local to be the default 'C' local in english, this really helps when parsing outputs
export LANG=C

########################################################################
#check that all the parameters are OK
########################################################################
# Checking that env file customvars.mk exists
# Checking that 3 main variables are well defined
# 		CT_RELEASE
# 		CT_ASIC
# 		CT_TARGET
# Checking that drivers exist for FLSH_MODEL, XCV_MODEL, PA_MODEL, SW_MODEL


# Check if CT_RELEASE is defined and valid
VALID_RELEASE_LIST :=release debug calib sa_profile cool_profile custom
SELECTED_RELEASE := $(filter $(VALID_RELEASE_LIST), $(CT_RELEASE))
ifeq "$(SELECTED_RELEASE)" ""
    $(error "!!!! CT_RELEASE=${CT_RELEASE} - Not a valid release type !!!!")
endif

# Check if a driver exists for the specified FLSH_MODEL
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/edrv/memd/$(FLSH_MODEL))" ""
    $(error "\n!!!! FLSH_MODEL=$(FLSH_MODEL) not valid - No driver exist !!!!")
endif

# Check if a driver exists for the specified XCV_MODEL
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/edrv/rfd/$(XCV_MODEL))" ""
    $(error "\n!!!! XCV_MODEL=$(XCV_MODEL) not valid - No driver exist !!!!")
endif

# Check if a driver exists for the specified PA_MODEL
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/edrv/rfd/$(PA_MODEL))" ""
    $(error "\n!!!! PA_MODEL=$(PA_MODEL) not valid - No driver exist !!!!")
endif

# Check if a driver exists for the specified SW_MODEL
ifeq "$(wildcard $(SOFT_WORKDIR)/platform/edrv/rfd/$(SW_MODEL))" ""
    $(error "\n!!!! SW_MODEL=$(SW_MODEL) not valid - No driver exist !!!!")
endif

# Check if a driver exists for the specified CT_RESGEN
#VALID_YESNO_LIST := YES yes Yes NO no No
VALID_YESNO_LIST := yes no
SELECTED_RESGEN := $(filter $(VALID_YESNO_LIST), $(CT_RESGEN))
ifeq "$(SELECTED_RESGEN)" ""
    ifneq "${CT_RESGEN}" ""
        $(error "!!!! CT_RESGEN=${CT_RESGEN} - Not a valid resource generation option !!!!")
    endif
endif

SELECTED_USERGEN := $(filter $(VALID_YESNO_LIST), $(CT_USERGEN))
ifeq "$(SELECTED_USERGEN)" ""
    ifneq "${CT_USERGEN}" ""
        $(error "!!!! CT_USERGEN=${CT_USERGEN} - Not a valid user sector generation option !!!!")
    endif
endif

export RESGEN := no
export USERGEN := no
ifneq "${CT_RAMRUN}" "1"
# Check if "RESGEN" is needed
ifeq "${CT_RESGEN}" "yes"
    RESGEN := yes
endif
# Check if "USERGEN" is needed
ifeq "${CT_USERGEN}" "yes"
    USERGEN := yes
endif
endif # !CT_RAMRUN

export BCPUGEN := no
ifeq "${CHIP_HAS_BCPU_ROM}" "1"
ifneq "${NO_BCPU_IMAGE}" "1"
    BCPUGEN := yes
endif
endif

export RAMINITGEN := no
ifeq "${STANDALONE_RAMINIT_IMAGE}" "1"
ifneq "${BUILD_ROM}" "1"
    RAMINITGEN := yes
endif
endif

ifeq "${CT_RAMRUN}" "1"
export CT_JAVA_ENABLE := 0
else
export CT_JAVA_ENABLE := 0
endif


##############################################################################
# Constructs CYGPATH if we are under cygwin
##############################################################################
# Construct the proper CYGPATH building path function
# If we're on a unix machine we don't change the path
# If we are on a CYGWIN install we need to convert the 
# cygwin path into a windows path to use coolxml

## COMPAT_SOFT_WORKDIR = SOFT_WORKDIR version compatible with Windows and Cygwin/Linux env
# To be used with internal tools compiled for windows and linux

BUILD_HOST_TYPE_CMD := "case `uname` in Linux*) echo LINUX;; CYGWIN*) echo CYGWIN;; Windows*) echo WINDOWS;; *) echo UNKNOWN;; esac"
export BUILD_HOST_TYPE := $(shell sh -c $(BUILD_HOST_TYPE_CMD))

ifeq "$(BUILD_HOST_TYPE)" "UNKNOWN"
BUILD_HOST_UNAME := $(shell uname)
$(error "$(BUILD_HOST_UNAME) is unsupported(LINUX, CYGWIN, WINDOWS)")
endif
$(info Build host is $(BUILD_HOST_TYPE))


##############################################################################
##  Make Option
##############################################################################
# Eliminate use of the built-in implicit rules
export MAKEFLAGS += -r

# Silent option
ifneq "${VERBOSE}" "1"
    export MAKEFLAGS += -s --no-print-directory
    # Pipe STDOUT/STDERR to /dev/null if not VERBOSE
    # Conversely, have a null output "echo" for "CC ... (file)" stuff when
    # VERBOSE is set
    export STDOUT_NULL := 1>/dev/null
    export STDERR_NULL := 2>/dev/null
    export ECHO := echo
else
    export ECHO := echo >/dev/null
endif # ! VERBOSE


##########################################
# Define the tools to use
##########################################
ifeq "$(CT_COMPILER)" "gcc4_xcpu"
export CROSS := mips-elf-
else ifeq "$(CT_COMPILER)" "gcc7_xcpu2"
export CROSS := mips-rda-elf-
else
$(error "CT_COMPILER is invalid")
endif

export AS := $(CROSS)as
ifeq "${CCACHE_ENABLE}" "1"
export CC := ccache $(CROSS)gcc
else
export CC := $(CROSS)gcc
endif
export CPP := ${CC} -E
export CXX := $(CROSS)g++
ifeq "${CT_JAVA_ENABLE}" "1"
export GCJ := $(CROSS)gcj
export GCJH := gjavah
endif
export AR := $(CROSS)ar
export RANLIB := $(CROSS)ranlib
export OBJCOPY := $(CROSS)objcopy
export OBJDUMP := $(CROSS)objdump
export NM := $(CROSS)nm
export LD := $(CROSS)ld
export STRIP := $(CROSS)strip
# To preprocess non-C-code files (no line numbers, no garbage, no predefined vars)
export LDPP := $(CROSS)cpp -C -P -undef 
export GDB ?= $(CROSS)gdbtui
export GPERF := gperf
export AWK := awk
export CAT := cat
export LZMA := lzma

export CP := cp -f
export FIND := find
export DIFF := diff

# Version checking function
# $(1): shell commands to generate the version string at the first line
# Return: 0/1 major minor info
VERSION_CHECK_FUNC = $(shell $(1) | perl -e '$$_=<>; \
    		if (/\s((\d+)\.(\d+)(\.\d+)*)\s/) { \
    			print 0; print " $$2"; print " $$3"; print " $$1"; \
    		} else { \
    			print 1; print " 0"; print " 0"; print " unknown"; \
    		}')

GCC_VER_CHECK := $(call VERSION_CHECK_FUNC, $(CC) --version)
GCC_VER_RESULT := $(word 1,$(GCC_VER_CHECK))
GCC_VER_MAJOR := $(word 2,$(GCC_VER_CHECK))
GCC_VER_MINOR := $(word 3,$(GCC_VER_CHECK))
GCC_VER_INFO := $(word 4,$(GCC_VER_CHECK))
ifneq "$(GCC_VER_RESULT)" "0"
$(error GCC version is ${GCC_VER_INFO})
endif
$(info GCC version is ${GCC_VER_INFO})

BINUTILS_VER_CHECK := $(call VERSION_CHECK_FUNC, $(LD) --version)
BINUTILS_VER_RESULT := $(word 1,$(BINUTILS_VER_CHECK))
BINUTILS_VER_MAJOR := $(word 2,$(BINUTILS_VER_CHECK))
BINUTILS_VER_MINOR := $(word 3,$(BINUTILS_VER_CHECK))
BINUTILS_VER_INFO := $(word 4,$(BINUTILS_VER_CHECK))
ifneq "$(BINUTILS_VER_RESULT)" "0"
$(error Binutils version is ${BINUTILS_VER_INFO})
endif
$(info Binutils version is ${BINUTILS_VER_INFO})

export USE_GCC_4 := 1
export USE_BINUTILS_2_19 := 1
export ENABLE_GC_SECTIONS ?= 1
export GCC_VER_MAJOR

# Discard all the unlisted input sections (even if they are used) ?
export DISCARD_UNLISTED_SECTIONS ?= 1
# Print the removed unused input sections ?
export PRINT_GC_SECTIONS ?= 0

ifeq "${USE_GCC_4}" "1"
C_SPECIFIC_CFLAGS    += -Woverride-init
endif

# Use the GCC libs in repository
ifeq "$(CT_COMPILER)" "gcc7_xcpu2"
LIBGCC_FILE_NAME := $(shell $(CC) -march=xcpu2 -mips16 -EL -print-libgcc-file-name)
export LIBGCC_PATH := $(shell dirname $(LIBGCC_FILE_NAME))
else
export LIBGCC_PATH := ${SOFT_WORKDIR}/env/lib/gcc/mips-elf/4.4.2
endif
$(info libgcc in $(LIBGCC_PATH))

# Limit objdump to these sections :
#export OBJDUMP_DISPLAY_SECTS := .boot_code .calib .factory_settings .user_data .main_entry_section .sramtext .sramdata .sramucdata .ram .text .ucramdata .data .rotext .rodata .internal_rom .internal_tmp_rom .internal_rom.crc .sramtxt .sysIrqSection .bbIrqSection .sramtextmisc .proxy_memory .fast_cpu_memory_overlay_0 .fast_cpu_memory_overlay_1 .fast_cpu_memory_overlay_2 .fast_cpu_memory_overlay_3 .fast_cpu_memory_overlay_4 .fast_cpu_memory_overlay_5 .boot_rom_version_number
#export OBJDUMP_OPTS := ${foreach sect, ${OBJDUMP_DISPLAY_SECTS}, -j ${sect}} -D -t
export OBJDUMP_OPTS := -D -t

ifeq "${CT_COMPRESS_CODE_SECTION}" "y"
COMPRESS_PROCESS_ENABLE := yes
endif
ifeq "${CT_COMPRESS_DATA_SECTION}" "yes"
COMPRESS_PROCESS_ENABLE := yes
endif
export COMPRESS_PROCESS_ENABLE

ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"
ifeq "${CT_COMPRESS_PREMAIN}" "y"
COMPRESS_PREMAIN_ENABLE := yes
endif
endif # COMPRESS_PROCESS_ENABLE
export COMPRESS_PREMAIN_ENABLE

export CKSUM := cksum #md5sum
ifeq "$(strip ${MMI_SUPPORT_NATIVE_JAVA})" "yes"
ifneq "${CT_COMPRESS_DATA_SECTION}" "yes" 
$(error "native java use compressed data section, please check conversion.mk")
endif
NATIVE_JAVA_SUPPORT := yes
endif
export NATIVE_JAVA_SUPPORT

ifeq "${CT_PROTOSTACK_IN_RAM}" "y"
PROTOSTACK_IN_RAM := 1
endif
export PROTOSTACK_IN_RAM

###############################################################################
# Generic environment stuff
###############################################################################
## Generic directory names
export BUILD_DIR := build/${CT_TARGET}/_default_
ifeq "${IS_ENTRY_POINT}" "yes"
ifneq "${CT_APP}" "_"
export BUILD_DIR := build/${CT_TARGET}/$(notdir ./${LOCAL_NAME})
endif
endif
export OBJ_DIR := obj
export SRC_DIR := src
export LIB_DIR := lib
export DEPS_DIR := deps
export INC_DIR := include

HEX_DIR := hex

# BUILD_ROOT is the base of the build tree,
# just like SOFT_WORKDIR is that of the source tree
# It defaults to  ${SOFT_WORKDIR}/${BUILD_DIR} when OUTPUT_DIR is not set.
ifneq "$(OUTPUT_DIR)" ""
export BUILD_ROOT := ${OUTPUT_DIR}/${BUILD_DIR}
export HEX_ROOT := ${OUTPUT_DIR}/${HEX_DIR}
else
export BUILD_ROOT := ${SOFT_WORKDIR}/${BUILD_DIR}
export HEX_ROOT := ${SOFT_WORKDIR}/${HEX_DIR}
endif

ifeq "${BCPUGEN}"  "yes"
export BCPU_BUILD_ROOT := ${BUILD_ROOT}/bcpu
endif

ifeq "${RAMINITGEN}"  "yes"
export RAMINIT_BUILD_ROOT := ${BUILD_ROOT}/raminit
endif

# Date to include in binaries as build date
# TODO_OPTIM?
BUILD_TIME ?= $(shell date "+%Y-%m-%d_%H:%M:%S")
BUILD_DATE ?= $(shell date "+%Y%m%d")
export BUILD_TIME BUILD_DATE

ifeq "${WITH_GIT}" "1"
BUILD_VERSION ?= $(shell git rev-parse --short=8 HEAD)
else
BUILD_VERSION ?= unknown
endif
export BUILD_VERSION

## These targets simply call the bin rule 
# If the last target built is different from the last one, clean before building
export LAST_TARGET_FILE := ${BUILD_ROOT}/last_target.txt
export LAST_TARGET_EXIST := $(wildcard ${LAST_TARGET_FILE})
ifeq ($(strip $(LAST_TARGET_EXIST)),)
export LAST_TARGET :=
else
export LAST_TARGET := $(shell cat ${LAST_TARGET_FILE})
endif

# Allow the objects of different release types to coexist with each other
export TARGET_TAG := ${CT_TARGET}

# Flag to put rom contents into flash elf file
ifeq "${CT_RAMRUN}" "1"
export ROM_IN_FLASH_ELF ?= 0
else
export ROM_IN_FLASH_ELF ?= 1
endif

# Flags to generate lods to clean special sectors
export CLEAN_USER_SECTOR ?= 0
export CLEAN_CALIB_SECTOR ?= 0
export CLEAN_FACT_SECTOR ?= 0

ifneq "${CLEAN_USER_SECTOR}${CLEAN_CALIB_SECTOR}${CLEAN_FACT_SECTOR}" "000"
# Generate sector cleaner lods only
# (Note that the override cannot take effect in sub-make by exporting)
override CT_MODULES := link
# Do NOT compress code
COMPRESS_PROCESS_ENABLE := no
# Do NOT support java and related compression codes
NATIVE_JAVA_SUPPORT := no
# Do NOT generate BCPU image
BCPUGEN := no
# Do NOT generate RAMINIT image
RAMINITGEN := no
endif

# Do NOT compress code if this is a ramrun build, or this is not a full MMI build.
# NOTE: The following assignments cannot override the definition from command line.
ifeq "${CT_RAMRUN}" "1"
COMPRESS_PROCESS_ENABLE := no
# Do NOT support java and related compress process
NATIVE_JAVA_SUPPORT := no
endif # CT_RAMRUN
ifeq "$(findstring application,${LOCAL_MODULE_DEPENDS})" ""
COMPRESS_PROCESS_ENABLE := no
# Do NOT support java and related compress process
NATIVE_JAVA_SUPPORT := no
endif # LOCAL_MODULE_DEPENDS

export COMPRESS_PROCESS_ENABLE

#-----------------------------------------------------------------
# Optional Build:
#     Build designated modules only and link all others.
#
# CT_MODULES defines a comma-delimited module list, which should
# be rebuilt in this make, and all other modules can be linked
# with the libraries generated in the last make.
# ASSUMPTION:
#     LOCAL_MODULE_DEPENDS are located in the subdirectories of this module.
# KNOWN EXCEPTIONS:
#     platform -- target
#     platform/chip/hal -- platform/chip/regs/$(CT_ASIC)
# (All the exceptions can be figured out by specifying
# DUMP_DEP_NOT_IN_SUBDIR=1 in the command line)
#-----------------------------------------------------------------
export MODULES_MUST_BUILD :=

# Define a fake directory hierachy for an exceptional prerequisite module,
# so as to show the dependency but not to rebuild the whole upper-level module
export EXCEPTION_MODULE_1 := platform
export EXCEPTION_MOD_DEP_1 := target
export EXCEPTION_FAKE_SUBDIR_1 := platform/target

export EXCEPTION_MODULE_2 := platform/chip/hal
export EXCEPTION_MOD_DEP_2 := platform/chip/regs/$(CT_ASIC)
export EXCEPTION_FAKE_SUBDIR_2 := platform/chip/hal/regs

ifneq "$(CT_MODULES)" ""
NULL_STRING :=
SPACE := $(NULL_STRING) # end of the line
COMMA := ,
MODULES_MUST_BUILD += $(subst $(COMMA),$(SPACE),$(CT_MODULES))

ifeq "${CLEAN_USER_SECTOR}${CLEAN_CALIB_SECTOR}${CLEAN_FACT_SECTOR}" "000"
ifneq "$(CT_PRODUCT)" ""
# target_config.c needs to record the product name
MODULES_MUST_BUILD += target
endif
ifeq "${CHIP_HAS_AP}" "1"
# target_config.c needs to record the build date
MODULES_MUST_BUILD += target
endif
endif # Not sector cleaner lods

ifneq "$(filter $(EXCEPTION_MOD_DEP_1),$(MODULES_MUST_BUILD))" ""
MODULES_MUST_BUILD += $(EXCEPTION_FAKE_SUBDIR_1)
endif
ifneq "$(filter $(EXCEPTION_MODULE_1),$(MODULES_MUST_BUILD))" ""
MODULES_MUST_BUILD += $(EXCEPTION_MOD_DEP_1)
endif

ifneq "$(filter $(EXCEPTION_MOD_DEP_2),$(MODULES_MUST_BUILD))" ""
MODULES_MUST_BUILD += $(EXCEPTION_FAKE_SUBDIR_2)
endif
ifneq "$(filter $(EXCEPTION_MODULE_2),$(MODULES_MUST_BUILD))" ""
MODULES_MUST_BUILD += $(EXCEPTION_MOD_DEP_2)
endif

MODULES_MUST_BUILD := $(sort $(MODULES_MUST_BUILD))
endif # CT_MODULES
#-----------------------------------------------------------------
# End of Optional Build
#-----------------------------------------------------------------

########################################################################
# safety check
########################################################################
ifeq ($(origin LOCAL_EXPORT_FLAG),command line)
$(error LOCAL_EXPORT_FLAG must not be overriden from the command line!)
endif

########################################################################
# End of MAKELEVEL=0. Things to do only once.
########################################################################
endif

ifeq ($(MAKELEVEL),40)
$(error Recursive make reached level 40, there is probably a cycle in your module/model dependancies)
endif #MAKELEVEL 40

# Check if compilevars.mk has been correctly included at the beginning of the Makefile 
ifndef COMPILEVARS_DEFINED
$(error "!!!! Each Makefile must include $(SOFT_WORKDIR)/env/compilation/compilevars.mk  at the beginning !!!!")
endif

# When building BCPU image, override BUILD_ROOT, COMPRESS_PROCESS_ENABLE,
# and NATIVE_JAVA_SUPPORT settings
ifeq "${BUILD_BCPU_IMAGE}" "1"
export BUILD_ROOT := ${BCPU_BUILD_ROOT}
COMPRESS_PROCESS_ENABLE := no
NATIVE_JAVA_SUPPORT := no
endif

# When building RAMINIT image, override ENABLE_GC_SECTIONS, BUILD_ROOT,
# COMPRESS_PROCESS_ENABLE, and NATIVE_JAVA_SUPPORT settings
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
# The symbols in RAMINIT image will be referenced by normal image.
export ENABLE_GC_SECTIONS := 0
export BUILD_ROOT := ${RAMINIT_BUILD_ROOT}
COMPRESS_PROCESS_ENABLE := no
NATIVE_JAVA_SUPPORT := no
endif

# Must not be exported to be taken into account
SHELL := /bin/bash 

ifeq "$(BUILD_HOST_TYPE)" "CYGWIN"
NOCYGPATH = $(shell cygpath -am $(1))
else
NOCYGPATH = $(1)
endif


#############################################################################
# Defaults for global options
#############################################################################
WITH_PERL ?= 1
# All files are compiled in mips16 format by default
# By default all files are compiled in MIPS16 format
# with dbg info + optimized
# with reduced regs
CT_MIPS16_CFLAGS ?= -mips16
CT_ROMULATOR ?= n
# Default configuration to chip
CT_ASIC_CFG ?= chip

# for legacy Makefile compatibility
LOCAL_MODULE_DEPENDS += $(LOCAL_DEPEND_LIBS)

# Absolute path to PWD, from the Makefile
LOCAL_PATH := ${SOFT_WORKDIR}/${LOCAL_NAME}
# Corresponding path within the build tree
LOCAL_SHADOW_PATH := ${BUILD_ROOT}/${LOCAL_NAME}

# Module name.
MODULE_NAME := ${notdir ${LOCAL_NAME}}

# LOCAL_*_DIR == abstract for *_DIR, more generic (takes into account IS_ENTRY_POINT
# where we have no src/, etc...)
# For use as relative path
ifneq "${IS_ENTRY_POINT}" "yes"
    LOCAL_SRC_DIR := ${SRC_DIR}
    LOCAL_INC_DIR := ${INC_DIR}
    LOCAL_LIB_DIR := ${LIB_DIR}
else
    LOCAL_SRC_DIR := .
    LOCAL_INC_DIR := .
    LOCAL_LIB_DIR := .
endif #IS_ENTRY_POINT

ifeq "${IS_DLL_ENTRY}" "yes"
    LOCAL_SRC_DIR := .
    LOCAL_INC_DIR := .
    LOCAL_LIB_DIR := .
endif #IS_DLL_ENTRY


## Corresponding absolute paths
# In source tree
SRC_PATH := ${LOCAL_PATH}/${SRC_DIR}
INC_PATH := ${LOCAL_PATH}/${INC_DIR}
# This one can be overwritten by packed libraries.
BINLIB_PATH ?= ${LOCAL_PATH}/${LIB_DIR}
# in build tree
DEPS_PATH := ${LOCAL_SHADOW_PATH}/${DEPS_DIR}
LIB_PATH := ${LOCAL_SHADOW_PATH}/${LIB_DIR}
OBJ_PATH := ${LOCAL_SHADOW_PATH}/${OBJ_DIR}
LOCAL_LIB_PATH := ${LOCAL_SHADOW_PATH}/${LOCAL_LIB_DIR}
#TODO REMOVE REL PATH
DEPS_REL_PATH := ${DEPS_PATH}/${CT_RELEASE}
OBJ_REL_PATH := ${OBJ_PATH}/${CT_RELEASE}

## Srecmap config files
MAP_FILE := ${SOFT_WORKDIR}/env/compilation/${CT_ASIC}_map_cfg
ifeq "${CT_ASICSIMU}" "1"
    ifneq "$(wildcard ${SOFT_WORKDIR}/env/compilation/${CT_ASIC}_asicsimu_map_cfg)" ""
        MAP_FILE := ${SOFT_WORKDIR}/env/compilation/${CT_ASIC}_asicsimu_map_cfg
    endif
endif # CT_ASICSIMU

## Add the old local lib variable for compatible issues
LOCAL_LIBS += ${LOCAL_LIB}


###############################################################################
# Dependency treatment
###############################################################################
# Split LOCAL_MODULE_DEPENDS between SRC_LIBS and BINARY_LIBS.
# Use defined binary libs as default (e.g. from customvars.mk)
# Allow local append for binary libs, then local override for src libs, with src 
# superseding any conflicting binary setting

# Sometimes we want the build to be done in a particular order, eg. when
# the build of a module creates a public header file.
## DO NOT Remove any duplicate.
#LOCAL_MODULE_DEPENDS:= ${sort ${LOCAL_MODULE_DEPENDS}}

LOCAL_MODULE_DEPENDS_SORTED := ${sort ${LOCAL_MODULE_DEPENDS}}
ifneq "${words ${LOCAL_MODULE_DEPENDS_SORTED}}" "${words ${LOCAL_MODULE_DEPENDS}}"
$(warning ${LOCAL_NAME} has duplicate modules listed in LOCAL_MODULE_DEPENDS!)
endif

NULL_STRING :=
SPACE := $(NULL_STRING) # end of the line

# ----------------------------------------------------------------------
# Check if to override LOCAL_MODULE_DEPENDS when building BCPU image
# ----------------------------------------------------------------------
ifeq "${BUILD_BCPU_IMAGE}" "1"
# NOTE: Sub-module of exception modules are not considered here
EXCEPTION_BCPU_MOD :=
ifneq "$(filter $(EXCEPTION_MODULE_1),$(LOCAL_MODULE_DEPENDS))" ""
ifneq "$(filter $(EXCEPTION_MOD_DEP_1),$(BCPU_MODULE_DEPENDS))" ""
EXCEPTION_BCPU_MOD += $(EXCEPTION_MOD_DEP_1)
endif
endif

ifneq "$(filter $(EXCEPTION_MODULE_2),$(LOCAL_MODULE_DEPENDS))" ""
ifneq "$(filter $(EXCEPTION_MOD_DEP_2),$(BCPU_MODULE_DEPENDS))" ""
EXCEPTION_BCPU_MOD += $(EXCEPTION_MOD_DEP_2)
endif
endif

LOCAL_MOD_IS_BCPU_MOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(filter $(MOD_LOCAL),$(BCPU_MODULE_DEPENDS)) \
     )
LOCAL_MOD_IS_BCPU_SUBMOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(foreach MOD_BCPU,$(BCPU_MODULE_DEPENDS), \
            $(if $(findstring $(SPACE)$(MOD_BCPU)/,$(SPACE)$(MOD_LOCAL)$(SPACE)), \
                 $(MOD_LOCAL), \
             ) \
         ) \
     )
BCPU_MOD_IS_LOCAL_SUBMOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(foreach MOD_BCPU,$(BCPU_MODULE_DEPENDS), \
            $(if $(findstring $(SPACE)$(MOD_LOCAL)/,$(SPACE)$(MOD_BCPU)$(SPACE)), \
                 $(MOD_BCPU), \
             ) \
         ) \
     )
LOCAL_MODULE_DEPENDS := $(sort $(LOCAL_MOD_IS_BCPU_MOD) $(LOCAL_MOD_IS_BCPU_SUBMOD) $(BCPU_MOD_IS_LOCAL_SUBMOD) $(EXCEPTION_BCPU_MOD))

GLOBAL_BINARY_LIBS := ${GLOBAL_BCPU_BINARY_LIBS}
LOCAL_BINARY_LIBS := ${LOCAL_BCPU_BINARY_LIBS}
LOCAL_LIBS := ${LOCAL_BCPU_LIBS}
LOCAL_SRC_LIBS := ${LOCAL_BCPU_SRC_LIBS}
endif

# ----------------------------------------------------------------------
# Check if to override LOCAL_MODULE_DEPENDS when building RAMINIT image
# ----------------------------------------------------------------------
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
# NOTE: Sub-module of exception modules are not considered here
EXCEPTION_RAMINIT_MOD :=
ifneq "$(filter $(EXCEPTION_MODULE_1),$(LOCAL_MODULE_DEPENDS))" ""
ifneq "$(filter $(EXCEPTION_MOD_DEP_1),$(RAMINIT_MODULE_DEPENDS))" ""
EXCEPTION_RAMINIT_MOD += $(EXCEPTION_MOD_DEP_1)
endif
endif

ifneq "$(filter $(EXCEPTION_MODULE_2),$(LOCAL_MODULE_DEPENDS))" ""
ifneq "$(filter $(EXCEPTION_MOD_DEP_2),$(RAMINIT_MODULE_DEPENDS))" ""
EXCEPTION_RAMINIT_MOD += $(EXCEPTION_MOD_DEP_2)
endif
endif

LOCAL_MOD_IS_RAMINIT_MOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(filter $(MOD_LOCAL),$(RAMINIT_MODULE_DEPENDS)) \
     )
LOCAL_MOD_IS_RAMINIT_SUBMOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(foreach MOD_RAMINIT,$(RAMINIT_MODULE_DEPENDS), \
            $(if $(findstring $(SPACE)$(MOD_RAMINIT)/,$(SPACE)$(MOD_LOCAL)$(SPACE)), \
                 $(MOD_LOCAL), \
             ) \
         ) \
     )
RAMINIT_MOD_IS_LOCAL_SUBMOD := \
    $(foreach MOD_LOCAL,$(LOCAL_MODULE_DEPENDS), \
        $(foreach MOD_RAMINIT,$(RAMINIT_MODULE_DEPENDS), \
            $(if $(findstring $(SPACE)$(MOD_LOCAL)/,$(SPACE)$(MOD_RAMINIT)$(SPACE)), \
                 $(MOD_RAMINIT), \
             ) \
         ) \
     )
LOCAL_MODULE_DEPENDS := $(sort $(LOCAL_MOD_IS_RAMINIT_MOD) $(LOCAL_MOD_IS_RAMINIT_SUBMOD) $(RAMINIT_MOD_IS_LOCAL_SUBMOD) $(EXCEPTION_RAMINIT_MOD))

GLOBAL_BINARY_LIBS := ${GLOBAL_RAMINIT_BINARY_LIBS}
LOCAL_BINARY_LIBS := ${LOCAL_RAMINIT_BINARY_LIBS}
LOCAL_LIBS := ${LOCAL_RAMINIT_LIBS}
LOCAL_SRC_LIBS := ${LOCAL_RAMINIT_SRC_LIBS}
endif

# ----------------------------------------------------------------------
# List all the modules that are declared as BINARY in the dependency list
# ----------------------------------------------------------------------
BINARY_LIBS := ${filter ${GLOBAL_BINARY_LIBS}  ${LOCAL_BINARY_LIBS},${LOCAL_MODULE_DEPENDS}}
# remove the ones that are localy declared as source module
BINARY_LIBS := ${filter-out ${LOCAL_SRC_LIBS},${BINARY_LIBS}}
# Add the user libraries
#BINARY_LIBS += ${LOCAL_LIBS}


LOCAL_BUILT_BINARY_LIBS :=

#-----------------------------------------------------------------
# Optional Build
#-----------------------------------------------------------------
DEPS_NOT_IN_SUBDIR :=

ifeq "$(DUMP_DEP_NOT_IN_SUBDIR)" "1"
DEPS_NOT_IN_SUBDIR := \
    $(strip \
        $(foreach MOD_DEP, $(LOCAL_MODULE_DEPENDS), \
            $(if $(findstring $(SPACE)$(LOCAL_NAME)/,$(SPACE)$(MOD_DEP)$(SPACE)),,$(MOD_DEP)) \
         ) \
     )
endif # DUMP_DEP_NOT_IN_SUBDIR == 1

# ----------------------------------------------------------------------
# List all the modules that are declared as must-be-built
# ----------------------------------------------------------------------
ifeq "$(UPD_TOP_LIB_ONLY)" "1"
# Top-level libs are generated at makelevel 1
ifeq "$(MAKELEVEL)" "1"
TOP_LIB_DEPENDS := \
    $(foreach MOD_DEP, $(LOCAL_MODULE_DEPENDS), \
        $(foreach MOD_BUILD, $(MODULES_MUST_BUILD), \
            $(if \
                $(or $(filter $(MOD_DEP),$(MOD_BUILD)), \
                     $(findstring $(SPACE)$(MOD_DEP)/,$(SPACE)$(MOD_BUILD)$(SPACE)), \
                 ), \
                $(MOD_BUILD), \
                \
             ) \
         ) \
     )
TOP_LIB_DEPENDS := \
    $(subst $(SPACE)$(EXCEPTION_FAKE_SUBDIR_1)$(SPACE),$(SPACE)$(EXCEPTION_DEP_MOD_1)$(SPACE),$(SPACE)$(TOP_LIB_DEPENDS)$(SPACE))
TOP_LIB_DEPENDS := \
    $(subst $(SPACE)$(EXCEPTION_FAKE_SUBDIR_2)$(SPACE),$(SPACE)$(EXCEPTION_DEP_MOD_2)$(SPACE),$(SPACE)$(TOP_LIB_DEPENDS)$(SPACE))
# Build designated modules and archive them to the top lib
LOCAL_MODULE_DEPENDS := $(TOP_LIB_DEPENDS)
# All the prerequisite modules should be rebuilt and no need to check linkable modules any more
MODULES_MUST_BUILD :=
endif # MAKELEVEL == 1
endif # UPD_TOP_LIB_ONLY == 1

ifneq "$(MODULES_MUST_BUILD)" ""
# Deferred expansion for function definition
CHECK_SUBMOD_MUST_BUILD_FUNC = \
    $(if \
        $(strip \
            $(foreach MOD_BUILD,$(MODULES_MUST_BUILD), \
                $(if $(findstring $(SPACE)$(MOD_BUILD)/,$(SPACE)$(1)$(SPACE)),x,) \
             ) \
         ), \
         , \
         $(1) \
     )
LIBS_MUST_LINK := \
    $(foreach MOD_DEP, $(LOCAL_MODULE_DEPENDS), \
        $(if \
            $(or $(filter $(MOD_DEP),$(MODULES_MUST_BUILD)), \
                 $(findstring $(SPACE)$(MOD_DEP)/,$(SPACE)$(MODULES_MUST_BUILD)$(SPACE)), \
             ), \
            , \
            $(call CHECK_SUBMOD_MUST_BUILD_FUNC,$(MOD_DEP)) \
         ) \
     )
LOCAL_BUILT_BINARY_LIBS += $(filter-out $(BINARY_LIBS),$(LIBS_MUST_LINK))
endif # MODULES_MUST_BUILD
#-----------------------------------------------------------------
# End of Optional Build
#-----------------------------------------------------------------

# ----------------------------------------------------------------------
# List all the modules that are declared as source
# ----------------------------------------------------------------------
SRC_LIBS := ${filter-out ${BINARY_LIBS} ${LOCAL_BUILT_BINARY_LIBS},${LOCAL_MODULE_DEPENDS}}

# ----------------------------------------------------------------------
# Locally some extra libries can be defined using LOCAL_LIBS
# ----------------------------------------------------------------------
# The convention for LOCAL_LIBS is to declare them starting from $SOFT_WORDIR
# The name of such a library must be of the form lib<my_librarary_name>.a
# ex: LOCAL_LIBS := platform/thirdparty/thisprovider/libstuff.a

# ----------------------------------------------------------------------
#  Build library file list, path and linker script inclusion flag
# ----------------------------------------------------------------------
# Src libs path and files : in $BUILD_ROOT
SRC_LIBRARY_PATH := ${foreach MODULE_PATH, ${SRC_LIBS}, -L${BUILD_ROOT}/${MODULE_PATH}/${LIB_DIR} }
SRC_LIBRARY_FILES := ${foreach MODULE_PATH, ${SRC_LIBS}, ${BUILD_ROOT}/${MODULE_PATH}/${LIB_DIR}/lib${notdir ${MODULE_PATH}}_${CT_RELEASE}.a }

ifeq "${LOCAL_LIBS_FIRST}" "1"
LIBS_LINK_LOCAL += ${foreach MODULE_PATH, ${BINARY_LIBS}, \
                        ${if ${wildcard ${BUILD_ROOT}/${MODULE_PATH}/${LIB_DIR}/lib${notdir ${MODULE_PATH}}_${CT_RELEASE}.a}, \
                            ${MODULE_PATH} } }
LOCAL_BUILT_BINARY_LIBS += ${LIBS_LINK_LOCAL}
BINARY_LIBS := ${filter-out ${LIBS_LINK_LOCAL}, ${BINARY_LIBS}}
endif # LOCAL_LIBS_FIRST

BINARY_LIBRARY_PATH := 
BINARY_LIBRARY_FILES := 

# Local built binary libs path and files: in $BUILD_ROOT
BINARY_LIBRARY_PATH += ${foreach MODULE_PATH, ${LOCAL_BUILT_BINARY_LIBS}, -L${BUILD_ROOT}/${MODULE_PATH}/${LIB_DIR} }
BINARY_LIBRARY_FILES += ${foreach MODULE_PATH, ${LOCAL_BUILT_BINARY_LIBS}, ${BUILD_ROOT}/${MODULE_PATH}/${LIB_DIR}/lib${notdir ${MODULE_PATH}}_${CT_RELEASE}.a }

# Binary libs path and files : in $SOFT_WORKDIR
BINARY_LIBRARY_PATH += ${foreach MODULE_PATH, ${BINARY_LIBS}, -L${SOFT_WORKDIR}/${MODULE_PATH}/${LIB_DIR} }
BINARY_LIBRARY_FILES += ${foreach MODULE_PATH, ${BINARY_LIBS}, ${SOFT_WORKDIR}/${MODULE_PATH}/${LIB_DIR}/lib${notdir ${MODULE_PATH}}_${CT_RELEASE}.a }

# Local libs path and files : in $SOFT_WORKDIR
LOCAL_ADD_LIBRARY_PATH := ${foreach MODULE_PATH, ${LOCAL_LIBS}, -L${SOFT_WORKDIR}/${dir ${MODULE_PATH}}}
# LOCAL_LIBS is already a file list:
#LOCAL_ADD_LIBRARY_FILES := ${foreach MODULE_PATH, ${LOCAL_LIBS}, ${SOFT_WORKDIR}/${MODULE_PATH}/${notdir ${MODULE_PATH}}}
LOCAL_ADD_LIBRARY_FILES := ${foreach FILE_PATH, ${LOCAL_LIBS}, ${SOFT_WORKDIR}/${FILE_PATH}}

# Full libraries path used for linking -L<path_to_library>
FULL_LIBRARY_PATH := ${SRC_LIBRARY_PATH} ${BINARY_LIBRARY_PATH} ${LOCAL_ADD_LIBRARY_PATH}
# List all library files for dependencies checking full_path+"lib"+libname.a
FULL_LIBRARY_FILES := ${SRC_LIBRARY_FILES} ${BINARY_LIBRARY_FILES} ${LOCAL_ADD_LIBRARY_FILES}
# List all libraries used for linking -l(no path, just names without prefix lib)
FULL_LIBRARY_EXT := ${foreach MODULE_PATH, ${FULL_LIBRARY_FILES}, -l${patsubst lib%,%,${basename ${notdir ${MODULE_PATH}}}}}

# Used when building a toplevel with submodules only : all object files from submodules that go into the lib
ifeq "$(IS_TOP_LEVEL)" "yes"
FULL_LIBRARY_OBJECTS := ${foreach lib, ${LOCAL_MODULE_DEPENDS}, ${BUILD_ROOT}/${lib}/${OBJ_DIR}/${CT_RELEASE}/*.o} 
endif

########################################################
## Recursive make calls 
########################################################
# When building a target, we need to build the libraries of the modules 
# declared as source first
SRC_DIRS := ${foreach MODULE_PATH, ${SRC_LIBS}, ${MODULE_PATH}}

# For all dependencies in SRC, rules to call make in dependency modules
FULL_DEPENDENCY_COMPILE := ${foreach SUBDIR, ${SRC_DIRS}, echo && ${ECHO} "MAKE              ${SUBDIR}" && ${MAKE} -C ${SOFT_WORKDIR}/${SUBDIR} all && } echo

FULL_DEPENDENCY_CLEAN := ${foreach SUBDIR, ${SRC_DIRS}, ${MAKE} -C ${SOFT_WORKDIR}/${SUBDIR} cleanstem;}
FULL_DEPENDENCY_ALLCLEAN := ${foreach SUBDIR, ${SRC_DIRS}, ${MAKE} -C ${SOFT_WORKDIR}/${SUBDIR} allcleanstem;}
FULL_DEPENDENCY_DEPCLEAN := ${foreach SUBDIR, ${SRC_DIRS}, ${MAKE} -C ${SOFT_WORKDIR}/${SUBDIR} depcleanstem;}

#################################
## Include path generation ######
#################################
# LOCAL_API_DEPENDS
# list all the include from LOCAL_API_DEPENDS
DEPENDENCY_INCLUDE_PATH := ${foreach module, ${LOCAL_API_DEPENDS}, -I${SOFT_WORKDIR}/${module}/${INC_DIR} }

# ADD all the include from LOCAL_MODULE_DEPENDS 
# (if we depend on a module, we depend on its include also)
DEPENDENCY_INCLUDE_PATH += ${foreach module, ${LOCAL_MODULE_DEPENDS}, -I${SOFT_WORKDIR}/${module}/${INC_DIR} }

# List LOCAL_ADD_INCLUDE header retrieving path
MYINCLUDEDIR := ${foreach tmpDir, ${LOCAL_ADD_INCLUDE}, -I${SOFT_WORKDIR}/${tmpDir}}

# List LOCAL_ADD_ABS_INCLUDE (in absolute path) header retrieving path
MYINCLUDEDIR += ${foreach tmpDir, ${LOCAL_ADD_ABS_INCLUDE}, -I${tmpDir}}

# Root include directory
ROOT_INCLUDE	:= ${SOFT_WORKDIR}/${INC_DIR}

TARGET_INCLUDE	:= ${SOFT_WORKDIR}/target/${CT_TARGET}/${INC_DIR}

# Final include path
# ROOT_INCLUDE at the end because DEPENDENCY_INCLUDE_PATH must be allowed to supersede it (e.g. for sxs_type.h)
INCLUDE_PATH	:= ${MYINCLUDEDIR} -I${LOCAL_INC_DIR} -I${LOCAL_SRC_DIR} ${DEPENDENCY_INCLUDE_PATH} -I${TARGET_INCLUDE} -I${ROOT_INCLUDE}

###############################################################################
# ROM elf and def files
###############################################################################
# Select chip internal ROM elf file if exists.
# This is the description of the objects present in the ROM of the chip.

# By default there is no ROM in the chip - No flag to describe the ROM in linker
LDFLAG_USED_ELF_FILES := 

ifneq "${IS_DLL_ENTRY}" "yes"

ifeq "${BCPUGEN}"  "yes"
ifeq "${BUILD_BCPU_IMAGE}" "1"
CHIP_ROM_ELF_FILE := ${BCPU_ROM_ELF_FILE}
CHIP_ROM_DEF_FILE := ${BCPU_ROM_DEF_FILE}
STRIP_ROM_OBJ2KEEP_FILE := ${STRIP_BCPU_ROM_OBJ2KEEP_FILE}
STRIP_ROM_OBJ2STRIP_FILE := ${STRIP_BCPU_ROM_OBJ2STRIP_FILE}
endif
endif

#----------------------------------------------
# Include the ROM def file if it exists
#----------------------------------------------
ifneq "${CHIP_ROM_DEF_FILE}" ""
ifneq "$(wildcard ${SOFT_WORKDIR}/${CHIP_ROM_DEF_FILE})" ""
include $(SOFT_WORKDIR)/$(CHIP_ROM_DEF_FILE)
endif
endif

LINK_ROM_SYMBOL_ONLY := 1

ifeq "${ROM_IN_FLASH_ELF}" "1"
LINK_ROM_SYMBOL_ONLY := 0
endif

ifeq "${REWRITE_ROM_AT_BOOT}" "1"
ifneq "${CT_ASIC_CFG}" "fpga"
$(error "!!!! REWRITE_ROM_AT_BOOT=1 should be used on FPGA only !!!!")
endif
LDPPFLAGS += -DREWRITE_ROM_AT_BOOT
LINK_ROM_SYMBOL_ONLY := 0
endif

# If the RAMINIT image exists, and if this is an XCPU (neither RAMINIT
# nor BCPU) elf build, the RAMINIT image should be linked instead of the
# ROM image, for the ROM image has been included in the RAMINIT image.
LINK_ROM_ELF := 1
ifneq "${BUILD_BCPU_IMAGE}" "1"
ifeq "${RAMINITGEN}" "yes"
ifneq "${BUILD_RAMINIT_IMAGE}" "1"
LINK_ROM_ELF := 0
endif
endif
endif

ifeq "${LINK_ROM_ELF}" "1"

STRIP_ROM_ELF_FILE := stripped_rom_elf_file.elf

#----------------------------------------------
# Link XCPU rom when building XCPU elf, or link BCPU rom when building BCPU elf
#----------------------------------------------
ifneq "${CHIP_ROM_ELF_FILE}" ""

# There is a ROM in the chip that we fully or partially use, mention it to the linker
CHIP_ROM_ELF_FILE_FULL  := ${SOFT_WORKDIR}/${CHIP_ROM_ELF_FILE}

# If we want to remove some objects from the ROM elf file.
# This can be useful if we don't want to link with some objects of the ROM.
ifneq "${STRIP_ROM_OBJ2KEEP_FILE}${STRIP_ROM_OBJ2STRIP_FILE}" ""
# Only Keep symbols specified in STRIP_ROM_OBJ2KEEP_FILE  
# or Only Strip symbols specified in STRIP_ROM_OBJ2STRIP_FILE 
# This file will be generated just before linking
# Path to generate the stripped ROM elf file.
STRIP_ROM_ELF_FILE_PATHED := ${BUILD_ROOT}/${STRIP_ROM_ELF_FILE}
STRIP_ROM_OBJ2KEEP_FILE_PATHED := ${SOFT_WORKDIR}/${STRIP_ROM_OBJ2KEEP_FILE}
STRIP_ROM_OBJ2STRIP_FILE_PATHED := ${SOFT_WORKDIR}/${STRIP_ROM_OBJ2STRIP_FILE}
USED_ROM_ELF_FILE:= ${STRIP_ROM_ELF_FILE_PATHED}
else # STRIP_ROM_OBJ2KEEP_FILE or STRIP_ROM_OBJ2STRIP_FILE
# use FULL ROM content
USED_ROM_ELF_FILE := ${CHIP_ROM_ELF_FILE_FULL}
endif # STRIP_ROM_OBJ2KEEP_FILE or STRIP_ROM_OBJ2STRIP_FILE

ifeq "${LINK_ROM_SYMBOL_ONLY}" "1"
LDFLAG_USED_ELF_FILES := -just-symbols ${USED_ROM_ELF_FILE}
else # !LINK_ROM_SYMBOL_ONLY
# Put rom contents into flash elf
LDPPFLAGS += -DLOAD_ROM_ELF
LDFLAG_USED_ELF_FILES := ${USED_ROM_ELF_FILE}
endif # !LINK_ROM_SYMBOL_ONLY

endif # CHIP_ROM_ELF_FILE

endif # LINK_ROM_ELF

#----------------------------------------------
# Link BCPU rom when building XCPU elf
#----------------------------------------------
ifneq "${BCPU_ROM_ELF_FILE}" ""

ifeq "${BCPUGEN}"  "yes"
ifneq "${BUILD_BCPU_IMAGE}" "1"
ifneq "${BUILD_ROM}" "1"

ifeq "${LINK_ROM_SYMBOL_ONLY}" "1"

ifneq "${STRIP_ROM_OBJ2KEEP_FILE}${STRIP_ROM_OBJ2STRIP_FILE}" ""
# BCPU rom has been stripped when building BCPU elf
USED_BCPU_ROM_ELF_FILE := ${BCPU_BUILD_ROOT}/${STRIP_ROM_ELF_FILE}
else
USED_BCPU_ROM_ELF_FILE := ${SOFT_WORKDIR}/${BCPU_ROM_ELF_FILE}
endif

LDFLAG_USED_ELF_FILES += -just-symbols ${USED_BCPU_ROM_ELF_FILE}

else # !LINK_ROM_SYMBOL_ONLY

# BCPU rom has been included in BCPU elf, and it will be linked into
# XCPU elf along with BCPU elf.

endif # !LINK_ROM_SYMBOL_ONLY

endif # !BUILD_ROM
endif # !BUILD_BCPU_IMAGE
endif # BCPUGEN

endif # BCPU_ROM_ELF_FILE

endif # !IS_DLL_ENTRY

###################################################################################
# Linker script generation
###################################################################################
# The linker script is generated by preprocessing ${CT_ASIC}_master.ld by default
# An alternate linker source file can be specified using the LINKER_MASTER variable
# This is something more advanced, that takes the preprocessed output and modifies 
# it to generate a custom memory layout, according to env variables.

# Generate sector cleaner lods only
ifneq "${CLEAN_USER_SECTOR}${CLEAN_CALIB_SECTOR}${CLEAN_FACT_SECTOR}" "000"
override LINKER_MASTER := modem2G_sectorcleaner_master.ld
endif

# Select the source of the linker script with LINKER_MASTER
# If not specified we will use the default linker for the CT_ASIC
# If specified use an alternate linker script
ifeq "${LINKER_MASTER}" ""
ifeq "${BUILD_BCPU_IMAGE}" "1"
LD_SRC := ${SOFT_WORKDIR}/env/link/${CT_ASIC}_bcpu_master.ld
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
LD_SRC := ${SOFT_WORKDIR}/env/link/${CT_ASIC}_raminit_master.ld
else
LD_SRC := ${SOFT_WORKDIR}/env/link/${CT_ASIC}_master.ld
endif
endif
else # LINKER_MASTER
ifeq "${BUILD_BCPU_IMAGE}" "1"
LD_SRC := ${SOFT_WORKDIR}/env/link/${BCPU_LINKER_MASTER}
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
LD_SRC := ${SOFT_WORKDIR}/env/link/${RAMINIT_LINKER_MASTER}
else
LD_SRC := ${SOFT_WORKDIR}/env/link/${LINKER_MASTER}
endif
endif
endif # LINKER_MASTER

# Filename of the Linker script generated on the fly
LD_FILE := ${BUILD_ROOT}/${CT_ASIC}.ld

##################################################################################
## Linker flags
##################################################################################
OFORMAT := --oformat=elf32-littlemips
SREC		:= --input-target=elf32-littlemips --output-target=srec

CODE_BASE ?= 0x00000000
EXTRA_CODE_BASE ?= 0x007D0000
EXTRA_CODE_SIZE ?= 0x00000000
FLASH_BOOT_OFFSET ?= 0x00000000

ifeq "${FOTA_SUPPORT}" "y"
CODE_FLASH_PHY_END ?= ${FLASH_BASE}+${FOTA_BACKUP_AREA_START}
else
CODE_FLASH_PHY_END ?= ${FLASH_BASE}+${SPIFFS_FLASH_START}
endif

# Exports RAM/FLASH parameters to the linker script
# (Specified in memd/$(FLSH_MODEL}/memd.def or 
# overdefined in target/xxx/target.def)
ifneq "${CHIP_HAS_AP}" "1"
LDPPFLAGS += \
    -DCODE_FLASH_PHY_END=${CODE_FLASH_PHY_END} \
    -DUSER_DATA_PHY_BASE=${FLASH_BASE}+${USER_DATA_BASE} \
    -DUSER_DATA_SIZE=${USER_DATA_SIZE} \
    -DFLASH_SIZE=${FLASH_SIZE} \
    -DRAM_SIZE=${RAM_SIZE} \
    -DCALIB_BASE=${FLASH_BASE}+${CALIB_BASE} \
    -DFACT_SETTINGS_BASE=${FLASH_BASE}+${FACT_SETTINGS_BASE} \
    -DCODE_BASE=${FLASH_BASE}+${CODE_BASE} \
    -DEXTRA_CODE_BASE=${FLASH_BASE}+${EXTRA_CODE_BASE} \
    -DEXTRA_CODE_SIZE=${EXTRA_CODE_SIZE} \
    -DREMAIN_BASE=${FLASH_BASE}+${REMAIN_BASE} \
    -DREMAIN_SIZE=${REMAIN_SIZE}
else # CHIP_HAS_AP
LDPPFLAGS += \
    -DRAM_SIZE=${RAM_SIZE} \
    -DCALIB_BASE=${EXT_RAM_BASE}+${CALIB_BASE} \
    -DFACT_SETTINGS_BASE=${EXT_RAM_BASE}+${FACT_SETTINGS_BASE} \
    -DEXTRA_CODE_BASE=${EXT_RAM_BASE}+${EXTRA_CODE_BASE} \
    -DEXTRA_CODE_SIZE=${EXTRA_CODE_SIZE} \
    -DRESERVED_RAMINIT_SIZE=${RESERVED_RAMINIT_SIZE} \
    -DSYS_SRAM_DATA_SIZE=${SYS_SRAM_DATA_SIZE} \
    -DEXT_RAM_DATA_SIZE=${EXT_RAM_DATA_SIZE} \
    -DREMAIN_BASE=${FLASH_BASE}+${REMAIN_BASE} \
    -DREMAIN_SIZE=${REMAIN_SIZE}
endif # CHIP_HAS_AP

ifeq "${REDUNDANT_DATA_REGION}" "y"
LDPPFLAGS += -DREDUNDANT_DATA_REGION
endif

# Export all the CHIP_EXPORT_FLAG to the linker, so that dedicated
# features of a chip that need a component of the linker script
# can enable those parts.
#
CHIP_EXPORT_FLAG := $(sort ${CHIP_EXPORT_FLAG})

LDPPFLAGS := ${LDPPFLAGS} ${foreach tmpFlag, ${CHIP_EXPORT_FLAG}, -D${tmpFlag}}

# Exports CHIP Memory parameters to the linker script (specified in platform/chip/defs/xxx.def)
LDPPFLAGS += \
	-DFLASH_BASE=${FLASH_BASE} \
	-DFLASH_BOOT_OFFSET=${FLASH_BOOT_OFFSET} \
	-DEXT_RAM_BASE=${EXT_RAM_BASE} \
	-DSYS_SRAM_BASE=${SYS_SRAM_BASE} \
	-DSYS_SRAM_AVAILABLE_BASE=${SYS_SRAM_AVAILABLE_BASE} \
	-DSYS_SRAM_SIZE=${SYS_SRAM_SIZE} \
	-DSYS_SRAM_END=${SYS_SRAM_END} \
	-DSYS_SRAM_IRQ_STACKWATCH_SIZE=${SYS_SRAM_IRQ_STACKWATCH_SIZE} \
	-DXCPU_IRQ_STACK_SIZE=${XCPU_IRQ_STACK_SIZE} \
	-DSYS_PROXY_MEM_BASE=${SYS_PROXY_MEM_BASE} \
	-DSYS_PROXY_MEM_SIZE=${SYS_PROXY_MEM_SIZE} \
	-DBB_SRAM_BASE=${BB_SRAM_BASE} \
	-DBB_SRAM_SIZE=${BB_SRAM_SIZE} \
	-DBB_SRAM_GLOBALS_SIZE=${BB_SRAM_GLOBALS_SIZE} \
	-DRESERVED_PATCH_SIZE=${RESERVED_PATCH_SIZE} \
	-DBB_DUALPORT_SRAM_BASE=${BB_DUALPORT_SRAM_BASE} \
	-DBB_DUALPORT_SRAM_SIZE=${BB_DUALPORT_SRAM_SIZE} \
	-DRESERVED_MBX_SIZE=${RESERVED_MBX_SIZE} \
	-DINT_ROM_BASE=${INT_ROM_BASE} \
	-DINT_ROM_END=${INT_ROM_END} \
	-DROM_MISC_BASE=${ROM_MISC_BASE} \
	-DROM_MISC_UBASE=${ROM_MISC_UBASE} \
	-DROM_MISC_SIZE=${ROM_MISC_SIZE}

ifeq "${BCPUGEN}"  "yes"
BCPU_CODE_BASE              ?= 0x1100
BCPU_RAMRUN_CODE_BASE       ?= 0x1500

LDPPFLAGS += \
    -DBCPU_ROM_BASE=${BCPU_ROM_BASE} \
    -DBCPU_ROM_END=${BCPU_ROM_END} \
    -DBCPU_SYS_SRAM_BASE=${BCPU_SYS_SRAM_BASE}

ifeq "${CHIP_HAS_AP}" "1"
LDPPFLAGS += -DBCPU_CODE_BASE=${EXT_RAM_BASE}+${BCPU_CODE_BASE}
else
LDPPFLAGS += -DBCPU_CODE_BASE=${FLASH_BASE}+${BCPU_CODE_BASE}+${FLASH_BOOT_OFFSET}
endif

ifeq "${CT_RAMRUN}" "1"
LDPPFLAGS += -DBCPU_RAMRUN_CODE_BASE=${EXT_RAM_BASE}+${BCPU_RAMRUN_CODE_BASE}
endif
endif # BCPUGEN

ifeq "${CHIP_HAS_BTCPU}" "1"
LDPPFLAGS += \
    -DBTCPU_ROM_BASE=${BTCPU_ROM_BASE}
endif

ifeq "${CHIP_HAS_AP}" "1"
LDPPFLAGS += \
	-DAP_DUALPORT_SRAM_BASE=${AP_DUALPORT_SRAM_BASE} \
	-DAP_DUALPORT_SRAM_SIZE=${AP_DUALPORT_SRAM_SIZE}  
endif

ifeq "${CHIP_HAS_TMCU}" "1"
LDPPFLAGS += \
	-DCHIP_HAS_TMCU=${CHIP_HAS_TMCU} \
	-DTMCU_DM_SRAM_BASE=${TMCU_DM_SRAM_BASE} \
	-DTMCU_PM_SRAM_BASE=${TMCU_PM_SRAM_BASE}  \
	-DTMCU_IFCBUF_ADDR=${TMCU_IFCBUF_ADDR}  \
	-DTMCU_IFCBUF_FCCHADDR=${TMCU_IFCBUF_FCCHADDR}  \
	-DTMCU_DM_SRAM_SIZE=${TMCU_DM_SRAM_SIZE} \
	-DTMCU_PM_SRAM_SIZE=${TMCU_PM_SRAM_SIZE}  \
	-DTMCU_SHARE_BBSRAM1_BASE=${TMCU_SHARE_BBSRAM1_BASE} \
	-DTMCU_SHARE_BBSRAM2_BASE=${TMCU_SHARE_BBSRAM2_BASE}  \
	-DTMCU_SHARE_PMSRAM1_BASE=${TMCU_SHARE_PMSRAM1_BASE} \
	-DTMCU_SHARE_PMSRAM2_BASE=${TMCU_SHARE_PMSRAM2_BASE}  \
	-DTMCU_SHARE_BBSRAM1_SIZE=${TMCU_SHARE_BBSRAM1_SIZE} \
	-DTMCU_SHARE_BBSRAM2_SIZE=${TMCU_SHARE_BBSRAM2_SIZE}  \
	-DTMCU_SHARE_PMSRAM1_SIZE=${TMCU_SHARE_PMSRAM1_SIZE} \
	-DTMCU_SHARE_PMSRAM2_SIZE=${TMCU_SHARE_PMSRAM2_SIZE}  
endif

ifeq "${CHIP_HAS_WCPU}" "1"
LDPPFLAGS += \
	-DWD_DUALPORT_SRAM_BASE=${WD_DUALPORT_SRAM_BASE} \
	-DWD_ROM_BASE=${WD_ROM_BASE} \
	-DWD_RAM_BASE=${WD_RAM_BASE} \
	-DWD_DUALPORT_SRAM_SIZE=${WD_DUALPORT_SRAM_SIZE}
endif

# Exports NAND flash parameters to the linker script
NAND_FLASH ?= 0
LDPPFLAGS += \
	-DNAND_FLASH=${NAND_FLASH} \
	-DNAND_FLASH_BOOT_CODE_SIZE=${NAND_FLASH_BOOT_CODE_SIZE}

# Native JAVA
ifeq "${NATIVE_JAVA_SUPPORT}" "yes" 
LDPPFLAGS += -D__NATIVE_JAVA__
endif

ifeq "${WAP_CACHE_IN_TFLASH}" "y"
LDPPFLAGS += -DWAP_CACHE_IN_TFLASH
endif
# Compression parameters
LINK_STAGE_2 ?= no
ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"
LDPPFLAGS += -DCOMPRESS_PROCESS_ENABLE

ifeq "${CT_COMPRESS_CODE_SECTION}" "y"
MYCPPFLAGS += -D__MMI_COMPRESS_CODE_SECTION__
LDPPFLAGS += -DCT_COMPRESS_CODE_SECTION \
	-DRAM_PHY_SIZE=${RAM_PHY_SIZE} \
	-DXCPU_OVERLAY_BASE=${EXT_RAM_BASE}+${XCPU_OVERLAY_BASE} \

ifeq "${COMPRESS_STATE}" "3264"
LDPPFLAGS += -DCOMPRESS_3264
endif
ifeq "${COMPRESS_STATE}" "6464"
LDPPFLAGS += -DCOMPRESS_6464
endif
ifeq "${COMPRESS_STATE}" "3232"
LDPPFLAGS += -DCOMPRESS_3232
endif
ifeq "${COMPRESS_STATE}" "3232_NO_OVERLAY"
LDPPFLAGS += -DCOMPRESS_3232_NO_OVERLAY
endif
ifeq "${COMPRESS_STATE}" "QVGA3232"
LDPPFLAGS += -DCOMPRESS_QVGA3232
endif
ifeq "${COMPRESS_STATE}" "SLIMPDA6464"
LDPPFLAGS += -DCOMPRESS_SLIMPDA6464
endif
endif # CT_COMPRESS_CODE_SECTION

ifeq "${CT_COMPRESS_DATA_SECTION}" "yes"
MYCPPFLAGS += -D__MMI_COMPRESS_DATA_SECTION__
LDPPFLAGS += -DCT_COMPRESS_DATA_SECTION
endif # CT_COMPRESS_DATA_SECTION

ifeq "${CT_COMPRESS_PREMAIN}" "y"
LDPPFLAGS += -DCOMPRESS_PREMAIN
endif

ifeq "${LINK_STAGE_2}" "yes"
LDPPFLAGS += -DLINK_STAGE_2
endif # LINK_STAGE_2
endif # COMPRESS_PROCESS_ENABLE

# Export Misc Parameters to the linker script.
LDPPFLAGS += \
	-DUSE_BINUTILS_2_19=${USE_BINUTILS_2_19} \
	-DDISCARD_UNLISTED_SECTIONS=${DISCARD_UNLISTED_SECTIONS} \

# Sector cleaner flags
ifneq "${CLEAN_USER_SECTOR}${CLEAN_CALIB_SECTOR}${CLEAN_FACT_SECTOR}" "000"
LDPPFLAGS += \
	-DCLEAN_USER_SECTOR=${CLEAN_USER_SECTOR} \
	-DCLEAN_CALIB_SECTOR=${CLEAN_CALIB_SECTOR} \
	-DCLEAN_FACT_SECTOR=${CLEAN_FACT_SECTOR}
endif

# SPI register debug
ifeq "${SPI_REG_DEBUG}" "y"
LDPPFLAGS += -DSPI_REG_DEBUG
endif

# I2C register debug
ifeq "${I2C_REG_DEBUG}" "y"
LDPPFLAGS += -DI2C_REG_DEBUG
endif

# NBCPU
ifneq "${filter FPGA_NBCPU,${ALL_EXPORT_FLAG}}" ""
LDPPFLAGS += -DFPGA_NBCPU
LDPPFLAGS += \
	-DNB_SRAM_BASE=${NB_SRAM_BASE} \
	-DNB_SRAM_SIZE=${NB_SRAM_SIZE} \
	-DNB_SRAM_GLOBALS_SIZE=${NB_SRAM_GLOBALS_SIZE} \
	-DNB_RESERVED_PATCH_SIZE=${NB_RESERVED_PATCH_SIZE} \
	-DNB_SRAM_PATCH_SIZE=${NB_SRAM_PATCH_SIZE}
endif


ifeq "${BUILD_BCPU_PSRAM}" "1"
LDPPFLAGS += -DBUILD_BCPU_PSRAM
endif
ifeq "${CT_ASICSIMU}" "1"
LDPPFLAGS += -DASICSIMU
endif

ifneq "${CT_RAMRUN}" "1"
# Put Const into ROM
# Code is either in RAM or ROM
ifneq "${WITH_FLASH}" "1" 
LDPPFLAGS += -DCODE_IN_RAM
endif
else # CT_RAMRUN
ifeq "${CHIP_HAS_AP}" "1"
# Modem codes are already located in ram. Special handling on
# the booting sections and the data sections only.
LDPPFLAGS += -DRAMRUN
else
LDPPFLAGS += -DCONST_IN_RAM -DCODE_IN_RAM -DPRELOADED_INT_SRAM -DPRELOADED_RAM -DPRELOADED_PROXY_MEMORY -DRAMRUN
endif
MYCPPFLAGS += -DRAMRUN
endif # CT_RAMRUN

ifeq "${USE_BINUTILS_2_19}" "1"
ifeq "${ENABLE_GC_SECTIONS}" "1"
# Should be used along with GCC options -ffunction-sections, -fdata-sections
LDFLAGS += --gc-sections
ifeq "${PRINT_GC_SECTIONS}" "1"
LDFLAGS += --print-gc-sections
endif # PRINT_GC_SECTIONS
endif # ENABLE_GC_SECTIONS
endif # USE_BINUTILS_2_19

ifeq "${CT_USE_LIBSTDC++}" "1"
LDFLAGS		+= ${FULL_LIBRARY_PATH} --start-group ${MYLIBFILES} ${FULL_LIBRARY_EXT} -lgcc -lstdc++ --end-group
LDFLAGS		+= -L${LIBGCC_PATH} 
else
LDFLAGS		+= ${FULL_LIBRARY_PATH} --start-group ${MYLIBFILES} ${FULL_LIBRARY_EXT} --end-group
LDFLAGS		+= -L${LIBGCC_PATH} -lgcc
endif
ifeq "${IS_DLL_ENTRY}" "yes"
LDFLAGS		+= --whole-archive --start-group ${MYLIBFILES} ${FULL_LIBRARY_EXT} --end-group --warn-unresolved-symbols -shared --export-dynamic
else
LDFLAGS		+= --warn-common  --error-unresolved-symbols 
endif

ifeq "${PROTOSTACK_IN_RAM}" "1"
LDPPFLAGS += -DPROTOSTACK_IN_RAM
endif


#--warn-section-align

##################################################################################
## Compile flags
# Some of those are exported recursively, other not.
##################################################################################

# Assembly compilation flags
ifeq "$(CT_COMPILER)" "gcc7_xcpu2"
ASFLAGS += -march=xcpu2 -mtune=xcpu2 -EL -msoft-float
else
ASFLAGS += -march=xcpu -mtune=xcpu -EL
endif

# Assembly preprocessing flags
ASCPPFLAGS  += -DCT_ASM
ifeq "${DLL_SUPPORT}" "yes"
ASCPPFLAGS  += -DCT_C_USE_GP
endif

# We disable the feature of assembling mips16 assembly code, 
# as assembling code with the --gstabs debug feature is 
# broken: mips16 code is interpreted as mips32, which leads
# to an exception.
# ie: Don't write mips16 assembly files.
# Since -gstab is removed, this limitation is not anymore...
#ifneq (,${findstring mips16,${CT_MIPS16_CFLAGS}})
# This file is mips16
#ASCPPFLAGS += -DCT_ASM_MIPS16
#endif # CT_MIPS16_CFLAGS

# C/C++ compilation flags

# User flags
# The local export flags are defined in the Makefile.
ALL_EXPORT_FLAG := $(sort ${ALL_EXPORT_FLAG} ${LOCAL_EXPORT_FLAG})

MYCPPFLAGS := ${MYCPPFLAGS} ${foreach tmpFlag, ${ALL_EXPORT_FLAG}, -D${tmpFlag}}



# ------------------------------------------------------------------------
# Depending on the release type, set the corresponding compilation flags
# ------------------------------------------------------------------------
ifeq "${CT_RELEASE}" "release"
MYCPPFLAGS += -DCT_NO_DEBUG
MYCPPFLAGS += -DHAL_GDB_CONFIG
CT_OPT ?= speed
endif

ifeq "${CT_RELEASE}" "debug"
CT_OPT ?= dbg_opt
endif

ifeq "${CT_RELEASE}" "cool_profile"
MYCPPFLAGS += -DCT_PROFILING
CT_OPT ?= dbg_opt
endif

# Only useful for Jade, not for Modem2G.
ifeq "${CT_RELEASE}" "calib"
MYCPPFLAGS += -DPAL_CALIB
CT_OPT ?= dbg_opt
endif

# Old stuff for SA profiling, might not be used anymore.
ifeq "${CT_RELEASE}" "sa_profile"
MYCPPFLAGS += -DSA_PROFILING
CT_OPT ?= dbg_opt
endif



# ------------------------------------------------------------------------
# Warning flags
# ------------------------------------------------------------------------

ifneq "${WITHOUT_WSIGN_COMPARE}" "1"
ifeq "${WITH_WSIGN_COMPARE}" "1"
CFLAGS      += -Wsign-compare
endif
endif

ifneq "${WITHOUT_WERROR}" "y"
CFLAGS      += -Werror
endif

ifneq "${WITHOUT_WSHADOW}" "1"
CFLAGS      += -Wshadow
endif

ifeq "${USE_GCC_4}" "1"
# -finline-small-functions, by default on -O2 and -Os
#  makes code bigger. We disable it explicitly when
#  when we want small code.
#
# Nota: In the ROM we also don't want inlined function !
CFLAGS_EXTRA_SIZE_REDUCE := -fno-inline-small-functions -fno-inline-functions \
							-fno-align-functions  -fno-align-jumps -fno-align-loops -fno-align-labels 

# A good limit value for inlining function size: -finline-limit=6 

else
CFLAGS_EXTRA_SIZE_REDUCE := 
endif



# -----------------------------------
# Debug
# -----------------------------------
ifeq "${CT_OPT}" "dbg"
CFLAGS		+= -g
ASFLAGS		+= --gstabs
#MYCPPFLAGS	+=
else 
ifeq "${CT_OPT}" "dbg_opt"
CFLAGS		+= -O2 -g -falign-functions=16 -falign-jumps=16
#ASFLAGS		+= --gstabs
#MYCPPFLAGS	+=
else
ifeq "${CT_OPT}" "dbg_size"
CFLAGS		+= -Os -g  ${CFLAGS_EXTRA_SIZE_REDUCE}
else
ifeq "${CT_OPT}" "size"
CFLAGS		+= -Os  ${CFLAGS_EXTRA_SIZE_REDUCE}
#ASFLAGS    += 
#MYCPPFLAGS	+=
else #"${CT_OPT}" "speed"
CFLAGS		+= -O2 -falign-functions=16 -falign-jumps=16 
#ASFLAGS    += 
#MYCPPFLAGS	+=
endif # "size"
endif # "dbg_size"
endif # "dbg_opt"
endif # "dbg"

MYCPPFLAGS  += -D__NEW_GCC__

# Whether to call functions outside 256M address range.
# 8809 BCPU ROM codes are uncached (for TCM access), and 
# BCPU flash codes should have long call capability.
ifeq "${BUILD_BCPU_IMAGE}" "1"
ifneq "${BUILD_ROM}" "1"
CFLAGS += -mlong-calls
endif
endif

# We don't have a FPU, so we need to use the software implementation (with the
# integers units) of the floating point operations.
# Nota: we SHOULD implement algorithms using integers for perf issues
CFLAGS += -msoft-float

# Warn against undefined identifiers in '#if' but outside of 'defined'
# Warn against unused static functions
CFLAGS += -Wundef -Wunused-function
C_SPECIFIC_CFLAGS    += -std=gnu99

ifeq "${USE_GCC_4}" "1"
CFLAGS               += -Wuninitialized
CFLAGS += -minterlink-mips16 -fno-strict-aliasing
ifeq "${ENABLE_GC_SECTIONS}" "1"
# Should be used along with LD option --gc-sections 
CFLAGS += -ffunction-sections -fdata-sections -fno-common
endif # ENABLE_GC_SECTIONS
# Create a warning when compiling C++ flags
C_SPECIFIC_CFLAGS += -Wno-pointer-sign 
C++_SPECIFIC_CFLAGS +=  -Wno-write-strings
# Discard conflicting builtin functions reimplemented by the code
CFLAGS += -fno-builtin-iswspace
# Might be useful for wide char stuff: -fno-builtin-iswalnum -fno-builtin-iswalpha -fno-builtin-iswascii -fno-builtin-iswcntrl -fno-builtin-iswdigit -fno-builtin-iswgraph -fno-builtin-iswlower -fno-builtin-iswprint -fno-builtin_iswpunct -fno-builtin-iswspace -fno-builtin-iswupper -fno-builtin-iswxdigit -fno-builtin-towlower -fno-builtin-towupper     and if scanf is detected, enable -fno-builtin-fscanf -fno-builtin-scanf -fno-builtin-sscanf 
MYCPPFLAGS +=  -DUSE_GCC_4=1 -DUSE_BINUTILS_2_19=1
else # USE_GCC_4 != "1"
# We're using GCC 3, with support for march/mtune flags, etc...
# which doesn't natively support interleaving of mips16 and mips32.
CFLAGS += -mfix-abijump
endif # USE_GCC_4 != "1"

# -----------------------------------
# Reduced regs management
# -----------------------------------
#  List of registers not used
REDUCED_REGS_FLAG ?= -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7 -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7 -ffixed-fp

ifeq "${BUILD_BCPU_IMAGE}" "1"
WITH_REDUCED_REGS := 0
else
ifeq "${BUILD_ROM}" "1"
WITH_REDUCED_REGS := 1
else
# ABI is incompatible for WITH_REDUCED_REGS 0/1. So, it can't be changed
ifeq "$(CT_COMPILER)" "gcc4_xcpu"
WITH_REDUCED_REGS := 1
else
WITH_REDUCED_REGS := 0
endif
endif
endif
ifeq  "${WITH_REDUCED_REGS}" "1" 
MYCPPFLAGS +=  -D__REDUCED_REGS__ 
CFLAGS += ${REDUCED_REGS_FLAG}
endif # WITH_REDUCED_REGS

ifeq "${EXTRA_WARNING}" "1"
CFLAGS += -Wextra 
export EXTRA_WARNING
endif

# -----------------------------------
# Final values for compile flags
# -----------------------------------
CPPFLAGS = ${INCLUDE_PATH} -DEL ${MYCPPFLAGS} -DBUILD_DATE=${BUILD_DATE} -D_T_MMI=
EXTERN_CPPFLAGS = -fno-gnu-keywords -fno-operator-names -fno-exceptions -fno-optional-diags -fno-rtti -fno-threadsafe-statics

ifeq "$(CT_COMPILER)" "gcc7_xcpu2"
# Maybe -fpermissive is needed, but it is bad
CFLAGS += -G0 -Wall -march=xcpu2 -mtune=xcpu2 -Wa,-march=xcpu2,-mtune=xcpu2 -EL -mexplicit-relocs -fweb -frename-registers -mmemcpy -nostartfiles -nostdlib -nostdinc -nodefaultlibs -Wno-error=unused-but-set-variable -Wno-error=unused-const-variable -c -pipe -fsigned-char
else
CFLAGS += -G0 -Wall -march=xcpu -mtune=xcpu -Wa,-march=xcpu,-mtune=xcpu -EL -mexplicit-relocs -fweb -frename-registers -mmemcpy -mmips-tfile -nostartfiles -nostdlib -nostdinc -nodefaultlibs -c -pipe 
endif

ifneq "${NO_WIDE_CHARSET}" "1"
CFLAGS += -fwide-exec-charset=UTF-16LE -fshort-wchar
endif

CFLAGS += -include fpconfig.h
CPPFLAGS += -include fpconfig.h

# ------------------------------------------------------------------------
# Export to recursive calls in dependencies
# ------------------------------------------------------------------------
export CT_RELEASE
export CT_TARGET
export CT_ASIC
export CT_ASIC_CAPD
export CT_ASIC_CFG
export CT_OS
export CT_ROMULATOR

# Exported flags and include path are exported.
# The global, target and chip export flags are not exported because
# they are included by all Makefiles, no need to export them.
export LOCAL_EXPORT_FLAG


###############################################################################
# Sources & Objects
###############################################################################
# No need to specify searching paths here, as
# 1. ${LOCAL_SRC_DIR} is the only valid location (a relative path) for
#    source files in implicit/pattern rules;
# 2. nobody uses ${LOCAL_SUBDIRS};
# 3. vpath for object files will cause trouble in directory removal.
#    See the comment in target "locallclean".
#vpath %.c . ${LOCAL_SRC_DIR}
#vpath %.cpp . ${LOCAL_SRC_DIR}
#vpath %.S . ${LOCAL_SUBDIRS} ${LOCAL_SRC_DIR}
#vpath %.o 	${OBJ_REL_PATH}
#vpath %.h . ${LOCAL_SRC_DIR} ${LOCAL_INC_DIR}

# The list of objects is derived from the source files, 
# *plus* the contents of LOCAL_OBJS, so we can have e.g.
# a binary only main.o...

USED_SRC_IN_ROM ?= ${filter-out ${BYPASSED_SRC_IN_ROM}, ${SRC_IN_ROM}}
USED_SRC_IN_BCPU_ROM ?= ${filter-out ${BYPASSED_SRC_IN_BCPU_ROM}, ${SRC_IN_BCPU_ROM}}

ifeq "${BUILD_BCPU_IMAGE}" "1"
USED_SRC_IN_ROM := ${USED_SRC_IN_BCPU_ROM}

ifeq "${IS_ENTRY_POINT}" "yes"
C_SRC := ${BCPU_C_SRC}
S_SRC := ${BCPU_S_SRC}
C++_SRC := ${BCPU_C++_SRC}
endif
endif # BUILD_BCPU_IMAGE

ifeq "${BUILD_RAMINIT_IMAGE}" "1"
ifeq "${IS_ENTRY_POINT}" "yes"
C_SRC := ${RAMINIT_C_SRC}
S_SRC := ${RAMINIT_S_SRC}
C++_SRC := ${RAMINIT_C++_SRC}
endif
endif # BUILD_BCPU_IMAGE

# Remove from the list of files to compile the files that are already in ROM
C_SRC_FILTERED := ${filter-out ${USED_SRC_IN_ROM}, ${C_SRC} }
S_SRC_FILTERED := ${filter-out ${USED_SRC_IN_ROM}, ${S_SRC} }
C++_SRC_FILTERED := ${filter-out ${USED_SRC_IN_ROM}, ${C++_SRC} }

ASFILES			:=	${patsubst %.S,%.asm,${S_SRC_FILTERED}}

# Full List of Objects that have to be generated from src (i.e.must be compiled)
FULL_SRC_OBJECTS :=	${LOCAL_OBJS} \
				    ${patsubst %.S,%.o,${S_SRC_FILTERED}}			\
				    ${patsubst %.c,%.o,${C_SRC_FILTERED}}			\
				    ${patsubst %.cpp,%.o,${C++_SRC_FILTERED}}		
FULL_SRC_OBJECTS := ${foreach obj, ${FULL_SRC_OBJECTS},${OBJ_REL_PATH}/${obj}}

# The list of objects that must be built regardless of the modification time
SRC_OBJECTS_FORCE_BUILD :=
ifneq "${LOCAL_SRC_FORCE_BUILD}" ""
ifneq "${filter-out ${USED_SRC_IN_ROM}, ${LOCAL_SRC_FORCE_BUILD}}" "${LOCAL_SRC_FORCE_BUILD}"
$(warning LOCAL_SRC_FORCE_BUILD contains ROM source files: ${LOCAL_SRC_FORCE_BUILD})
endif
ifneq "${filter-out ${C_SRC} ${S_SRC} ${C++_SRC}, ${LOCAL_SRC_FORCE_BUILD}}" ""
$(warning LOCAL_SRC_FORCE_BUILD contains useless files: ${LOCAL_SRC_FORCE_BUILD})
endif
SRC_OBJECTS_FORCE_BUILD := $(sort ${LOCAL_SRC_FORCE_BUILD})
SRC_OBJECTS_FORCE_BUILD := ${patsubst %.S,%.o,${SRC_OBJECTS_FORCE_BUILD}}
SRC_OBJECTS_FORCE_BUILD := ${patsubst %.c,%.o,${SRC_OBJECTS_FORCE_BUILD}}
SRC_OBJECTS_FORCE_BUILD := ${patsubst %.cpp,%.o,${SRC_OBJECTS_FORCE_BUILD}}
SRC_OBJECTS_FORCE_BUILD := ${foreach obj,${SRC_OBJECTS_FORCE_BUILD},${OBJ_REL_PATH}/${obj}}
endif # LOCAL_SRC_FORCE_BUILD

##############################################################################
## General Targets
##############################################################################
LOCAL_LIBRARY_NOTDIR := lib${MODULE_NAME}_${CT_RELEASE}.a
LOCAL_SRCLIBRARY := ${LIB_PATH}/${LOCAL_LIBRARY_NOTDIR}
# This one can be overwritten by packed libraries.
LOCAL_BINLIBRARY ?= ${BINLIB_PATH}/${LOCAL_LIBRARY_NOTDIR}

# Allow name suffixing for all final binary targets, to keep history/variations.
ifneq "$(LOCAL_BATCH_NAME)" ""
NBR := ${LOCAL_BATCH_NAME}_
else 
NBR :=
endif # LOCAL_BATCH_NAME

ifneq "$(CT_PRODUCT)" ""
PRODUCT := ${CT_PRODUCT}_
else
PRODUCT :=
endif # CT_PRODUCT

# Build the base pathname of the generated lodfile.
ifeq "${CT_BIN}" "1"
LODBASE_NO_PATH := ${NBR}${CT_TARGET}_${PRODUCT}${CT_RELEASE}_bin
else
LODBASE_NO_PATH := ${NBR}${CT_TARGET}_${PRODUCT}${CT_RELEASE}
endif

# Path for storing all the generated files for one test (elf, dis, lod...).
BINARY_PATH := ${HEX_ROOT}/${LODBASE_NO_PATH}

# "_" is a special name for building default application
ifneq "$(CT_APP)" "_"
ifeq "$(CT_APP)" ""
LODBASE_NO_PATH := $(notdir ${LOCAL_NAME})_${LODBASE_NO_PATH}
else
LODBASE_NO_PATH := ${CT_APP}_${LODBASE_NO_PATH}
endif # CT_APP != <null>
endif # CT_APP != _

ifeq "${BCPUGEN}"  "yes"
BCPU_LODBASE_NO_PATH := bcpu_${LODBASE_NO_PATH}
endif

ifeq "${RAMINITGEN}"  "yes"
RAMINIT_LODBASE_NO_PATH := raminit_${LODBASE_NO_PATH}
endif

# Change the location where everything is compiled.
BIN_PATH := ${BUILD_ROOT}/${LOCAL_NAME}

# If building BCPU image, change the lod base name, but NOT to change BINARY_PATH !
ifeq "${BUILD_BCPU_IMAGE}" "1"
LODBASE_NO_PATH := ${BCPU_LODBASE_NO_PATH}
endif

# If building RAMINIT image, change the lod base name, but NOT to change BINARY_PATH !
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
LODBASE_NO_PATH := ${RAMINIT_LODBASE_NO_PATH}
endif

BAS := ${BIN_PATH}/${LODBASE_NO_PATH}
BIN := ${BAS}.elf
MAP := ${BAS}.map
HEX := ${BAS}.srec
DIS := ${BAS}.dis
DLL := ${BAS}.dll
BAS_FINAL := ${BINARY_PATH}/${LODBASE_NO_PATH}
BIN_FINAL := ${BAS_FINAL}.elf
MAP_FINAL := ${BAS_FINAL}.map
HEX_FINAL := ${BAS_FINAL}.srec
DIS_FINAL := ${BAS_FINAL}.dis
XMD_FINAL := ${BAS_FINAL}.xmd
XCP_FINAL := ${BAS_FINAL}.xcp
DLL_FINAL := ${BAS_FINAL}.dll
LODBASE   := ${BAS_FINAL}_

ifeq "${BCPUGEN}"  "yes"
BCPU_BIN_PATH := ${BCPU_BUILD_ROOT}/${LOCAL_NAME}
BCPU_BAS := ${BCPU_BIN_PATH}/${BCPU_LODBASE_NO_PATH}
BCPU_BIN := ${BCPU_BAS}.elf
BCPU_BAS_FINAL := ${BINARY_PATH}/${BCPU_LODBASE_NO_PATH}
BCPU_BIN_FINAL := ${BCPU_BAS_FINAL}.elf-bcpu
BCPU_MAP_FINAL := ${BCPU_BAS_FINAL}.map-bcpu
endif

ifeq "${BUILD_BCPU_IMAGE}" "1"
BIN_FINAL := ${BCPU_BIN_FINAL}
MAP_FINAL := ${BCPU_MAP_FINAL}
endif

ifeq "${RAMINITGEN}"  "yes"
RAMINIT_BIN_PATH := ${RAMINIT_BUILD_ROOT}/${LOCAL_NAME}
RAMINIT_BAS := ${RAMINIT_BIN_PATH}/${RAMINIT_LODBASE_NO_PATH}
RAMINIT_BIN := ${RAMINIT_BAS}.elf
RAMINIT_BAS_FINAL := ${BINARY_PATH}/${RAMINIT_LODBASE_NO_PATH}
RAMINIT_BIN_FINAL := ${RAMINIT_BAS_FINAL}.elf-raminit
RAMINIT_MAP_FINAL := ${RAMINIT_BAS_FINAL}.map-raminit
endif

ifeq "${BUILD_RAMINIT_IMAGE}" "1"
BIN_FINAL := ${RAMINIT_BIN_FINAL}
MAP_FINAL := ${RAMINIT_MAP_FINAL}
endif

COMPRESSED_DATA_OBJECTS :=
COMPRESSED_DATA_OBJECTS_CHECKSUM :=
ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"
STAGE1_POSTFIX := -stage1
STAGE1_LINKABLE_POSTFIX := ${STAGE1_POSTFIX}-linkable
BIN_STAGE1_LINKABLE := ${BAS}.elf${STAGE1_LINKABLE_POSTFIX}
BIN_STAGE1 := ${BAS}.elf${STAGE1_POSTFIX}
MAP_STAGE1 := ${BAS}.map${STAGE1_POSTFIX}
BIN_FINAL_STAGE1 := ${BAS_FINAL}.elf${STAGE1_POSTFIX}
MAP_FINAL_STAGE1 := ${BAS_FINAL}.map${STAGE1_POSTFIX}
LD_FILE_STAGE1 := ${LD_FILE}${STAGE1_POSTFIX}
COMPRESSED_STUB_SRC_DIR := ${SOFT_WORKDIR}/application/coolmmi/mmi/cdflashus/compressed_stub
COMPRESSED_DATA_OBJECTS += ${BIN_PATH}/get_compressed_data.o
ifneq "${LINK_STAGE_2}" "yes"
BIN := ${BIN_STAGE1}
MAP := ${MAP_STAGE1}
BIN_FINAL := ${BIN_FINAL_STAGE1}
MAP_FINAL := ${MAP_FINAL_STAGE1}
LD_FILE := ${LD_FILE_STAGE1}
else # LINK_STAGE_2 = yes
COMPRESSED_SECTION_LIST :=
include ${COMPRESSED_STUB_SRC_DIR}/compress_section_list.cfg
ifeq "${COMPRESS_PREMAIN_ENABLE}" "yes"
COMPRESSED_SECTION_LIST += xcpu_premain
endif
COMPRESSED_DATA_OBJECTS += ${foreach section,${COMPRESSED_SECTION_LIST},${BIN_PATH}/${section}.bin.lzma.o}
COMPRESSED_DATA_OBJECTS_CHECKSUM += ${foreach section,${COMPRESSED_SECTION_LIST},${BIN_PATH}/${section}.sum.o}
endif # LINK_STAGE_2 = yes
endif # COMPRESS_PROCESS_ENABLE = yes

USR_DATA_OBJECTS :=
ifeq "${USERGEN}" "yes"
# No compression or compression stage 2
ifneq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-no"
USR_DATA_OBJECTS += ${BIN_PATH}/user.bin.o
endif
endif

BUILD_XCP_PATH := ${BUILD_ROOT}/xcp

EXP := ${LOCAL_DLL_EXPORT}

# The default target is either a library or a srec file, depending on IS_ENTRY_POINT
ifeq "${IS_ENTRY_POINT}" "yes"
	TOP_TARGET := ${HEX}
else
	TOP_TARGET := ${LOCAL_SRCLIBRARY}
endif # IS_ENTRY_POINT

ifeq "${IS_DLL_ENTRY}" "yes"
	TOP_TARGET := ${DLL}
endif # IS_DLL_ENTRY

# List of directories that may need to be created
ifneq "${IS_ENTRY_POINT}" "yes"
MAKE_DIRS := ${OBJ_REL_PATH} ${DEPS_REL_PATH} ${BUILD_XCP_PATH} ${LOCAL_LIB_PATH}
else
MAKE_DIRS := ${OBJ_REL_PATH} ${DEPS_REL_PATH} ${BUILD_XCP_PATH}
endif # IS_ENTRY_POINT


###############################################################################
# BCPU elf file (maybe including BCPU ROM elf file)
###############################################################################
ifeq "${BCPUGEN}"  "yes"
ifneq "${BUILD_BCPU_IMAGE}" "1"
ifneq "${BUILD_ROM}" "1"
# References between BCPU ROM and XCPU ROM are forbidden at present.
# Note that if XCPU ROM references to BCPU ROM are allowed,
# 1) when building XCPU ROM, BCPU ROM image is not in BCPU_ROM_ELF_FILE_FULL yet,
# but in BCPU_ELF_FILE_FULL;
# 2) when building XCPU flash image with ROM_IN_FLASH_ELF enabled, BCPU ROM symbols
# should be stripped from XCPU ROM before linking stage.

ifneq "$(wildcard ${BCPU_BIN})" ""

BCPU_ELF_FILE_FULL := ${BCPU_BIN}
# XCPU flash elf does NOT need to link BCPU ROM, as it will link BCPU flash elf,
# and BCPU ROM symbols have been linked into BCPU flash elf.

ifneq "${STRIP_BCPU_OBJ2STRIP_FILE}" ""
STRIP_BCPU_OBJ2STRIP_FILE_PATHED := ${SOFT_WORKDIR}/${STRIP_BCPU_OBJ2STRIP_FILE}
BCPU_ELF_FILE_STRIPPED := ${BCPU_BIN_PATH}/stripped_bcpu_elf_file.elf
USED_BCPU_ELF_FILE := ${BCPU_ELF_FILE_STRIPPED}
else # !STRIP_BCPU_OBJ2STRIP_FILE
USED_BCPU_ELF_FILE := ${BCPU_ELF_FILE_FULL}
endif # !STRIP_BCPU_OBJ2STRIP_FILE

# Load the content of BCPU elf file into XCPU elf
LDPPFLAGS += -DLOAD_BCPU_ELF
LDFLAG_USED_ELF_FILES += ${USED_BCPU_ELF_FILE}

endif # BCPU_BIN

endif # !BUILD_ROM
endif # !BUILD_BCPU_IMAGE
endif # BCPUGEN
# ------------------------------
# End of BCPU elf file
# ------------------------------


###############################################################################
# RAMINIT elf file
###############################################################################
# RAMINIT elf is linked to XCPU elf only
ifneq "${BUILD_BCPU_IMAGE}" "1"
ifeq "${RAMINITGEN}"  "yes"
ifneq "${BUILD_RAMINIT_IMAGE}" "1"
ifneq "${BUILD_ROM}" "1"

ifneq "$(wildcard ${RAMINIT_BIN})" ""

RAMINIT_ELF_FILE_FULL := ${RAMINIT_BIN}

USED_RAMINIT_ELF_FILE := ${RAMINIT_ELF_FILE_FULL}

# Load the content of RAMINIT elf file into XCPU elf
LDPPFLAGS += -DLOAD_RAMINIT_ELF
LDFLAG_USED_ELF_FILES += ${USED_RAMINIT_ELF_FILE}

endif # RAMINIT_BIN

endif # !BUILD_ROM
endif # !BUILD_RAMINIT_IMAGE
endif # RAMINITGEN
endif # !BUILD_BCPU_IMAGE
# ------------------------------
# End of RAMINIT elf file
# ------------------------------


########################## Default Target ###################################
.PHONY: default

ifeq "${IS_DLL_ENTRY}" "yes"
default: dll
else
ifeq "${IS_ENTRY_POINT}" "yes"
default: lod
else
default: all
endif
endif


.PHONY: force
force: ;


#####################################################################
# Error management
#####################################################################
.PHONY: my_err
my_err: 
	$(error Make ERROR)


########################## Directories #######################################

${LOCAL_LIB_PATH}:
	mkdir -p ${LOCAL_LIB_PATH}

ifneq "${BINLIB_PATH}" "${LOCAL_LIB_PATH}"
${BINLIB_PATH}:
	mkdir -p ${BINLIB_PATH}
endif

${OBJ_REL_PATH}:
	mkdir -p ${OBJ_REL_PATH}

${BINARY_PATH}:
	mkdir -p ${BINARY_PATH}

${DEPS_REL_PATH}:
	mkdir -p ${DEPS_REL_PATH}

${BIN_PATH}:
	mkdir -p ${BIN_PATH}

${BUILD_XCP_PATH}:
	mkdir -p ${BUILD_XCP_PATH}

########################### Cleaning-up ######################################
.PHONY: locdepclean locclean locallclean hexclean allclean clean depclean realclean
.PHONY: depcleanstem cleanstem allcleanstem hexcleanstem

# Remove depfiles for the *current* board only
locdepclean:
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "DEPCLEAN BCPU     ${LOCAL_NAME}"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "DEPCLEAN RAMINIT  ${LOCAL_NAME}"
else
	@${ECHO} "DEPCLEAN          ${LOCAL_NAME}"
endif
endif
# With the -f option, rm will return 0 even if there is no .d file
	rm -f ${DEPS_REL_PATH}/*.d

# Remove local library if any, and any objects generated from source
locclean: xcpfinalcleanstem
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "CLEAN BCPU        ${LOCAL_NAME}"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "CLEAN RAMINIT     ${LOCAL_NAME}"
else
	@${ECHO} "CLEAN             ${LOCAL_NAME}"
endif
endif
ifneq "${IS_ENTRY_POINT}" "yes"
	rm -fr ${OBJ_REL_PATH}/*
	rm -f ${LOCAL_SRCLIBRARY}
else
#	rm -f ${filter-out ${foreach obj,${LOCAL_OBJS},${OBJ_REL_PATH}/${obj}}, ${FULL_SRC_OBJECTS}}
	echo ${FULL_SRC_OBJECTS} | xargs rm -f
#	# Remove files in the shadow.
	rm -f ${HEX} ${MAP} ${BIN}
#	# Remove files in hex.
	rm -f ${HEX_FINAL} ${MAP_FINAL} ${BIN_FINAL} ${LODBASE}*.lod ${XCP_FINAL} ${XMD_FINAL} ${DIS_FINAL}
endif # IS_ENTRY_POINT

# Remove objects and depfiles for *all* targets
locallclean: xcpfinalcleanstem
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "ALLCLEAN BCPU    ${LOCAL_NAME}"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "ALLCLEAN RAMINIT ${LOCAL_NAME}"
else
	@${ECHO} "ALLCLEAN         ${LOCAL_NAME}"
endif
endif
# NOTE:
# make will open the file handles for the directories given in vpath.
# In case of a directory removal (allclean), the "vpath %.o ${OBJ_REL_PATH}"
# statement will certainly block the removal operation for the object path:
# the "rm -fr xxx" command results in "Directory not empty".
ifeq "${CLEAN_ALL_RELEASE}" "1"
	echo ${OBJ_PATH} ${DEPS_PATH} ${LIB_PATH} | xargs rm -fr
else
	echo ${OBJ_REL_PATH} ${DEPS_REL_PATH} ${LOCAL_SRCLIBRARY} | xargs rm -fr
endif # CLEAN_ALL_RELEASE

# Remove all .srec, .dis, .map and elf files with the directory prefix, 
# regardless of the suffix (LOCAL_BATCH_NAME)
# In fact, we remove all files with the matching prefix, except those that could be source
# Use with care !
hexcleanstem:
	rm -f ${wildcard ${BIN_PATH}/${notdir ${LOCAL_NAME}}*}

ifeq "" "${notdir ${LOCAL_NAME}}"
hexclean:
	$(error "Bad LOCAL_NAME definition, should not end with '\' Not doing the clean to avoid loosing files.")
else
hexclean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} hexcleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} hexcleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} hexcleanstem
endif
endif # LOCAL_NAME

# Same targets, but recursive
allcleanstem: locallclean
# The '+' token works like ${MAKE} directly in the line content.
	+@${FULL_DEPENDENCY_ALLCLEAN}

cleanstem: locclean
# The '+' token works like ${MAKE} directly in the line content.
	+@${FULL_DEPENDENCY_CLEAN}

depcleanstem: locdepclean
# The '+' token works like ${MAKE} directly in the line content.
	+@${FULL_DEPENDENCY_DEPCLEAN}

allclean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} allcleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} allcleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} allcleanstem
endif

clean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} cleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} cleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} cleanstem
endif

depclean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} depcleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} depcleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} depcleanstem
endif

realclean: clean depclean xcprealclean

##################################################################################



########################## Generate the documentation #############################

DOC_MODULE_NAME ?= ${MODULE_NAME}

# Specific Doxygen Configuration Items
DOC_PROJECT_NAME := `perl -e 'print "\\U${DOC_MODULE_NAME}"'`
#DOC_PROJECT_NAME := `ruby -e "puts \\"${DOC_MODULE_NAME}\\".upcase"`

# Doc is generated in this directory
DOC_OUTPUT_DIR ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/doc/
DOCFULL_OUTPUT_DIR ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/docfull/
DOCMODEL_OUTPUT_DIR ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/docmodel/

# The tag containing the doc info will be
DOC_GENERATED_TAG ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/doc/${DOC_MODULE_NAME}.tag
DOCFULL_GENERATED_TAG ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/docfull/${DOC_MODULE_NAME}.tag
DOCMODEL_GENERATED_TAG ?= ${SOFT_WORKDIR}/${LOCAL_NAME}/docmodel/${DOC_MODULE_NAME}.tag

# Input for doxygen (only headers, sources, module dependencies, etc)
ifeq "${DOCUMENT_DEPENDENCIES}" "1"
#	# Input files come from $LOCAL_API_DEPENDS and $EXTRA_DOC_INPUTS.
    DOC_INPUT_MODULES := $(sort ${LOCAL_API_DEPENDS} ${EXTRA_DOC_INPUTS})

#	# Headers of the dependencies are copied in a tmp dir to edit the paging
#	# The local headers still need to be added manually.
    SOFTDOC_INPUT := ${LOCAL_PATH}/include \
					${foreach MODULE_NAME, ${DOC_INPUT_MODULES}, ${SOFT_WORKDIR}/$(MODULE_NAME)/${INC_DIR}}
	SOFTDOCFULL_INPUT := $(SOFTDOC_INPUT) ${foreach MODULE_NAME, ${DOC_INPUT_MODULES},\
                    ${SOFT_WORKDIR}/$(MODULE_NAME)}  
else
#	# Document only the current module and any extra the user like.
    SOFTDOC_INPUT := ${LOCAL_PATH}/include \
					${foreach MODULE_NAME, ${EXTRA_DOC_INPUTS}, ${SOFT_WORKDIR}/$(MODULE_NAME)/${INC_DIR}}	
    SOFTDOCFULL_INPUT := ${LOCAL_PATH} \
					${foreach MODULE_NAME, ${EXTRA_DOC_INPUTS},${SOFT_WORKDIR}/$(MODULE_NAME)}	
endif


.PHONY: doc docfull doc_release docfull_release docmodel

doc:
	@${ECHO} "DOXYGEN           $(LOCAL_NAME)"
	mkdir -p ${SOFT_WORKDIR}/doc/files
#	# Create specific configuration file holding specific items: current.doxy
#	# Ok, that could be better...
#	# NOTA: Use XMD_ALL_EXPORT_FLAG as we need to escape the " characters herre too.
	echo -e "\n\
             \t PROJECT_NAME = ${DOC_PROJECT_NAME} \n\
             \t INPUT = ${SOFTDOC_INPUT} \n\
             \t HTML_OUTPUT = files \n\
             \t OUTPUT_DIRECTORY = ${DOC_OUTPUT_DIR} \n\
             \t TAGFILES = ${DOC_TAG_LIST} \n\
             \t PREDEFINED = ${XMD_ALL_EXPORT_FLAG} \n\
             \t GENERATE_TAGFILE = ${DOC_GENERATED_TAG} \n" > ${SOFT_WORKDIR}/env/docgen/current.doxy
#	# Create doc.
	doxygen ${SOFT_WORKDIR}/env/docgen/DoxyApi ${STDOUT_NULL}
#	# Copy missing items (images...) and create accessor.
	cp $(SOFT_WORKDIR)/env/docgen/style/coolsand.png ./doc/files/
	cp $(SOFT_WORKDIR)/env/docgen/style/template.html ./doc/${DOC_MODULE_NAME}.html
	sed -i"" -e 's/XXX_MODULE_NAME_XXX/files/g' ./doc/${DOC_MODULE_NAME}.html


docfull:
	@${ECHO} "DOXYGEN           $(LOCAL_NAME)"
	mkdir -p ${SOFT_WORKDIR}/docfull/files
#	# Create specific configuration file holding specific items: current.doxy
#	# Ok, that could be better...
	echo -e "\n\
             \t PROJECT_NAME = ${DOC_PROJECT_NAME} \n\
             \t INPUT = ${SOFTDOCFULL_INPUT} \n\
             \t HTML_OUTPUT = files \n\
             \t OUTPUT_DIRECTORY = ${DOCFULL_OUTPUT_DIR} \n\
             \t TAGFILES = ${DOCFULL_TAG_LIST} \n\
             \t PREDEFINED = ${XMD_ALL_EXPORT_FLAG} \n\
             \t GENERATE_TAGFILE = ${DOCFULL_GENERATED_TAG} \n" > ${SOFT_WORKDIR}/env/docgen/current.doxy
#	# Create doc.
	doxygen ${SOFT_WORKDIR}/env/docgen/DoxyFull ${STDOUT_NULL}
#	# Copy missing items (images...) and create accessor.
	cp $(SOFT_WORKDIR)/env/docgen/style/coolsand.png ./docfull/files/
	cp $(SOFT_WORKDIR)/env/docgen/style/template.html ./docfull/${DOC_MODULE_NAME}.html
	sed -i"" -e 's/XXX_MODULE_NAME_XXX/files/g' docfull/${DOC_MODULE_NAME}.html


DOC_RELEASE_DIR ?= /n/DOC/Public/Modem2G/APIs
DOC_FULL_RELEASE_DIR ?= /n/projects/Granite/DOC/APIs_full


## ------------------------------------------------------------------------- ##
##          Documentation release rules
## ------------------------------------------------------------------------- ##
doc_release:
	$(MAKE) doc
	@${ECHO} "DOXYGEN           Copy documentation"
	rm -rf ${DOC_RELEASE_DIR}/${DOC_MODULE_NAME}*
	cp -r   ${SOFT_WORKDIR}/${LOCAL_NAME}/doc/files ${DOC_RELEASE_DIR}/${DOC_MODULE_NAME}
#	# Create accessor
	cp $(SOFT_WORKDIR)/env/docgen/style/template.html ${DOC_RELEASE_DIR}/${DOC_MODULE_NAME}.html
	sed -i"" -e 's/XXX_MODULE_NAME_XXX/${DOC_MODULE_NAME}/g' ${DOC_RELEASE_DIR}/${DOC_MODULE_NAME}.html
	${ECHO} Done!


docfull_release:
	$(MAKE) docfull
	@${ECHO} "DOXYGEN           Copy full documentation"
	rm -rf ${DOC_FULL_RELEASE_DIR}/${DOC_MODULE_NAME}*
	cp -r   ${SOFT_WORKDIR}/${LOCAL_NAME}/docfull/files ${DOC_FULL_RELEASE_DIR}/${DOC_MODULE_NAME}
#	# Create accessor
	cp $(SOFT_WORKDIR)/env/docgen/style/template.html ${DOC_FULL_RELEASE_DIR}/${DOC_MODULE_NAME}.html
	sed -i"" -e 's/XXX_MODULE_NAME_XXX/${DOC_MODULE_NAME}/g' ${DOC_FULL_RELEASE_DIR}/${DOC_MODULE_NAME}.html
	${ECHO} Done!


ifeq ($ISMODELS,YES)
docmodel:
	@${ECHO} "DOXYGEN           $(LOCAL_NAME)"
#	# Create specific configuration file holding specific items: current.doxy
#	# Ok, it couldn't be more beautifull, mon buddy ...
	echo -e "\n\
	    \t PROJECT_NAME = ${DOC_PROJECT_NAME} \n\
	    \t HTML_OUTPUT = files \n\
	    \t INPUT = ${DOCMODEL_INPUT} \n\
	    \t OUTPUT_DIRECTORY = ${DOCMODEL_OUTPUT_DIR} \n\
	    \t TAGFILES = ${DOCMODEL_TAG_LIST} \n\
	    \t GENERATE_TAGFILE = ${DOCMODEL_GENERATED_TAG} \n" > ${SOFT_WORKDIR}/env/docgen/current.doxy
	mkdir -p ${SOFT_WORKDIR}/${LOCAL_NAME}/docmodel/files
#	# Create Doc
	doxygen ${SOFT_WORKDIR}/env/docgen/DoxyModel ${STDOUT_NULL}
#	# Copy missing items (images...) and create accessor    
	cp $(SOFT_WORKDIR)/env/docgen/style/coolsand.png ./docmodel/files/
	cp $(SOFT_WORKDIR)/env/docgen/style/template.html ./docmodel/${DOC_MODULE_NAME}.html
	sed -i"" -e 's/XXX_MODULE_NAME_XXX/files/g' ./docmodel/${DOC_MODULE_NAME}.html
else
docmodel:
	echo "There are no models for ${DOC_MODULE_NAME} module."
endif

##################################################################################



##################### Generate the overlay linkers ###############################

# Search path for preprocessor to include genrated parts
LDPPFLAGS += -I${BUILD_ROOT}

ifeq "${OVERLAYLDGEN}" "yes"

OVERLAY_NUMBER_PER_LOCATION := 6

# notes about code bellow
# $(call FUNC, params...) evaluate FUNC which must be defined as =, replace $(1) ... with params
# $(value NAME) -> $(NAME)
# $(eval block) return empty string and to eval of block

# function OVERLAY_SECTION_ITEM parameters
# $(1) = content -> toto.o titi.o
# $(2) = section catch -> .text .text.* .sramtext
OVERLAY_SECTION_ITEM = $(foreach object, $1, "            *$(object)($(2))\n")

# function FULL_OVERLAY_SECTION_OBJECT_FUNC parameters
# $(1) = content
# $(2) = numberOfOverlays
FULL_OVERLAY_SECTION_OBJECT_FUNC = $(foreach i,$(shell SimpleSeq.sh 0 `expr $(2) - 1`), $(value $(1)$(i)))

OVERLAY_TEXT_SECTIONS := .text .text.* .sramtext
OVERLAY_DATA_SECTIONS := .rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*
OVERLAY_BSS_SECTIONS := .bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*

# function OVERLAY_INSTANCE_GEN_FUNCTION parameters
# $(1) = baseName -> sys_sram_overlay
# $(2) = Base of the Makefile variable hodling the symbols to include for 'i' -> INT_SRAM_OVERLAY_CONTENT_
# $(3) = numberOfOverlays -> 2
OVERLAY_INSTANCE_GEN_FUNCTION = $(eval OVERLAY_BUILDING_SIZE := 0)\
    $(eval OVERLAY_MAX_SIZE := $(4)) \
    $(foreach i,$(shell SimpleSeq.sh 0 `expr $(3) - 1`), \
    $(eval OVERLAY_BUILDING_SIZE += + SIZEOF(.$(1)_$(i)))\
    $(eval OVERLAY_MAX_SIZE = MAX(SIZEOF(.$(1)_$(i)), $(OVERLAY_MAX_SIZE)))\
    "        .$(1)_$(i)\n"\
    "        {\n"\
                $(if $(value $(2)$(i)_INPUT_SECTIONS),\
                    $(call OVERLAY_SECTION_ITEM,$(value $(2)$(i)),$(value $(2)$(i)_INPUT_SECTIONS)),\
                    $(call OVERLAY_SECTION_ITEM,$(value $(2)$(i)),$(OVERLAY_TEXT_SECTIONS))\
                        $(call OVERLAY_SECTION_ITEM,$(value $(2)$(i)),$(OVERLAY_DATA_SECTIONS))\
                        $(call OVERLAY_SECTION_ITEM,$(value $(2)$(i)),$(OVERLAY_BSS_SECTIONS))\
                )\
    "            \#if (USE_BINUTILS_2_19 == 1) \n" \
    "            /* FIXME: LD 2.19 doesn't like empty overlays... */\n" \
    "            LONG(0)\n" \
    "            \#endif \n" \
    "            . = ALIGN(0x10);\n"\
    "        }\n\n")\
    "        \#define FLASH_SIZE_OF_$(shell perl -e 'print "\U$(1)"') $(OVERLAY_BUILDING_SIZE)\n"\
    "        \#define MAX_SIZE_OF_$(shell perl -e 'print "\U$(1)"') $(OVERLAY_MAX_SIZE)\n"


.PHONY: overlayLinkerIntSram overlayLinkerFastCpuMemory overlaylinkers

# build INT SRAM overlay linker file
overlayLinkerIntSram:
ifeq ($(filter $(INT_SRAM_OVERLAY_BSS_RESERVED_OBJECT_LIST),$(call FULL_OVERLAY_SECTION_OBJECT_FUNC,INT_SRAM_OVERLAY_CONTENT_,$(OVERLAY_NUMBER_PER_LOCATION))),)	
	echo -e $(call OVERLAY_INSTANCE_GEN_FUNCTION,sys_sram_overlay,INT_SRAM_OVERLAY_CONTENT_,$(OVERLAY_NUMBER_PER_LOCATION),0) > ${BUILD_ROOT}/int_sram_overlay.ld
else
	echo -e $(call OVERLAY_INSTANCE_GEN_FUNCTION,sys_sram_overlay,INT_SRAM_OVERLAY_CONTENT_,$(OVERLAY_NUMBER_PER_LOCATION),$(INT_SRAM_OVERLAY_BSS_RESERVED_SIZE)) > ${BUILD_ROOT}/int_sram_overlay.ld
endif

# build Fast Cpu Memory overlay linker file
overlayLinkerFastCpuMemory:
	echo -e $(call OVERLAY_INSTANCE_GEN_FUNCTION,fast_cpu_memory_overlay,FAST_CPU_MEMORY_OVERLAY_CONTENT_,$(OVERLAY_NUMBER_PER_LOCATION),0) > ${BUILD_ROOT}/fast_cpu_memory_overlay.ld


# all overlays
overlaylinkers: overlayLinkerIntSram overlayLinkerFastCpuMemory

endif # OVERLAYLDGEN

#################################################################################



##################################################################################
## Clean the disassembly rule. (Part of the lod creation process, as a new lod
## implies that any previous disassembly becomes false and irrelevant.
##################################################################################
.PHONY: clean_disassembly
clean_disassembly:
	rm -f ${DIS_FINAL}

#################################################################################



########################## Generate the CoolProfile headers ######################

.PHONY: xcprealclean xcpfinalcleanstem xcpfinalclean xcpclean
.PHONY: xcppack

# Generate automatically the CoolProfile header from the XCP file for a given
# software module. This is define in the module's Makefile.
# Default value is no.
ifeq "${AUTO_XCP2H}" "yes"

XCP_INPUT_NAME     := *_profile_codes.xcp
XCP_INPUT_DIR      := ${LOCAL_INC_DIR}
XCP_INPUT_FILE     := $(wildcard ${XCP_INPUT_DIR}/${XCP_INPUT_NAME})
XCP_INPUT_ABS_PATH := $(call NOCYGPATH,${SOFT_WORKDIR}/${LOCAL_NAME}/${XCP_INPUT_FILE})

# Create the output file by changing the extension of the input file.
XCP_OUTPUT_DIR     := ${LOCAL_INC_DIR}
XCP_OUTPUT_FILE    := $(patsubst %.xcp,%.h,${XCP_INPUT_FILE})
XCP_OUTPUT_ABS_DIR := $(call NOCYGPATH,${SOFT_WORKDIR}/${LOCAL_NAME}/${XCP_OUTPUT_DIR})

XCP_BUILD_NAME     := $(notdir ${XCP_INPUT_FILE})
XCP_BUILD_FILE     := ${BUILD_XCP_PATH}/${XCP_BUILD_NAME}

XCP_DEPENDS_INPUTS := $(wildcard ${BUILD_XCP_PATH}/*.xcp)

XCP_PROCESS := @${ECHO} "XCP2H             $(notdir $(XCP_INPUT_FILE))"; \
		xcp2h -i ${XCP_INPUT_ABS_PATH} \
		-o ${XCP_OUTPUT_ABS_DIR} \
        -u ${STDOUT_NULL}

.PHONY: xcprealcleanstem xcp2h

# Generate the CoolProfile header from the XCP file, when asked.
xcp2h: ${XCP_OUTPUT_FILE}

ifneq (${XCP_INPUT_FILE},)
${XCP_OUTPUT_FILE}: ${XCP_INPUT_FILE}
#	@${ECHO} "XCP OUTPUT    $(XCP_OUTPUT_FILE)"
	$(XCP_PROCESS)
endif

xcprealcleanstem:
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "XCPREALCLEAN BCPU"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "XCPREALCLEAN RAMINIT"
else
	@${ECHO} "XCPREALCLEAN"
endif
endif
	rm -rf ${BUILD_XCP_PATH}

xcprealclean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} xcprealcleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} xcprealcleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} xcprealcleanstem
endif

XCP2H := xcp2h

ifneq (${XCP_INPUT_FILE},)
# Also generate XCP_BUILD_FILE during XCP process
xcp2h: ${XCP_BUILD_FILE}
${XCP_BUILD_FILE}: ${XCP_INPUT_FILE} | ${BUILD_XCP_PATH}
#	@${ECHO} "XCP BUILD     $(XCP_BUILD_FILE)"
	cp ${XCP_INPUT_FILE} $@
endif

xcpfinalcleanstem:
ifneq (${XCP_INPUT_FILE},)
#	@${ECHO} "XCPFINALCLEAN     $(XCP_BUILD_FILE)"
	rm -f ${XCP_BUILD_FILE}
endif

xcpfinalclean:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} xcpfinalcleanstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} xcpfinalcleanstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} xcpfinalcleanstem
endif

# CoolProfile header files from XCP are also committed in version control database,
# and there is no need to remove them when cleaning objects.
xcpclean:
#ifneq (${XCP_INPUT_FILE},)
##	@${ECHO} "XCPCLEAN     $(XCP_OUTPUT_FILE)"
#	rm -f ${XCP_OUTPUT_FILE}
#endif

# Generate the CoolProfile packed file from the XCP file.
${XCP_FINAL}: $(XCP_DEPENDS_INPUTS) ${BINARY_PATH} force
ifneq ($(strip $(XCP_DEPENDS_INPUTS)),)
	@${ECHO}
	@${ECHO} "XCPPACK           $(notdir $(XCP_FINAL))"
	rm -f $@
	echo "<cpconfig>" >> $@
	cat ${XCP_DEPENDS_INPUTS} >> $@
	echo "</cpconfig>" >> $@
endif # XCP_DEPENDS_INPUTS

# Special target to generate XCP package only.
xcppack: ${XCP_FINAL}

else  # !AUTO_XCP2H

XCP2H :=

xcppack ${XCP_FINAL}:
xcprealclean xcpfinalcleanstem xcpfinalclean xcpclean:

endif # !AUTO_XCP2H

##################################################################################



########################## Create a .h file with the commit in versin control system

# Default value is no.
ifeq "${AUTO_GEN_REVISION_HEADER}" "yes"

MOD_LOWCASE := ${MODULE_NAME}
MOD_CMD := perl -e 'print "\U${MOD_LOWCASE}"'
ifeq "${WITH_GIT}" "1"
LOCAL_REVISION_CMD := echo 0x$$(git rev-parse --short=8 HEAD) || echo 0xffffffff
LOCAL_BRANCH_CMD := basename $$(git name-rev --name-only HEAD | head -1) || echo "none"
else # WITH_GIT != 1
ifeq "${WITH_SVN}" "0"
LOCAL_REVISION_CMD := echo "-1"
LOCAL_BRANCH_CMD := echo "none"
else
LOCAL_REVISION_CMD := svn info ${LOCAL_PATH} | grep "Last Changed Rev" | sed 's/.*: //'
LOCAL_BRANCH_CMD := svn info ${LOCAL_PATH} | grep "^URL" | cut -d '/' -f 6-
endif
endif # WITH_GIT != 1

PROJECT_BRANCH_CMD := svn info ${SOFT_WORKDIR} | grep "^URL" | cut -d '/' -f 6-
UPDATE_VERSION_HEADER := updateversion

ifeq "${IS_ENTRY_POINT}" "yes"
REV_FILE := ${MOD_LOWCASE}p_version.h
else
REV_FILE := ${SRC_PATH}/${MOD_LOWCASE}p_version.h
endif

REV_SKEL_FILE := ${SOFT_WORKDIR}/env/compilation/mod_version_skel.h
REGISTERED_REV_CMD := sed -n 's/.*VERSION_REVISION *(\(-\?[[:alnum:]]*\)).*/\1/p' ${REV_FILE}

.PHONY: updateversion
updateversion:
# REGISTERED_REV is defined only if the file already exists.
# If it doesn't exist, or the value is different, the file is
# created, which will lead to the update of the revision file.
	if [ "x${WITH_VERGEN}" = "x1" ]; then							\
	    if [ "x${IS_ENTRY_POINT}" != "xyes" -a ! -d ${SRC_PATH} ]; then				\
            mkdir ${SRC_PATH};														\
        fi;																			\
	    if [ -f ${REV_FILE} ]; then													\
	    	REGISTERED_REV=`$(REGISTERED_REV_CMD)`;									\
	    fi;																			\
	    LOCAL_REVISION=`$(LOCAL_REVISION_CMD)`;										\
	    LOCAL_BRANCH=`$(LOCAL_BRANCH_CMD) | sed 's/\\//\\\\\\//g'`;					\
	    if [ "x${WITH_SVN}" = "x1" ]; then											\
			PROJECT_BRANCH=`$(PROJECT_BRANCH_CMD) | sed 's/\\//\\\\\\//g'`;			\
			LOCAL_BRANCH=$$LOCAL_BRANCH" PROJ:"$$PROJECT_BRANCH;					\
	    fi;																			\
	    MOD=`$(MOD_CMD)`;															\
	    if [ "$$REGISTERED_REV" != "$$LOCAL_REVISION" ]; then						\
	       	${ECHO} "REVGEN            `basename ${REV_FILE}` @ r$$LOCAL_REVISION";	\
	    	sed -e "s/@{MOD}/$$MOD/g"												\
	    	    -e "s/@{LOCAL_REVISION}/$$LOCAL_REVISION/g"							\
	    	    -e "s/@{LOCAL_BRANCH}/\"$$LOCAL_BRANCH\"/g" ${REV_SKEL_FILE} > ${REV_FILE};	\
	    fi;																			\
	fi;

else # !AUTO_GEN_REVISION_HEADER

UPDATE_VERSION_HEADER :=

endif # !AUTO_GEN_REVISION_HEADER

##################################################################################



############### Generate the CoolWatcher memory description headers ##############

.PHONY: xmdpack xmdinstall

# Generate the CoolWatcher memory description headers from the XMD files,
# only if it is needed by the modification dates.
# Do this once for all the XMDs of all the modules.
ifeq "${AUTO_XMD2H}" "yes"

# Only keeps the flags containing an "=" character.
XMD_ALL_EXPORT_FLAG := ${subst ",\", ${ALL_EXPORT_FLAG}}
# "} # just to fix vim's coloring that beleave we are in a string there...
XMD_PROCESS := xmd2h.sh "${VERBOSE}" "${XMD_ALL_EXPORT_FLAG}"
XMD_MAP_DIR := toolpool/map/${CT_ASIC_CAPD}
XMD_ROOT_ABS := $(call NOCYGPATH,${SOFT_WORKDIR}/${XMD_MAP_DIR}/${CT_ASIC}_soft_root.xmd)
XMD_ROOT_WITH_VOC_ABS := $(call NOCYGPATH,${SOFT_WORKDIR}/${XMD_MAP_DIR}/${CT_ASIC}_soft_root_with_voc.xmd)
XMD_PKG_ABS := $(call NOCYGPATH,${SOFT_WORKDIR}/${XMD_MAP_DIR}/${CT_ASIC}_soft_pkg.xmd)
XMD_PKG_WITH_VOC_ABS := $(call NOCYGPATH,${SOFT_WORKDIR}/${XMD_MAP_DIR}/${CT_ASIC}_soft_pkg_with_voc.xmd)
XMD_FINAL_ABS := $(call NOCYGPATH,${BAS_FINAL}.xmd)

.PHONY: xmd2h
xmd2h: ${UPDATE_VERSION}
	@${ECHO}
	@${ECHO} "XMD2H"
# If not VERBOSE, the redeclaration of variables warnings will be filtered out.
	$(XMD_PROCESS)

# Generate the CoolXml packed file from the XMD file.
${XMD_FINAL}: ${BINARY_PATH} force
	@${ECHO}
	@${ECHO} "XMDPACK           `basename $(XMD_FINAL)`"
	rm -f $@
ifeq ($(VERBOSE),1)
#	# Pass the SPC_IF_VERsion number to coolpkg to generate a coherent .xmd for
#	# norom targets using the standard (latest) version for BCPU code instead
#	# of the romed one.
	coolpkg -i ${XMD_ROOT_ABS} -o ${XMD_FINAL_ABS} ${XMD_FLAGS} -D SPC_IF_VER=${SPC_IF_VER} \
		--keepcomments ${STDOUT_NULL}
else
#	# Filter out the redeclaration of variables warnings.
	coolpkg -i ${XMD_ROOT_ABS} -o ${XMD_FINAL_ABS} ${XMD_FLAGS} -D SPC_IF_VER=${SPC_IF_VER} \
		--keepcomments ${STDOUT_NULL} ${STDERR_NULL}
endif # VERBOSE

# Special target to generate XMD package only.
xmdpack: ${XMD_FINAL}

# Special target to just install XMD package and create one copy with VOC.
xmdinstall: ${XMD_FINAL}
	@${ECHO}
	@${ECHO} "INSTALL           ${XMD_MAP_DIR}/`basename $(XMD_PKG_ABS)`"
	rm -f ${XMD_PKG_ABS}
	cp ${XMD_FINAL_ABS} ${XMD_PKG_ABS}
ifeq ($(VERBOSE),1)
	coolpkg -i ${XMD_ROOT_WITH_VOC_ABS} -o ${XMD_PKG_WITH_VOC_ABS} ${XMD_FLAGS} \
		--keepcomments ${STDOUT_NULL} 
else
#	# Filter out the redeclaration of variables warnings.
	coolpkg -i ${XMD_ROOT_WITH_VOC_ABS} -o ${XMD_PKG_WITH_VOC_ABS} ${XMD_FLAGS} \
		--keepcomments ${STDOUT_NULL}  ${STDERR_NULL}
endif # VERBOSE

else # !AUTO_XMD2H

${XMD_FINAL} xmdpack xmdinstall:

endif # !AUTO_XMD2H

##################################################################################



##################################################################################
## Compilation rules
##################################################################################

ifeq "${WITH_FULL_PATH}" "1"
REALPATH=$(abspath $<)
else
REALPATH=$<
endif # WITH_FULL_PATH := 1

############################ Rules to generate compressed objects ########################

ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"

${BIN_PATH}/get_compressed_data.o: \
${COMPRESSED_STUB_SRC_DIR}/get_compressed_data.c \
${COMPRESSED_STUB_SRC_DIR}/../include/cutdownflashusage.h \
 | ${BIN_PATH}
	@${ECHO} "CC                $(<F)"
	$(CC) $(C_SPECIFIC_CFLAGS) $(CFLAGS) $(CT_MIPS16_CFLAGS) $(MYCFLAGS) $(CPPFLAGS) -I${COMPRESSED_STUB_SRC_DIR}/../include  -o $@ $<

${BIN_PATH}/%.bin.lzma.o ${BIN_PATH}/%.sum.o: ${BIN_STAGE1} | ${BIN_PATH}
	@${ECHO} "Compress          $*"
	${OBJCOPY} -j .$* -O binary $< ${BIN_PATH}/$*.bin
	cp -f ${BIN_PATH}/$*.bin ${BIN_PATH}/$*.bin.org
	${LZMA} -f -v -9 $(call NOCYGPATH,${BIN_PATH}/$*.bin)
	cd ${BIN_PATH} && ${OBJCOPY} -I binary -O elf32-littlemips -B mips $*.bin.lzma ${BIN_PATH}/$*.bin.lzma.o
	@${ECHO}
	@${ECHO} "cksum             $*"
	${CKSUM}  ${BIN_PATH}/$*.bin.org |cut -d " " -f 1  | perl -e 'print pack "J*", <>' > ${BIN_PATH}/$*.sum
	cd ${BIN_PATH} && ${OBJCOPY} -I binary -O elf32-littlemips -B mips $*.sum ${BIN_PATH}/$*.sum.o
	@${ECHO}

endif # COMPRESS_PROCESS_ENABLE


############################ Rule to generate user data #############################

ifeq "${USERGEN}" "yes"

${BIN_PATH}/user.bin.o: ${BIN_PATH}/usrgenerator/user.bin
	${OBJCOPY} -I binary -O elf32-littlemips -B mips --rename-section .data=.user_data,alloc,load,readonly,data,contents $< $@

endif

############################ Implicit rules ########################################

# We generate one dependency file on the fly for each source file, 
# either when preprocessing for asm files, or when compiling for C files.
# Those depfiles are also board depend (to deal with conditional includes).
# Empty rules are generated for all header files, to avoid issues in case
# one header is deleted (-MP)

${OBJ_REL_PATH}/%.bin.o: ${LOCAL_SRC_DIR}/%.bin
#	${OBJCOPY} -I binary -O elf32-littlemips -B mips --rename-section .data=.$*,readonly,contents $^ $@
#	@${ECHO}
	obcname=_binary_$(subst /,_,$(subst .,_,$<));\
	${OBJCOPY} -I binary -O elf32-littlemips -B mips:xcpu \
	--rename-section .data=.rodata \
	--redefine-sym $${obcname}_start=$*_start \
	--redefine-sym $${obcname}_end=$*_end \
	--redefine-sym $${obcname}_size=$*_size \
	$< $@
	@${ECHO}

${OBJ_REL_PATH}/%.o: ${LOCAL_SRC_DIR}/%.S
	@${ECHO} "CPP               $*.S"
	$(CPP) $(CPPFLAGS) $(ASCPPFLAGS)  -MT ${OBJ_REL_PATH}/$*.o -MD -MP -MF ${DEPS_REL_PATH}/$*.d -o ${OBJ_REL_PATH}/$*.asm $(REALPATH)
	@${ECHO} "AS                $*.asm"
	$(AS) $(ASFLAGS) -o ${OBJ_REL_PATH}/$*.o ${OBJ_REL_PATH}/$*.asm

${OBJ_REL_PATH}/%.o: ${LOCAL_SRC_DIR}/%.c
	@${ECHO} "CC                $*.c"
	@echo $(C_SPECIFIC_CFLAGS) $(CFLAGS) $(CT_MIPS16_CFLAGS) $(MYCFLAGS) $(CPPFLAGS) > ${OBJ_REL_PATH}/$*.opt
	$(CC) -MT ${OBJ_REL_PATH}/$*.o -MD -MP -MF ${DEPS_REL_PATH}/$*.d @${OBJ_REL_PATH}/$*.opt -o ${OBJ_REL_PATH}/$*.o $(REALPATH)

${OBJ_REL_PATH}/%.o: ${LOCAL_SRC_DIR}/%.cpp
	@${ECHO} "C++               $*.cpp"
	@echo $(C++_SPECIFIC_CFLAGS) $(CFLAGS) $(CT_MIPS16_CFLAGS) $(MYCFLAGS) $(CPPFLAGS) > ${OBJ_REL_PATH}/$*.opt
	$(CXX) -MT ${OBJ_REL_PATH}/$*.o -MD -MP -MF ${DEPS_REL_PATH}/$*.d @${OBJ_REL_PATH}/$*.opt -o ${OBJ_REL_PATH}/$*.o $(REALPATH) $(EXTERN_CPPFLAGS)

# Add the local rule. As the rule is set by 'define' directive, there is only
# one local rule available in each make run.
$(eval ${LOCAL_RULE})

# NOTE:
# Do not add general implicit rules for source files.
# Otherwise it will impact the use of VPATH directory search result,
# for the source files in prerequisites will become targets,
# and this will cause compilation errors when VPATH and -B option 
# are both used (the path found via directory search is abandoned).


######################### library stuff #########################################
.PHONY: makedirs headergen dependencies

makedirs: ${MAKE_DIRS}
headergen: ${XCP2H} ${UPDATE_VERSION_HEADER}

# The submodules depend on:
# 	The automatically generated header files for this module
dependencies: force headergen
# The '+' token works like ${MAKE} directly in the line content.
	+@${FULL_DEPENDENCY_COMPILE}

extragen:
	${foreach file,${C_SRC},mkdir -p ${OBJ_REL_PATH}/$(dir ${file});}
	${foreach file,${C_SRC},mkdir -p ${DEPS_REL_PATH}/$(dir ${file});}

ifneq "${strip ${FULL_SRC_OBJECTS}}" ""
# Prerequisites are order-only as we do not want to re-build the
# source objects in every make run (depending on phony targets)
ifeq "${USE_DIFFERENT_SOURCE_LAYOUT}" "yes"
${FULL_SRC_OBJECTS}: | makedirs headergen extragen
else
${FULL_SRC_OBJECTS}: | makedirs headergen
endif
# The local source objects that must be built
ifneq "${strip ${SRC_OBJECTS_FORCE_BUILD}}" ""
${SRC_OBJECTS_FORCE_BUILD}: force
endif # SRC_OBJECTS_FORCE_BUILD
endif # FULL_SRC_OBJECTS

# Whether to extract object files from sub libraries and archive into local library
COMBINE_LIB ?= yes

LOCAL_LIB_INCR_LINK ?= no

# Explode sub-modules libraries
LOCAL_SUBMODULE_LIBRARY_EXPLODE_CMD = cd ${OBJ_REL_PATH} && $(AR) x ${libfile} && ${ECHO} "        (added ${notdir $(libfile)} objects)"
LOCAL_SUBMODULE_LIBRARY_EXPLODE := ${foreach libfile, $(FULL_LIBRARY_FILES) , $(LOCAL_SUBMODULE_LIBRARY_EXPLODE_CMD) &&}

# The local library is different in a module that depends on submodules, 
# since we need to depend on the submodules, and add them to the archive...
# No lib is generated for ENTRY_POINT dirs
ifneq "${IS_ENTRY_POINT}" "yes"
ifeq "${IS_TOP_LEVEL}" "yes"

# We are building a module with submodules
# This module depends on:
# 	The directories which save the built objects or files
# 	The submodules that need to be compiled (listed in LOCAL_MODULE_DEPENDS, target "dependencies")
# 	The local sources that go in the library
# 	The binary library files
# 	The local library files
# We need to explode the binary sub library into objects
# We need to copy the all objects from the submodules in the obj directory of this module
$(LOCAL_SRCLIBRARY): dependencies ${BINARY_LIBRARY_FILES} ${LOCAL_ADD_LIBRARY_FILES} | makedirs
ifneq "$(FULL_SRC_OBJECTS)" ""
# headergen might change some header files. If the local source objects are listed
# as prerequisites, make has checked the timestamp of the header files before they
# are modified, and the objects will not be rebuilt in this make.
	${MAKE} $(FULL_SRC_OBJECTS)
endif
	@${ECHO} "PREPARING         ${notdir ${LOCAL_SRCLIBRARY}}"
ifneq "${COMBINE_LIB}" "yes"
	echo "/* ${LOCAL_SRCLIBRARY} */" > ${LOCAL_SRCLIBRARY}
	for libfile in $(FULL_LIBRARY_FILES); do \
		if head -1 $$libfile | grep '!<arch>' &> /dev/null; then \
			echo "INPUT($$libfile)" >> ${LOCAL_SRCLIBRARY}; \
		else \
			cat $$libfile >> ${LOCAL_SRCLIBRARY}; \
		fi; \
	done
ifneq "$(FULL_SRC_OBJECTS)" ""
	$(AR) cru  ${LOCAL_SRCLIBRARY}l $(FULL_SRC_OBJECTS)
	echo "INPUT(${LOCAL_SRCLIBRARY}l)" >> ${LOCAL_SRCLIBRARY}
endif
else
	${LOCAL_SUBMODULE_LIBRARY_EXPLODE} ${ECHO} "        (All submodules objects added)"
	@${ECHO} "AR                ${notdir ${LOCAL_SRCLIBRARY}}"
	if find ${OBJ_REL_PATH} -name "*.o" | sort >${LOCAL_SRCLIBRARY}.l 2>/dev/null; \
		then $(AR) cru 	${LOCAL_SRCLIBRARY} @${LOCAL_SRCLIBRARY}.l; \
		else $(AR) cq 	${LOCAL_SRCLIBRARY}; \
	fi;
endif # COMBINE_LIB
ifneq "$(DEPS_NOT_IN_SUBDIR)" ""
	@${ECHO} "--- DEPS_NOT_IN_SUBDIR ---"
	@${ECHO} " $(DEPS_NOT_IN_SUBDIR)"
endif # DEPS_NOT_IN_SUBDIR

else # !IS_TOP_LEVEL

# This type of module depends on:
# 	The directories which save the built objects or files
# 	The automatically generated header files
# 	The local sources that go in the library
# (The prerequisites "makedirs" and "headergen" are necessary as FULL_SRC_OBJECTS might be empty)
$(LOCAL_SRCLIBRARY): ${FULL_SRC_OBJECTS} | makedirs headergen
	@${ECHO} "AR                ${notdir ${LOCAL_SRCLIBRARY}}"
ifeq "${LOCAL_LIB_INCR_LINK}" "yes"
	$(LD) $(OFORMAT) -r -o $(OBJ_REL_PATH)/$(MODULE_NAME)lib.o $^
	$(STRIP) --strip-unneeded -x $(OBJ_REL_PATH)/$(MODULE_NAME)lib.o
	$(AR) cr $@ $(OBJ_REL_PATH)/$(MODULE_NAME)lib.o
else
	$(AR) cru ${LOCAL_SRCLIBRARY} ${FULL_SRC_OBJECTS}
endif

endif # IS_TOP_LEVEL
endif # IS_ENTRY_POINT

############################  resgen ############################ 
EMPTY :=
RMMACRO := -DMMI_ON_HARDWARE_P
RESCFLAGS := $(subst $(RMMACRO), $(EMPTY), $(foreach Aflag, $(MMI_EXPORT_FLAG), -D$(Aflag)))
RESCFLAGS += $(subst $(RMMACRO), $(EMPTY), $(foreach Aflag, $(GLOBAL_EXPORT_FLAG), -D$(Aflag)))

RESGEN_DIR := ${SOFT_WORKDIR}/$(MMI_CUSTOMER)/ResGenerator
STRCMPEX_FILE := ${RESGEN_DIR}/strcmpex.exe
STRCMPEX_RUN := ${STRCMPEX_FILE}

.PHONY : resgen
resgen : 

	@${ECHO} MMI Res used is ${MMI_CUSTOMER}
	@${ECHO}
	mkdir -p ${RESGEN_DIR}
	chmod 777 ${RESGEN_DIR}
ifneq "${MMI_CUSTOMER}" "${MMI_DEFAULT_CUSTOMER}"
	cp ${SOFT_WORKDIR}/${MMI_DEFAULT_CUSTOMER}/ResGenerator/strcmpex.exe ${STRCMPEX_FILE}
endif
	chmod 777 ${STRCMPEX_FILE}
	
#	# ${STRCMPEX_RUN} is a Windows app and accepts Windows-style path only	
	cd ${RESGEN_DIR} && ${STRCMPEX_RUN} TRUE TRUE e ~cus_opt.tmp $(foreach def,$(RESCFLAGS),$(def))
	echo "CUSTOM_OPTION = \\" > ${RESGEN_DIR}/custom_option.txt
ifneq 	($(strip $(RESCFLAGS)),  )
	cat ${RESGEN_DIR}/~cus_opt.tmp >> ${RESGEN_DIR}/custom_option.txt
	@sed -i 's/-DCOMPRESS_OPTIMIZE_FLASH_SIZE/\ /g' ${RESGEN_DIR}/~cus_opt.tmp
	@sed -i 's/-D__MMI_ANALOGTV__/\ /g' ${RESGEN_DIR}/~cus_opt.tmp
	@sed -i 's/-D__MMI_WLAN_FEATURES__/\ /g' ${RESGEN_DIR}/~cus_opt.tmp
	@sed -i 's/-D__WATCH_MONKEY_TEST__/\ /g' ${RESGEN_DIR}/~cus_opt.tmp
	@sed -i 's/-D\([^ ]*\)/\ \/D\ \"\1\"/g' ${RESGEN_DIR}/~cus_opt.tmp
	@cp ${RESGEN_DIR}/~cus_opt.tmp ${SOFT_WORKDIR}/application/CoolSimulator/TargetOption.txt
	rm ${RESGEN_DIR}/~cus_opt.tmp
endif	
ifneq "${MMI_CUSTOMER}" "${MMI_DEFAULT_CUSTOMER}"
# Caution: no make's built-in implicit rules for the resource generator either
	+${SOFT_WORKDIR}/$(MMI_CUSTOMER)/resgen.sh ${SOFT_WORKDIR}/$(MMI_CUSTOMER) ${CT_ERES}
else
	+${SOFT_WORKDIR}/$(MMI_DEFAULT_CUSTOMER)/ResGenerator/resgen.sh ${SOFT_WORKDIR}/$(MMI_CUSTOMER)
endif


.PHONY : usrgen
usrgen :
	chmod 777 ${SOFT_WORKDIR}/toolpool/usrgenerator/usrgen.sh
	+${SOFT_WORKDIR}/toolpool/usrgenerator/usrgen.sh ${CT_TARGET} ${FLSH_MODEL} ${BIN_PATH} ${CT_ERES}


GENNES_FILE := ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/gennes.exe
GENNES_RUN := ${GENNES_FILE}

.PHONY : nesgen
nesgen :
	rm -rf ${SOFT_WORKDIR}/toolpool/nesgenerator/add_nes.h
	cp ${SOFT_WORKDIR}/application/adaptation/nes/include/add_nes.h ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/add_nes.h
	chmod 777 ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/add_nes.h -R
	gcc $<  -o ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/gennes.exe ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/gennes.c  
	echo "Creating nes data, please wait..."
	${GENNES_RUN} "${SOFT_WORKDIR}/application/adaptation/nes/res/"  "${SOFT_WORKDIR}/application/adaptation/nes/src/" "${SOFT_WORKDIR}/application/adaptation/nes/include/nes_resource.h"
	rm -rf ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/add_nes.h
	rm -rf ${SOFT_WORKDIR}/application/adaptation/nes/nesgenerator/gennes.exe
	echo "nesgen done"

############################ bin files #######################################
# Generate elf binary and map file
.PHONY: targetgen

TARGET_FILE := ${BUILD_ROOT}/targetgen

ifeq "$(BOOTLOADER)" "yes"
BOOT_SECTOR_OFFSET = 0x00000000
else
BOOT_SECTOR_OFFSET = $(FLASH_BOOT_OFFSET)
endif

targetgen: force
	@${ECHO} "#############################################"     > $(TARGET_FILE)
	@${ECHO} "# Target component definitions"                   >> $(TARGET_FILE)
	@${ECHO} "#############################################"    >> $(TARGET_FILE)
	@${ECHO} "XCV_MODEL:=            $(XCV_MODEL)"              >> $(TARGET_FILE)
	@${ECHO} "PA_MODEL:=             $(PA_MODEL)"               >> $(TARGET_FILE)
	@${ECHO} "SW_MODEL:=             $(SW_MODEL)"               >> $(TARGET_FILE)
	@${ECHO} "FLSH_MODEL:=           $(FLSH_MODEL)"             >> $(TARGET_FILE)
	@${ECHO} "BOARD_CFG_FILE:=       $(BOARD_CFG_FILE)"         >> $(TARGET_FILE)
	@${ECHO} "#############################################"    >> $(TARGET_FILE)
	@${ECHO} "# Flash Mapping"                                  >> $(TARGET_FILE)
	@${ECHO} "#############################################"    >> $(TARGET_FILE)
	@${ECHO} "FLASH_SIZE:=           $(FLASH_SIZE)"             >> $(TARGET_FILE)
	@${ECHO} "RAM_SIZE:=             $(RAM_SIZE)"               >> $(TARGET_FILE)
	@${ECHO} "RAM_PHY_SIZE:=         $(RAM_PHY_SIZE)"           >> $(TARGET_FILE)
	@${ECHO} "CALIB_BASE:=           $(CALIB_BASE)"             >> $(TARGET_FILE)
	@${ECHO} "FACT_SETTINGS_BASE:=   $(FACT_SETTINGS_BASE)"     >> $(TARGET_FILE)
	@${ECHO} "CODE_BASE:=            $(CODE_BASE)"              >> $(TARGET_FILE)
	@${ECHO} "USER_DATA_BASE:=       $(USER_DATA_BASE)"         >> $(TARGET_FILE)
	@${ECHO} "USER_DATA_SIZE:=       $(USER_DATA_SIZE)"         >> $(TARGET_FILE)
	@${ECHO} "ROMU_FLASH_LAYOUT:=    $(ROMU_FLASH_LAYOUT)"      >> $(TARGET_FILE)
	@${ECHO} "BOOT_SECTOR:=          $(BOOT_SECTOR_OFFSET)"     >> $(TARGET_FILE)
	@${ECHO} ""                                                 >> $(TARGET_FILE)
	@${ECHO} "#############################################"    >> $(TARGET_FILE)
	@${ECHO} "# others"                                  	    >> $(TARGET_FILE)
	@${ECHO} "#############################################"    >> $(TARGET_FILE)
	@${ECHO} -e ${foreach flag, ${EXPORT_BOARD_FLAGS}, "${flag} :=\t ${${flag}}\n"} >> $(TARGET_FILE)

############################ Rule to generate the binary ##################################

ifeq "${BINGEN}" "yes"

GEN_COMMON_ELF_LD_FILE_FUNC = ${OBJDUMP} -h ${1} | perl -e ' \
    $$filename = "${1}"; \
    $$filename =~ s/.*\///; \
    while(<>) { \
        if (/^ *[0-9]+ +([^ ]+) +[0-9a-zA-Z]+ +([0-9a-zA-Z]+) +([0-9a-zA-Z]+) /) { \
            $$vma = hex($$2); \
            $$lma = hex($$3); \
            next if ($$vma==0 || $$lma==0); \
            $$secstr = $$1; \
            $$vmastr = "0xffffffff$$2"; \
            $$lmastr = "0xffffffff$$3"; \
            $$loadstr = (index(<>,"LOAD")<0)?"(NOLOAD)":""; \
            printf("$$secstr ($$vmastr) $$loadstr : AT ($$lmastr) { KEEP(*$$filename($$secstr)) }\n"); \
         } \
     }'

ifeq "${REWRITE_ROM_AT_BOOT}-${CT_ASIC_CFG}" "1-fpga"
ifeq "${BUILD_BCPU_IMAGE}" "1"
GEN_ROM_ELF_LD_ROM_START := ${BCPU_ROM_BASE}
GEN_ROM_ELF_LD_ROM_END := ${BCPU_ROM_END}
GEN_ROM_ELF_LD_ROM_FLASH_SYM_PREFIX := bcpu_
else # !BUILD_BCPU_IMAGE
GEN_ROM_ELF_LD_ROM_START := ${INT_ROM_BASE}
GEN_ROM_ELF_LD_ROM_END := ${INT_ROM_END}
GEN_ROM_ELF_LD_ROM_FLASH_SYM_PREFIX := int_
endif # !BUILD_BCPU_IMAGE
GEN_ROM_ELF_LD_FILE_FUNC = ${OBJDUMP} -h ${1} | perl -e ' \
    $$rom_start_str = "${GEN_ROM_ELF_LD_ROM_START}"; \
    $$rom_start_str =~ s/^0[xX][fF]{8}//; \
    $$rom_end_str = "${GEN_ROM_ELF_LD_ROM_END}"; \
    $$rom_end_str =~ s/^0[xX][fF]{8}//; \
    $$rom_start = hex($$rom_start_str); \
    $$rom_end = hex($$rom_end_str); \
    $$rom_flash_sym_name = "${GEN_ROM_ELF_LD_ROM_FLASH_SYM_PREFIX}rom_tmp_flash"; \
    $$rom_section_index = 0; \
    $$rom_last_section_lma = $$rom_start; \
    $$rom_last_section_index = $$rom_section_index; \
    $$rom_first_section_index = $$rom_section_index; \
    printf("$${rom_flash_sym_name}_%u = .;\n", $$rom_first_section_index); \
    while (<>) { \
        if (/ *[0-9]+ +([^ ]+) +[0-9a-zA-Z]+ +([0-9a-zA-Z]+) +([0-9a-zA-Z]+) /) { \
            $$vma = hex($$2); \
            $$lma = hex($$3); \
            next if ($$vma==0 || $$lma==0); \
            $$secstr = $$1; \
            $$vmastr = "0xffffffff$$2"; \
            $$lmastr = "0xffffffff$$3"; \
            $$loadstr = (index(<>,"LOAD")<0)?"(NOLOAD)":""; \
            if ($$lma>=$$rom_start && $$lma<=$$rom_end) { \
                $$lmastr = sprintf("$${rom_flash_sym_name}_%u + (0x%08x - 0x%08x)", \
                    $$rom_first_section_index, $$lma, $$rom_start); \
            } \
            printf("$$secstr ($$vmastr) $$loadstr : AT ($$lmastr) { KEEP(${1}($$secstr)) }\n"); \
            if ($$lma>=$$rom_start && $$lma<=$$rom_end) { \
                printf("$${rom_flash_sym_name}_%u = $$lmastr + SIZEOF($$secstr);\n", \
                    $$rom_section_index+1); \
                $$rom_section_index++; \
                if($$lma > $$rom_last_section_lma) { \
                    $$rom_last_section_lma = $$lma; \
                    $$rom_last_section_index = $$rom_section_index; \
                } \
            } \
        } \
    } \
    printf(". = $${rom_flash_sym_name}_%u;\n", $$rom_last_section_index);'
else # !(REWRITE_ROM_AT_BOOT && CT_ASIC_CFG=fpga)
GEN_ROM_ELF_LD_FILE_FUNC = ${GEN_COMMON_ELF_LD_FILE_FUNC}
endif # !(REWRITE_ROM_AT_BOOT && CT_ASIC_CFG=fpga)

GEN_COMPRESS_STAGE1_ELF_LD_FILE_FUNC = ${OBJDUMP} -h ${1} | perl -e ' \
    $$filename = "${BIN_STAGE1_LINKABLE}"; \
    $$filename =~ s/.*\///; \
    while(<>) { \
        if (/^ *[0-9]+ +([^ ]+) +[0-9a-zA-Z]+ +([0-9a-zA-Z]+) +([0-9a-zA-Z]+) /) { \
            $$vma = hex($$2); \
            $$lma = hex($$3); \
            next if ($$vma==0 || $$lma==0); \
            $$secstr = $$1; \
            $$vmastr = "0xffffffff$$2"; \
            $$lmastr = "0xffffffff$$3"; \
            $$loadstr = (index(<>,"LOAD")<0)?"(NOLOAD)":""; \
            if ($$secstr eq ".get_compressed_data") { \
                printf("_get_compressed_data_start = $$vmastr;\n"); \
            } else { \
                printf("$$secstr ($$vmastr) $$loadstr : AT ($$lmastr) { KEEP(*$$filename($$secstr)) }\n"); \
            } \
        } \
    }'

GEN_COMPRESS_ELF_SECTION_ADDR_FILE_FUNC = ${OBJDUMP} -h ${1} | perl -e ' \
    while(<>) { \
        if (/^ *[0-9]+ +([^ ]+) +([0-9a-zA-Z]+) +([0-9a-zA-Z]+) +([0-9a-zA-Z]+) /) { \
            $$vma = hex($$3); \
            $$lma = hex($$4); \
            next if ($$vma==0 || $$lma==0); \
            $$secstr = $$1; \
            $$sizestr = (hex($$2)==0)?"0x0":"0x$$2"; \
            $$vmastr = "0x$$3"; \
            $$lmastr = "0x$$4"; \
            if ($$secstr eq ".get_compressed_data") { \
                printf("$$secstr <> $$vmastr $$lmastr\n"); \
            } elsif ($$secstr ne ".user_data") { \
                printf("$$secstr $$sizestr $$vmastr $$lmastr\n"); \
            } \
        } \
    }'

BIN_VAR_OPTIONAL_PREREQUISITES := 
LD_OPTIONAL_OBJECTS := 

BIN_VAR_PR_WITH_OPTIONS := 1
ifeq "${BUILD_ROM}" "1"
BIN_VAR_PR_WITH_OPTIONS := 0
endif
ifeq "${BUILD_BCPU_IMAGE}" "1"
BIN_VAR_PR_WITH_OPTIONS := 0
endif
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
BIN_VAR_PR_WITH_OPTIONS := 0
endif

ifeq "${BIN_VAR_PR_WITH_OPTIONS}" "1"
# With compression
ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"
BIN_VAR_OPTIONAL_PREREQUISITES += ${COMPRESSED_DATA_OBJECTS} ${COMPRESSED_DATA_OBJECTS_CHECKSUM}
LD_OPTIONAL_OBJECTS += ${COMPRESSED_DATA_OBJECTS}
LD_OPTIONAL_OBJECTS += ${COMPRESSED_DATA_OBJECTS_CHECKSUM}
endif
ifeq "${USERGEN}" "yes"
# No compression or compression stage 2
ifneq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-no"
BIN_VAR_OPTIONAL_PREREQUISITES +=  ${USR_DATA_OBJECTS}
LD_OPTIONAL_OBJECTS += ${USR_DATA_OBJECTS}
endif
endif # USERGEN
endif # BIN_VAR_PR_WITH_OPTIONS


${BIN}: dependencies clean_disassembly ${FULL_SRC_OBJECTS} ${FULL_LIBRARY_FILES} ${BIN_VAR_OPTIONAL_PREREQUISITES} | ${BIN_PATH} ${BINARY_PATH}
	@${ECHO}
# Keeping being done after, it has the priority on the stripping.
# ie: when STRIP_ROM_OBJ2STRIP_FILE and STRIP_ROM_OBJ2KEEP_FILE are
# both defined, only STRIP_ROM_OBJ2KEEP_FILE is taken into account.
ifneq "${STRIP_ROM_OBJ2STRIP_FILE}" ""
#	# Strip and get from the standard ROM a subset of the symbols which
#	# are needed anyway.
	${ECHO} "GEN               stripped (rm syms) elf file ${notdir $(STRIP_ROM_ELF_FILE_PATHED)}"
	${OBJCOPY} --strip-symbols=${STRIP_ROM_OBJ2STRIP_FILE_PATHED} \
							   ${CHIP_ROM_ELF_FILE_FULL} \
							   ${STRIP_ROM_ELF_FILE_PATHED}
endif # STRIP_ROM_OBJ2STRIP_FILE
#	#
ifneq "${STRIP_ROM_OBJ2KEEP_FILE}" ""
#	# Strip and get from the standard ROM a subset of the symbols which
#	# are needed anyway.
	${ECHO} "GEN               stripped (keep syms) elf file ${notdir $(STRIP_ROM_ELF_FILE_PATHED)}"
	${OBJCOPY} -S --keep-symbols=${STRIP_ROM_OBJ2KEEP_FILE_PATHED} \
								 ${CHIP_ROM_ELF_FILE_FULL} \
								 ${STRIP_ROM_ELF_FILE_PATHED}
endif # STRIP_ROM_OBJ2KEEP_FILE
#	#
ifneq "${BCPU_ELF_FILE_STRIPPED}" ""
#	# Strip and get from the BCPU ELF a subset of the sections which
#	# are needed anyway.
	${ECHO} "GEN               stripped (rm syms) elf file ${notdir $(BCPU_ELF_FILE_STRIPPED)}"
	${OBJCOPY} --strip-symbols=${STRIP_BCPU_OBJ2STRIP_FILE_PATHED} \
							   ${BCPU_ELF_FILE_FULL} \
							   ${BCPU_ELF_FILE_STRIPPED}
endif
#	#
	@${ECHO} "LDGEN             ${notdir ${LD_FILE}}"
	test -f ${LD_FILE} && chmod +w ${LD_FILE} || echo ""

# Keep RAMINIT (and possibly ROM) contents in flash elf file
ifneq "${USED_RAMINIT_ELF_FILE}" ""
	${call GEN_ROM_ELF_LD_FILE_FUNC,${USED_RAMINIT_ELF_FILE}} > ${BUILD_ROOT}/link_raminit_elf.ld
else # USED_RAMINIT_ELF_FILE == ""
# Keep ROM contents in flash elf file
ifeq "${LINK_ROM_ELF}" "1"
ifneq "${LINK_ROM_SYMBOL_ONLY}" "1"
ifneq "${USED_ROM_ELF_FILE}" ""
	${call GEN_ROM_ELF_LD_FILE_FUNC,${USED_ROM_ELF_FILE}} > ${BUILD_ROOT}/link_rom_elf.ld
endif # USED_ROM_ELF_FILE == ""
endif # LINK_ROM_SYMBOL_ONLY != 1
endif # LINK_ROM_ELF
endif # USED_RAMINIT_ELF_FILE == ""

# Load BCPU elf contents into XCPU flash elf file
ifneq "${BUILD_ROM}" "1"
ifneq "${USED_BCPU_ELF_FILE}" ""
	${call GEN_COMMON_ELF_LD_FILE_FUNC,${USED_BCPU_ELF_FILE}} > ${BUILD_ROOT}/link_bcpu_elf.ld
endif
endif

# No compression or compression stage 1
ifneq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-yes"
#	# Workaround:
#	# Make overlaylinkers directly in commands rather than in prerequisites,
#	# so as to avoid jobserver internal error during parallel execution when
#	# building compressed applications in Cygwin.
#	# Cygwin cannot handle the complexity of overlaylinkers (function expansion)
#	# during parallel execution in some cases (E.g., there is one .o being
#	# compiled).
#	# There are 2 symptoms when this error occurs:
#	# 1. Debug logs show "Reaping losing child ..." and make exits with
#	#    error "INTERNAL: Exiting with N jobserver tokens available; should be M!"
#	# 2. Make crashes with segmentation fault
	${MAKE} overlaylinkers OVERLAYLDGEN=yes
else # Compression stage 2
	${call GEN_COMPRESS_STAGE1_ELF_LD_FILE_FUNC,${BIN_STAGE1}} > ${BUILD_ROOT}/link_stage1_elf.ld
endif

	${LDPP} ${LDPPFLAGS} ${LD_SRC} > ${LD_FILE}
#	#
	@${ECHO} "LD                ${notdir ${BIN}}"
#	#
ifneq "${CLEAN_USER_SECTOR}${CLEAN_CALIB_SECTOR}${CLEAN_FACT_SECTOR}" "000"
#	#
#	# Generate sector cleaner lods only.
#	# We do not care about the non-existed prerequisite libs here, as
#	# the implicit rule %.a has suppress the issue.
	${LD} -nostdlib -o ${BIN} ${OFORMAT}    \
		${LDFLAG_USED_ELF_FILES}	\
		--script ${LD_FILE} \
		-Map ${MAP}
#	#
else # CLEAN_USER_SECTOR
#	#
# No compression or compression stage 1
ifneq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-yes"
	${LD} -nostdlib  -o ${BIN} ${OFORMAT} ${FULL_SRC_OBJECTS} ${LD_OPTIONAL_OBJECTS} \
		${LDFLAG_USED_ELF_FILES}	\
		--script ${LD_FILE} \
		$(LDFLAGS) \
		-Map ${MAP}
else # Compression stage 2
	${OBJCOPY} -R .get_compressed_data -R .compressed_data.crc ${BIN_STAGE1} ${BIN_STAGE1_LINKABLE}
	${LD} -nostdlib  -o ${BIN} ${OFORMAT} ${BIN_STAGE1_LINKABLE} ${LD_OPTIONAL_OBJECTS} \
		--script ${LD_FILE} \
		--warn-common --error-unresolved-symbols \
		-Map ${MAP}
#	# Verify the stage2 elf file
	@${ECHO} "VERIFY            Stage 2 Elf binary"
	${call GEN_COMPRESS_ELF_SECTION_ADDR_FILE_FUNC,${BIN_STAGE1}} | sort > ${BIN_PATH}/section_header.txt${STAGE1_POSTFIX}
	${call GEN_COMPRESS_ELF_SECTION_ADDR_FILE_FUNC,${BIN}} | sort > ${BIN_PATH}/section_header.txt
	${DIFF}  ${BIN_PATH}/section_header.txt${STAGE1_POSTFIX} ${BIN_PATH}/section_header.txt
endif
#	#
endif # CLEAN_USER_SECTOR
#	#
ifneq "${IS_DLL_ENTRY}" "yes"
	@${ECHO} "CP                Elf binary & map file"
	${CP} ${BIN} ${BIN_FINAL}
	${CP} ${MAP} ${MAP_FINAL}
endif

endif # BINGEN

############################ hex files #######################################
# Generate srec and disassembly files, same logic as bin
# Always build ${HEX}
${HEX}: force
	${MAKE} ${BIN} BINGEN=yes
	@${ECHO}
	$(OBJCOPY) ${OBJCPY_OPTS} ${SREC} ${BIN} ${HEX}

.PHONY: dump dumpstem

dumpstem:
	@${ECHO}
ifeq "$(wildcard ${BIN})" ""
	@${ECHO} "DUMP Skipped"
else # BIN
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "DUMP BCPU         ${notdir ${DIS}}"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "DUMP RAMINIT      ${notdir ${DIS}}"
else
	@${ECHO} "DUMP              ${notdir ${DIS}}"
endif
endif
	$(OBJDUMP) --demangle=auto ${OBJDUMP_OPTS} ${BIN} ${USED_ROM_ELF_FILE} > ${DIS}
ifeq "${BUILD_BCPU_IMAGE}" "1"
	@${ECHO} "CP   BCPU         Disassembly file"
else
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
	@${ECHO} "CP   RAMINIT      Disassembly file"
else
	@${ECHO} "CP                Disassembly file"
endif
endif
	cp -f ${DIS} ${DIS_FINAL}
endif # BIN

dump:
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} dumpstem BUILD_BCPU_IMAGE=1
endif
endif
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} dumpstem BUILD_RAMINIT_IMAGE=1
endif
endif
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} dumpstem
endif

############################# dll file ##########################################
${DLL}: ${BIN} | ${BINARY_PATH}
	@${ECHO}
ifneq "${EXP}" ""
	elf2dll  -i ${BIN} -x ${EXP} -o ${DLL}
else
	elf2dll  -i ${BIN} -o ${DLL}
endif
	@${ECHO}
	@${ECHO} "CP                Elf binary & map file"
	@${ECHO}
	@${ECHO} "${notdir ${DLL}} generated"
	cp -f ${BIN} ${BIN_FINAL}
	cp -f ${MAP} ${MAP_FINAL}
	cp -f ${DLL} ${DLL_FINAL}


dlldump:
	@${ECHO}
	@${ECHO} "DUMP              ${notdir ${DIS}}"
	echo OBJDUMP=$(OBJDUMP)
	$(OBJDUMP) -D -T -s -t ${BIN} > ${DIS}
	@${ECHO} "CP                Disassembly file"
	cp -f ${DIS} ${DIS_FINAL}

############################# modem_image file ##################################
ifeq "${GEN_MODEM_IMAGE}" "1"

.PHONY: modem_image

ifeq "${RAMINITGEN}"  "yes"
RAMINIT_SREC_BIN_FILE := ${BAS}_modemIntsram.bin
RAMINIT_SREC_IMAGE_FILE := ${BAS}_modemIntsram.img
include ${BAS}_modemIntsram.def
RAMINIT_LOAD_ADDR := ${SREC_BIN_LOAD_ADDR}
RAMINIT_ENTRY_ADDR := $(shell ${NM} ${BIN} | \
    grep '_boot_sector' | \
    sed 's/^\([0-9a-fA-F]*\) . \([^ ]*\)$$/0x\1/')
endif

SREC_BIN_FILE := ${BAS}_modemRam.bin
SREC_IMAGE_FILE := ${BAS}_modemRam.img
include ${BAS}_modemRam.def

ifeq "${CHIP_HAS_WCPU}" "1"
WCPU_SREC_BIN_FILE := ${SOFT_WORKDIR}/platform/chip/wcpu/main/wcpu_main.bin
WCPU_SREC_IMAGE_FILE := ${BAS}_WCPU_modemRam.img
WCPU_SREC_BIN_LOAD_ADDR :=$(subst 0xFFFFFFFF,0x,${WD_RAM_BASE})
WCPU_SREC_BIN_ENTRY_ADDR := 0
endif

TOTAL_IMAGE_FILE := ${LODBASE}modemTotal.img

modem_image:
ifeq "${RAMINITGEN}"  "yes"
	@${ECHO}
	@${ECHO} "MKIMAGE           $(notdir ${RAMINIT_SREC_IMAGE_FILE})"
	mkimage -A mips -C none -a ${RAMINIT_LOAD_ADDR} -e ${RAMINIT_ENTRY_ADDR} \
        -n "Modem raminit codes" -d ${RAMINIT_SREC_BIN_FILE} ${RAMINIT_SREC_IMAGE_FILE}
endif
	@${ECHO}
	@${ECHO} "MKIMAGE           $(notdir ${SREC_IMAGE_FILE})"
	mkimage -A mips -C none -a ${SREC_BIN_LOAD_ADDR} -e ${SREC_BIN_ENTRY_ADDR} \
        -n "Modem work codes" -d ${SREC_BIN_FILE} ${SREC_IMAGE_FILE}
	@${ECHO}
	
  
ifeq "${CHIP_HAS_WCPU}" "1"
ifneq "${MODEM_RECOVER_IMG}" "YES"	
	@${ECHO} "MKIMAGE           $(notdir ${WCPU_SREC_IMAGE_FILE})"
	mkimage -A arm -C none -a ${WCPU_SREC_BIN_LOAD_ADDR} -e ${WCPU_SREC_BIN_ENTRY_ADDR} \
        -n "Wcpu Modem work codes" -d ${WCPU_SREC_BIN_FILE} ${WCPU_SREC_IMAGE_FILE}
	@${ECHO}
	
	@${ECHO} "CAT               $(notdir ${TOTAL_IMAGE_FILE})"
	cat ${RAMINIT_SREC_IMAGE_FILE} ${SREC_IMAGE_FILE}  ${WCPU_SREC_IMAGE_FILE} > ${TOTAL_IMAGE_FILE}
	@${ECHO}
else
	@${ECHO} "CAT               $(notdir ${TOTAL_IMAGE_FILE})"
	cat ${RAMINIT_SREC_IMAGE_FILE} ${SREC_IMAGE_FILE} > ${TOTAL_IMAGE_FILE}
	@${ECHO}
endif
else
	
	@${ECHO} "CAT               $(notdir ${TOTAL_IMAGE_FILE})"
	cat ${RAMINIT_SREC_IMAGE_FILE} ${SREC_IMAGE_FILE} > ${TOTAL_IMAGE_FILE}
	@${ECHO}

endif
	
	
	@${ECHO} "---------------------------------------------------"
	@${ECHO} "$(notdir ${TOTAL_IMAGE_FILE}) Generated"
	@${ECHO} "---------------------------------------------------"

endif # GEN_MODEM_IMAGE

ifeq (${GEN_RAMRUN_IMAGE},1)
.PHONY: ramrun_bootimage
BOOT_SREC_BIN_FILE := ${BAS}_ramrunImage.bin
BOOT_SREC_IMAGE_FILE := ${BAS}_ramrun.img

include ${BAS}_ramrunImage.def
BOOT_LOAD_ADDR := ${SREC_BIN_LOAD_ADDR}
BOOT_ENTRY_ADDR := $(shell ${NM} ${BIN} | \
    grep '_boot_sector' | \
    sed 's/^\([0-9a-fA-F]*\) . \([^ ]*\)$$/0x\1/')

TOTAL_RAMRUN_IMAGE_FILE := ${BINARY_PATH}/u-ramrun.img

ramrun_bootimage:
	@${ECHO}
	@${ECHO} "MKIMAGE           $(notdir ${BOOT_SREC_IMAGE_FILE})"
	mkimage -A mips -C none -a ${BOOT_LOAD_ADDR} -e ${BOOT_ENTRY_ADDR} \
        -n "ramrun Image" -d ${BOOT_SREC_BIN_FILE} ${BOOT_SREC_IMAGE_FILE}
	cp -f ${BOOT_SREC_IMAGE_FILE} ${TOTAL_RAMRUN_IMAGE_FILE}
	@${ECHO}
	@${ECHO} "---------------------------------------------------"
	@${ECHO} "$(notdir ${TOTAL_RAMRUN_IMAGE_FILE}) Generated"
	@${ECHO} "---------------------------------------------------"
endif #GEN_BOOT_IMAGE
############################ Lod files ##########################################
## bin is the generic .lod generating rule.
# It creates the lod file for the current board, whatever that is.
.PHONY: bin

ifeq "${CT_ROMULATOR}" "y"
LDPPFLAGS   += -DROMULATOR=1
endif

SKIP_LOD_GEN := 0
SKIP_XCP_XMD_FINAL_GEN := 0
# Compression stage 1
ifeq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-no"
SKIP_LOD_GEN := 1
SKIP_XCP_XMD_FINAL_GEN := 1
endif
# Building BCPU image
ifeq "${BUILD_BCPU_IMAGE}" "1"
SKIP_LOD_GEN := 1
SKIP_XCP_XMD_FINAL_GEN := 1
endif
# Building RAMINIT image
ifeq "${BUILD_RAMINIT_IMAGE}" "1"
SKIP_LOD_GEN := 1
SKIP_XCP_XMD_FINAL_GEN := 1
endif
# Building ROM
ifeq "${BUILD_ROM}" "1"
SKIP_XCP_XMD_FINAL_GEN := 1
endif

ifneq "${IS_ENTRY_POINT}" "yes"

bin: force
	@echo "***************************************"
	@echo "* Need to be in an entry point module *"
	@echo "* to generate lod files ! Aborted.    *"
	@echo "***************************************"

else # IS_ENTRY_POINT

ifeq "${BUILD_BCPU_IMAGE}" "1"
ifeq "$(strip ${LOCAL_MODULE_DEPENDS} ${FULL_SRC_OBJECTS})" ""
$(error Cannot build BCPU image. Please add "export NO_BCPU_IMAGE := 1" in your Makefile)
endif
endif

bin: all
	@${ECHO}
#	#
ifeq "${BUILD_ROM}" "1"
#	#
			@${ECHO} "SREC              ${LODBASE_NO_PATH}.srec for rom"
			srecmap -c ${MAP_FILE} -m rom ${HEX} ${LODBASE} ${STDOUT_NULL}
			@${ECHO}
			@${ECHO} "---------------------------------------------------"
			@${ECHO} "${LODBASE_NO_PATH}_introm.lod Generated"
			@${ECHO} "---------------------------------------------------"
#	#
else # !BUILD_ROM
#	#
ifneq "${SKIP_LOD_GEN}" "1"
#	#
ifeq "${CT_ASICSIMU}" "1"
		@${ECHO} "SREC              ${LODBASE_NO_PATH}.srec for simulation"
		srecmap -c ${MAP_FILE} -m simu ${HEX} ${LODBASE} ${STDOUT_NULL}
#	#
else # !CT_ASICSIMU}
#	#
ifeq "${CT_RAMRUN}" "1"
				@${ECHO} "SREC              ${LODBASE_NO_PATH}.srec for ramrun"
ifeq "${BUILD_BOOT_IMAGE}" "1"
				srecmap -c ${MAP_FILE} -m ramrunImage ${HEX} ${BAS}_ ${STDOUT_NULL}
				rm -f ${BINARY_PATH}/*.img
				${MAKE} ramrun_bootimage GEN_RAMRUN_IMAGE=1
else
				srecmap -c ${MAP_FILE} -m preload ${HEX} ${LODBASE} ${STDOUT_NULL}
				@${ECHO} "CAT               ${LODBASE_NO_PATH}_ramrun.lod"
				cat ${LODBASE}*ram.lod > ${LODBASE}ramrun.lod
				rm -f ${LODBASE}*ram.lod 
				rm -f ${LODBASE}*flash.lod 
				@${ECHO}
				@${ECHO} "---------------------------------------------------"
				@${ECHO} "${LODBASE_NO_PATH}_ramrun.lod Generated"
				@${ECHO} "---------------------------------------------------"
endif				
else # !CT_RAMRUN
#	#
ifeq "${CHIP_HAS_AP}" "1"
#	#
				@${ECHO} "SREC              ${LODBASE_NO_PATH}.srec for bin"
				srecmap -c ${MAP_FILE} -m ${FLSH_MODEL} ${HEX} ${BAS}_ ${STDOUT_NULL}
				@${ECHO}
				@${ECHO} "---------------------------------------------------"
				@${ECHO} "${LODBASE_NO_PATH}_modemRam.bin Generated"
				@${ECHO} "---------------------------------------------------"
				${MAKE} modem_image GEN_MODEM_IMAGE=1
#	#
else # !CHIP_HAS_AP
#	#
				@${ECHO} "SREC              ${LODBASE_NO_PATH}.srec for flash/romulator"
ifeq "${EXPORT_BOARD_INFO}" "y"
				${MAKE} targetgen
ifeq "${CT_BIN}" "1"
				srecmapbin -c ${MAP_FILE} -m ${FLSH_MODEL} -b ${TARGET_FILE} ${HEX} ${LODBASE} ${STDOUT_NULL}
else
				srecmap -c ${MAP_FILE} -m ${FLSH_MODEL} -b ${TARGET_FILE} ${HEX} ${LODBASE} ${STDOUT_NULL}
endif
else
ifeq "${CT_BIN}" "1"
				srecmapbin -c ${MAP_FILE} -m ${FLSH_MODEL} ${HEX} ${LODBASE} ${STDOUT_NULL}
else
				srecmap -c ${MAP_FILE} -m ${FLSH_MODEL} -b ${TARGET_FILE} ${HEX} ${LODBASE} ${STDOUT_NULL}
endif
endif
				@${ECHO}
				@${ECHO} "---------------------------------------------------"
ifeq "${CT_ROMULATOR}" "y"
#					# In case of romulator - mv the file to a romulator file
					mv ${LODBASE}flash.lod ${LODBASE}romulator.lod
					@${ECHO} "${LODBASE_NO_PATH}_romulator.lod Generated"
else
					@${ECHO} "${LODBASE_NO_PATH}_flash.lod Generated"
endif
				@${ECHO} "---------------------------------------------------"
#	#
endif # !CHIP_HAS_AP
#	#
endif # !CT_RAMRUN
#	#
endif # !CT_ASICSIMU
#	#
endif # !SKIP_LOD_GEN
#	#
ifeq "${COMPRESS_PROCESS_ENABLE}" "yes"
ifeq "${LINK_STAGE_2}" "yes"
	@${ECHO}
	@${ECHO} "---------------------------------------------------"
	@${ECHO} "The second stage of code compression is finished"
	@${ECHO} "---------------------------------------------------"
	@${ECHO}
else # !LINK_STAGE_2
	@${ECHO}
	@${ECHO} "---------------------------------------------------"
	@${ECHO} "The first stage of code compression is finished"
	@${ECHO} "Starting the second stage ..."
	@${ECHO} "---------------------------------------------------"
	@${ECHO}
	${MAKE} LINK_STAGE_2=yes MODULES_MUST_BUILD=link
	@${ECHO}
endif # !LINK_STAGE_2
endif # COMPRESS_PROCESS_ENABLE
#	#
ifneq "${SKIP_XCP_XMD_FINAL_GEN}" "1"
	${MAKE} ${XCP_FINAL}
	${MAKE} ${XMD_FINAL}
endif # !SKIP_XCP_XMD_FINAL_GEN
#	#
endif # !BUILD_ROM

endif # !IS_ENTRY_POINT

######################## target lod build #################################################
.PHONY: requestclean save_target lod

requestclean:
	@${ECHO} "REQ CLEAN         ${TARGET_TAG}"
	rm -f ${LAST_TARGET_FILE}

save_target: makedirs force
	@${ECHO} "SAVE TGT          ${TARGET_TAG}"
	echo "${TARGET_TAG}" > ${LAST_TARGET_FILE}

lod:
# Some ommands for 'lod' should avoid multiple execution when compression enabled
# No compression or compression stage 1
ifneq "${COMPRESS_PROCESS_ENABLE}-${LINK_STAGE_2}" "yes-yes"
#	#
# Respect the order clean->save_target->resgen->bin
#	#
ifneq '${LAST_TARGET}' ''
ifneq '${TARGET_TAG}' '${LAST_TARGET}'
	${MAKE} clean
	${MAKE} save_target
endif
else # No LAST_TARGET
	${MAKE} save_target
endif # No LAST_TARGET
#	#
ifeq "${RESGEN}" "yes"
	${MAKE} resgen
endif
#	#
ifeq "${USERGEN}"  "yes"
	${MAKE} usrgen
endif
#	#
ifeq "${AUTO_XMD2H}" "yes"
# All XMD header files are generated on the first run to make the target
	${MAKE} xmd2h
endif
#	#
ifeq "${BCPUGEN}"  "yes"
ifneq "${SKIP_BCPU_BUILD}" "1"
	${MAKE} bin BUILD_BCPU_IMAGE=1
endif
endif
#	#
ifeq "${RAMINITGEN}"  "yes"
ifneq "${SKIP_RAMINIT_BUILD}" "1"
	${MAKE} bin BUILD_RAMINIT_IMAGE=1
endif
endif
#	#
endif # COMPRESS_PROCESS_ENABLE && LINK_STAGE_2
#	#
ifneq "${SKIP_XCPU_BUILD}" "1"
	${MAKE} bin
endif

dll: all

################### "all" target #############################################
# Needs to come after everything else has been defined...
.PHONY: all

all: $(TOP_TARGET)  

##############################################################################
## Special targets
##############################################################################
################## binary lib ################################################
# copy from BUILD_ROOT to SOFT_WORKDIR if needed
# so source library apears as binary lib after make binlib
# while still using BUILD_ROOT for compilation

ifneq "${IS_ENTRY_POINT}" "yes"

.PHONY: binlib

binlib: all ${BINLIB_PATH} force
	@${ECHO} "CP                Library ${notdir ${LOCAL_SRCLIBRARY}}"
	cp -f ${LOCAL_SRCLIBRARY} ${LOCAL_BINLIBRARY}

endif # !IS_ENTRY_POINT


################## VOC BUILD #################################################
# Rules to build voc .tab from voc sources, only valid when voc tools are available
ifeq (${WITH_VOC_TOOLS},1)
ifneq (${LOCAL_VOCPROJ_NAME},)

# reset V_OBJ
V_OBJ :=

# reset V_SRC to force build when *.vocsrcs does not exist
V_SRC := force

# set V_OBJ & V_SRC if the deps file is available
-include ${LOCAL_SRC_DIR}/${LOCAL_VOCPROJ_NAME}.vocsrcs

# all mips objects depend on the voc objects (through the marker, the voc makefile being used as marker)
${FULL_SRC_OBJECTS} : ${LOCAL_SRC_DIR}/${LOCAL_VOCPROJ_NAME}.mx

# voc marker make rule (compilation is either forced or the dependency list is not empty)
${LOCAL_SRC_DIR}/${LOCAL_VOCPROJ_NAME}.mx : ${V_SRC}
	@${ECHO} "VOC               ${LOCAL_VOCPROJ_NAME}"
	voc_make -v -p ${LOCAL_VOCPROJ_NAME} -s ${LOCAL_SRC_DIR} -d ${LOCAL_SHADOW_PATH}/voccompiletmp -h ${SOFT_WORKDIR}/platform/chip/regs/${CT_ASIC}/include/voc_ram.h
	touch ${LOCAL_SRC_DIR}/${LOCAL_VOCPROJ_NAME}.mx

# voc clean rule
vocclean:
	@${ECHO} "VOCCLEAN          ${LOCAL_VOCPROJ_NAME}"
	rm -f ${V_OBJ} ${LOCAL_SRC_DIR}/*.vocsrcs

# when cleaning locally also clean voc
locclean: vocclean

endif
endif

##############################################################################
## Dependencies
##############################################################################
# Generated on-the-fly, one dependency file per source file, on each build, 
# and for use on next build

DEPFILES := $(foreach file, $(basename ${C_SRC_FILTERED} ${C++_SRC_FILTERED} ${S_SRC_FILTERED}), ${DEPS_REL_PATH}/${file}.d)
ifneq "$(DEPFILES)" ""
# allow fail on include, if depfiles haven't been generated yet (first build, or after depclean)
-include ${DEPFILES}
endif # DEPFILES

# The libraries are always generated before. This rule respond allways true.
%.a:
	echo -n ""


######################## info #################################
# Misc rules usable to debug or get information about the
# compilation environment
############################################################### 
.PHONY: showdepends dumpinfo dumpexportflags
showdepends:
	@echo -------------------------------------------------
	@echo Local built binary libs:
	@echo        ${LOCAL_BUILT_BINARY_LIBS}
	@echo Binary libs:
	@echo        $(BINARY_LIBS)
	@echo Src libs:
	@echo        $(SRC_LIBS)
	@echo Local additions:
	@echo "  libs    : ${LOCAL_LIBS}"
	@echo "  objects : ${LOCAL_OBJS}"
	@echo Objects from local source:
	@echo        ${FULL_SRC_OBJECTS}
	@echo -------------------------------------------------
	@echo Full Library files:
	@echo       $(FULL_LIBRARY_FILES)
	@echo -------------------------------------------------
	@echo Dependencies compile command: 
	@echo 		'${FULL_DEPENDENCY_COMPILE}' 
	@echo -------------------------------------------------


dumpexportflags:
	@echo -----------------------------------------------------
	@echo All export flags :
	@echo ${ALL_EXPORT_FLAG}
	@echo Global flags :
	@echo ${GLOBAL_EXPORT_FLAG}
	@echo Target flags :
	@echo ${TARGET_EXPORT_FLAG}
	@echo Chip flags :
	@echo ${CHIP_EXPORT_FLAG}
	@echo Local flags :
	@echo ${LOCAL_EXPORT_FLAG}
	@echo -----------------------------------------------------

dumpinfo:
	@echo -----------------------------------------------------
	@echo GCC version :
	$(CC) -dumpversion
	@echo SOFT_WORKDIR :
	@echo ${SOFT_WORKDIR}
	@echo BUILD_ROOT :
	@echo ${BUILD_ROOT}
	@echo CT_USER :
	@echo ${CT_USER}
	@echo CT_ASIC :
	@echo ${CT_ASIC}
	@echo CT_TARGET :
	@echo ${CT_TARGET}
	@echo CT_RELEASE :
	@echo ${CT_RELEASE}
	@echo CT_ROMULATOR :
	@echo ${CT_ROMULATOR}
	@echo CT_RAMRUN :
	@echo ${CT_RAMRUN}
	@echo "#################"
	@echo ASFLAGS :
	@echo ${ASFLAGS}
	@echo LDFLAGS :
	@echo ${LDFLAGS}
	@echo CFLAGS :
	@echo ${CFLAGS}
	@echo REDUCED_REGS_FLAG:
	@echo ${REDUCED_REGS_FLAG}
	@echo "#################"
	@echo Include Path :
	@echo ${INCLUDE_PATH}
	@echo LDPPFLAGS :
	@echo ${LDPPFLAGS}
	@echo Extra CPP flags :
	@echo ${MYCPPFLAGS}
	@echo "#################"
	@echo Top target :
	@echo ${TOP_TARGET}
	@echo Local objs :
	@echo ${FULL_SRC_OBJECTS}
	@echo Linked libs :
	@echo ${FULL_LIBRARY_EXT}
	@echo USED_SRC_IN_ROM :
	@echo ${USED_SRC_IN_ROM}
	@echo LINKER_MASTER :
	@echo ${LINKER_MASTER}
	@echo XCP_DEPENDS_INPUTS :
	@echo ${XCP_DEPENDS_INPUTS}
	@echo DOC_INPUT_MODULES :
	@echo ${DOC_INPUT_MODULES}
	@echo -----------------------------------------------------
	@echo C_SRC:
	@echo ${C_SRC}
	@echo C++_SRC:
	@echo ${C++_SRC}
	@echo TARGET_TAG
	@echo ${TARGET_TAG}
	@echo LAST_TARGET
	@echo ${LAST_TARGET}

##############################################################################

# To connect to CoolGdb in localhost.
COOLGDB_IP ?= 127.0.0.1
# For the BCPU:
#COOLGDB_PORT ?= 26332
# For the XCPU:
COOLGDB_PORT ?= 26331

gdb:
	@echo Launching GDB on remote CoolGDB @ ${COOLGDB_IP}:${COOLGDB_PORT}...
	$(GDB) ${BIN_FINAL} -ex "target remote ${COOLGDB_IP}:${COOLGDB_PORT}"

clewn:
	@echo Launching Clewn on remote CoolGDB @ ${COOLGDB_IP}:${COOLGDB_PORT}...
	echo "file ${BIN_FINAL}" > proj
	echo "target remote ${COOLGDB_IP}:${COOLGDB_PORT}" >> proj
	clewn -gc mips-elf-gdb -p proj 
	rm proj

##############################################################################
# Rules to build tags
##############################################################################
.PHONY: tags etags

TAGS_FILE 		?= ${SOFT_WORKDIR}/tags
TAGS_FILE_EMACS ?= ${SOFT_WORKDIR}/TAGS

# tag for vim users
tags: build_tags

build_tags: clean_tags
	ctags -f ${TAGS_FILE} -h +hp -R ${SOFT_WORKDIR}/platform/base
	ctags -f ${TAGS_FILE} -h +hp --exclude='${SOFT_WORKDIR}/platform/chip/bcpu/spal/models/*' -Ra ${SOFT_WORKDIR}/platform/chip/bcpu
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/boot/$(CT_ASIC)
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/boot/include
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/boot/src
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/defs
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/hal
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/patch
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/regs/$(CT_ASIC)
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/rom/$(CT_ASIC)
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/chip/pal
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/${PLATFORM_SERVICE}
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/edrv
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform/include
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/stack
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/vpp
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/platform_test
	ctags -f ${TAGS_FILE} -h +hp --exclude='${SOFT_WORKDIR}/application/coolmmi/firmware/*' -Ra ${SOFT_WORKDIR}/application
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/target
	ctags -f ${TAGS_FILE} -h +hp -Ra ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/mdi


clean_tags:
	rm -f ${TAGS_FILE}

# Tag for emacs users
etags:
	rm ${TAGS_FILE_EMACS}; for i in `find -L ${SOFT_WORKDIR} -name "*.[hc]"`;  do etags -o ${TAGS_FILE_EMACS} -a $$i; done

















##############################################################################

# Rules that is useful to get compilation
# environment variables into a shell script
# (Try make get_var_CT_ASIC.)
get_var_%:
# Single quotes are necessary to avoid variable expansion in the shell
# Return the unexpanded value
	@echo '$(value $*)'

get_expanded_var_%:
# Single quotes are necessary to avoid variable expansion in the shell
# Return the expanded value
# (eval always returns an empty string)
	@echo '$(eval EXP_VAR := $$($*))$(EXP_VAR)'


# Rules that is useful to get compilation
# environment variable origins.
# (Try make get_var_origin_CT_ASIC.)
get_origin_var_%:
# Single quotes are necessary to avoid variable expansion in the shell
	@echo '$(origin $*)'











# Finally include custom rules if present
CUSTOMRULES_EXIST := $(wildcard ${SOFT_WORKDIR}/env/compilation/customrules.mk)
ifneq ($(strip $(CUSTOMRULES_EXIST)),)
include ${SOFT_WORKDIR}/env/compilation/customrules.mk
endif




