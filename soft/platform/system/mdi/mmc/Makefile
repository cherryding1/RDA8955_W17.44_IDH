## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := ${PLATFORM_SYSTEM}/mdi/mmc

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS :=

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
					${PLATFORM_SYSTEM}/mdi \
					${PLATFORM_SYSTEM}/mdi/alg/common \
					${PLATFORM_SYSTEM}/mdi/alg/isp \
					${PLATFORM_SYSTEM}/mdi/alg/wma_decoder \
					${PLATFORM_SYSTEM}/mdi/alg/amrring_codec_voc \
					${PLATFORM_SYSTEM}/mdi/alg/audiojpeg_dec_voc \
					${PLATFORM_SYSTEM}/mdi/alg/audiojpegenc \
					${PLATFORM_SYSTEM}/mdi/alg/preview\
					${PLATFORM_SYSTEM}/mdi/alg/avi_dec \
					${PLATFORM_SYSTEM}/mdi/alg/avi_enc \
					${PLATFORM_SYSTEM}/mdi/alg/h263_dec \
					${PLATFORM_SYSTEM}/mdi/alg/h264_dec \
					${PLATFORM_SYSTEM}/mdi/alg/mpeg4_dec \
					${PLATFORM_SYSTEM}/mdi/alg/rm_dec \
					${PLATFORM_SYSTEM}/mdi/alg/ra8lbr_dec \
					${PLATFORM_SYSTEM}/mdi/alg/aac_dec_c \
					${PLATFORM_SYSTEM}/mdi/alg/mp3mad_dec_c \
					${PLATFORM_SYSTEM}/mdi/alg/amr_dec_c \
					${PLATFORM_SYSTEM}/mdi/alg/dra_dec \
					${PLATFORM_SYSTEM}/mdi/alg/meitu_filter_voc \
					${PLATFORM_SYSTEM}/mdi/alg/isomedia_demux \
					${PLATFORM_SYSTEM}/mdi/alg/analogtv \
					${PLATFORM_SYSTEM}/mdi/alg/pngdec \
					${PLATFORM_SYSTEM}/mdi/alg/sbcenc \
					${PLATFORM_SERVICE} \
					platform/edrv/lcdd \
					platform/chip/defs \
					platform/ \
					application/coolmmi/debug \
					application/systeminc\
					application/systeminc/mmi \
					application/coolmmi/interface/config \
					application/coolmmi/mmi/AsyncEvents \
					application/coolmmi/media/common \
					application/coolmmi \
					application/coolmmi/adaptation \
					application/adaptation/custom \
					application/adaptation/device \
					application/systeminc/interface/config \
					application/systeminc/interface/hwdrv \
					application/systeminc/interface/l1audio \
					application/systeminc/interface/l1interface \
					application/systeminc/interface/media \
					application/systeminc/interface/mmi \
					application/systeminc/interface/os \
					application/systeminc/interface/ps \
					application/coolmmi/mmi \
					application/coolmmi/mmi/Audio \
					application/coolmmi/mmi/DateTime \
					application/coolmmi/mmi/DateTime \
					application/coolmmi/mmi/FileSystem \
					application/coolmmi/mmi/Framework \
					application/coolmmi/mmi/GUI \
					application/coolmmi/mmi/IdleScreen \
					application/coolmmi/mmi/MiscFramework \
					application/coolmmi/mmi/PROFILES \
					application/coolmmi/mmi/gpio \
					application/coolmmi/mmi_csdapp/EngineerMode \
					application/coolmmi/mmi_csdapp/GDI \
					application/coolmmi/mmi_csdapp/MDI \
					application/coolmmi/mmi_csdapp/mmiapi \
					application/systeminc/ps/interfaces \
					application/systeminc/ps/l4 \
					application/coolmmi/mmi/di \
					platform/base/std \
					platform/base/common \
					platform/base/sx \
					${PLATFORM_SYSTEM}/calib \
					platform/chip/regs/$(CT_ASIC)\
					platform/chip/defs \
					platform/chip/hal \
					platform/edrv/aud \
					platform/edrv/aud/fm \
					platform/edrv/fmd \
					target \
					platform/edrv/camd \
					platform/edrv/atvd \
					platform/edrv/pmd \
					platform/edrv/cammd \
					platform \
					${PLATFORM_SYSTEM}/svc/uctls \
					${PLATFORM_SERVICE} \
					${PLATFORM_SYSTEM}/svc/ars         \
					${PLATFORM_SYSTEM}/vpp/speech \
					application/coolmmi/mmi/DI \
					application/coolmmi/custom/drv \
					application/coolmmi/interface/hwdrv \
					application/coolmmi/mmi_csdapp/Camera \
					application/coolmmi/mmi_csdapp/filemgr \
					application/coolmmi/mmi_csdapp/fmt \
					application/coolmmi/mmi_csdapp/FileMgr \
					application/coolmmi/mmi/Resource \
					application/coolmmi/mmi_customer \
					application/coolmmi/mmi_csdapp/EbookReader \
					application/coolmmi/media/image \
					application/coolmmi/custom/app \
					application/coolmmi/custom/common \
					application/coolmmi/sst \
					application/coolmmi/interface/adaptation \
					application/coolmmi/mmi_csdapp/Video \
					application/media/common \
					application/media/image \
					application/adaptation/DI \

# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := no


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.


# DRA format
ifeq "${MEDIA_DRA_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_DRA_SUPPORT
endif

# FM line-in record
ifeq "${FM_LINEIN_RECORD}" "1"
LOCAL_EXPORT_FLAG += FM_LINEIN_RECORD
endif

LOCAL_EXPORT_FLAG += 

#CFLAGS += -isystem 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##

include src/make.srcs


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
