## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := ${PLATFORM_SYSTEM}/mdi/alg/avi_enc

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS :=

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
						${PLATFORM_SYSTEM}/mdi \
						${PLATFORM_SYSTEM}/mdi/mmc \
						${PLATFORM_SYSTEM}/mdi/drivers \
						${PLATFORM_SYSTEM}/mdi/alg/wma_decoder \
						${PLATFORM_SYSTEM}/mdi/alg/amrring_codec_voc \
						${PLATFORM_SYSTEM}/mdi/alg/audiojpeg_dec_voc \
						${PLATFORM_SYSTEM}/mdi/alg/audiojpeg_enc_voc \
						${PLATFORM_SYSTEM}/mdi/alg/avi_dec \
						${PLATFORM_SYSTEM}/mdi/alg/avi_enc \
						${PLATFORM_SYSTEM}/mdi/alg/voc_common \
						${PLATFORM_SYSTEM}/svc/fss \
						${PLATFORM_SERVICE} \
						platform/chip/defs \
						platform/ \
						application/coolmmi/debug \
						application/systeminc/mmi \
						platform/chip/hal \
						platform/base/sx \


# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := no


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG += 

#CFLAGS += -isystem 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
