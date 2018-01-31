## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/lcdd

# Include the target definition here to know whic libs to link to
LOCAL_MODULE_DEPENDS :=${foreach LCD, ${LCD_MODEL}, platform/edrv/lcdd/${LCD}}
LOCAL_MODULE_DEPENDS +=${foreach SUBLCD, ${SUBLCD_MODEL}, platform/edrv/lcdd/${SUBLCD}}
# List all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
                     platform/base/common \
                     platform/base/sx \
                     platform/chip \
                     platform/chip/hal \
                     platform/chip/defs\
                     platform/edrv/pmd	\
                     platform/edrv	\
					 platform/edrv/memd	\
                     platform/service/base \
                     target 

# This is a top-level module
IS_TOP_LEVEL := yes

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generates the CoolProfile headers automatically.
AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

comma :=,
empty :=#
space := $(empty) $(empty)#
TGT_LCD_REGINIT := ${foreach LCD, ${LCD_MODEL},lcdd_${LCD}_RegInit$(comma)}
LCDD_REGINIT_LIST:=$(subst $(space),$(empty),$(TGT_LCD_REGINIT))

TGT_SUBLCD_REGINIT := ${foreach SUBLCD, ${SUBLCD_MODEL},lcdd_${SUBLCD}_RegInit$(comma)}
SUBLCDD_REGINIT_LIST:=$(subst $(space),$(empty),$(TGT_SUBLCD_REGINIT))

LOCAL_EXPORT_FLAG += LCDD_REGINIT_LIST=$(LCDD_REGINIT_LIST)
LOCAL_EXPORT_FLAG += SUBLCDD_REGINIT_LIST=$(SUBLCDD_REGINIT_LIST)
#$(info TGT_LCD_new= ${LCDD_REGINIT_LIST})

# Compilation mode for the version string.
LOCAL_EXPORT_FLAG += LCDD_LCD_MODEL=${subst $(space),/,${strip ${LCD_MODEL}}}
LOCAL_EXPORT_FLAG += LCDD_SUBLCD_MODEL=${subst $(space),/,${strip ${SUBLCD_MODEL}}}


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
S_SRC :=
C_SRC := lcdd.c lcdd_map.c  
INPUT_FILES := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
