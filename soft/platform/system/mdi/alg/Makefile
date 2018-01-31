## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##

include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := ${PLATFORM_SYSTEM}/mdi/alg

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS := \
						${PLATFORM_SYSTEM}/mdi/alg/wma_decoder \
						${PLATFORM_SYSTEM}/mdi/alg/audiojpeg_dec_voc \
						${PLATFORM_SYSTEM}/mdi/alg/audiojpegenc \
						${PLATFORM_SYSTEM}/mdi/alg/avi_enc \
						${PLATFORM_SYSTEM}/mdi/alg/preview \
						${PLATFORM_SYSTEM}/mdi/alg/common \
						${PLATFORM_SYSTEM}/mdi/alg/pngdec \
						${PLATFORM_SYSTEM}/mdi/alg/meitu_filter_voc \

ifeq "${VIDEO_PLAYER_SUPPORT}" "y"

LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/avi_dec \
						${PLATFORM_SYSTEM}/mdi/alg/h263_dec \
						${PLATFORM_SYSTEM}/mdi/alg/mpeg4_dec \
						${PLATFORM_SYSTEM}/mdi/alg/isomedia_demux

ifeq "${MEDIA_RM_SUPPORT}" "y" 						
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/rm_dec \
						${PLATFORM_SYSTEM}/mdi/alg/ra8lbr_dec
endif

ifeq "${MEDIA_H264_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/h264_dec
endif

ifeq "${MEDIA_DRA_SUPPORT}" "1" 						
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/dra_dec
LOCAL_BINARY_LIBS    += ${PLATFORM_SYSTEM}/mdi/alg/dra_dec
endif

ifeq "${MEDIA_VOCVID_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/aac_dec_c
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/mp3mad_dec_c
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/amr_dec_c
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/sbcenc
endif

endif
						


# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
#						${PLATFORM_SYSTEM}/mdi \


ifeq "${ANALOG_TV_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/analogtv
endif
					
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/mdi/alg/analog_audio_iir

# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := yes


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG += 




## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC :=
C_SRC := 

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk




