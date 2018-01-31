## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc

# Compile the sub-modules, except when the "${PLATFORM_SYSTEM}/svc" must be used as a library.
ifeq ($(findstring ${PLATFORM_SYSTEM}/svc/packed,${GLOBAL_BINARY_LIBS}),)

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
						${PLATFORM_SYSTEM}/svc/fmg \
						${PLATFORM_SYSTEM}/svc/vois \
						
ifeq "${CHIP_HAS_NES}" "1"						
						${PLATFORM_SYSTEM}/svc/infones 
endif

# NOTE: ${PLATFORM_SYSTEM}/svc/aps is now only used in platform_test/stack/proto

ifeq "${CHIP_HAS_USB}" "1"

LOCAL_MODULE_DEPENDS +=	${PLATFORM_SYSTEM}/svc/uctls \
						${PLATFORM_SYSTEM}/svc/fstraces

ifeq "${_UTRACE_}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/utraces
endif
ifeq "${_UMSS_}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/umss
endif
ifeq "${_UCDCACM_}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/uat
endif
ifeq "${_UVIDEO_}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/uvideos
endif
ifeq "${_URNDIS_}" "y"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/urndis
endif
endif

ifeq "${CHIP_HAS_AP}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/syscmds
endif

# TODO: Do we really need to make this inclusion depend on the CSW_EXTENDED_API flag?
ifeq "${CSW_EXTENDED_API_CAMERA}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/cams
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/imgs
endif

ifeq "${CSW_EXTENDED_API_AUDIO_VIDEO}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/avps
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/mps
endif

ifeq "${CSW_EXTENDED_API_IMAGE}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/imgs
endif

ifeq "${CSW_EXTENDED_API_MRS}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/mrs
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/avrs
else
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/ars
endif

else # GLOBAL_BINARY_LIBS

LOCAL_BINARY_LIBS    += ${PLATFORM_SYSTEM}/svc/packed/svc_${CT_ASIC}
LOCAL_MODULE_DEPENDS := ${PLATFORM_SYSTEM}/svc/packed/svc_${CT_ASIC}

endif # GLOBAL_BINARY_LIBS

#ifeq "${CSW_EXTENDED_API_DTMF_DETECT}" "1"
LOCAL_MODULE_DEPENDS += ${PLATFORM_SYSTEM}/svc/voice_express
#endif
# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
