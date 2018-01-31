## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/service/net

# list all modules APIs that are necessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/chip/hal \
					platform/chip/hal/${CT_ASIC} \
					platform/chip/defs \
					platform/system/stack \
					${API_PLATFORM_DEPENDS} \
					platform/service/net/http \
					platform/service/net/http/http \
					platform/service/net/http/util \
					platform/service/net/http/net \
					at \
					at/ate \
					at/utility \

LOCAL_ADD_INCLUDE := platform/service/net/lwip/src/apps/altcp_tls
LOCAL_ADD_INCLUDE += platform/service/net/mbedtls/src/mbedtls/include



# Compile the sub-modules, except when the "service" must be used as a library.
# list all the modules that need to be compiled prior to using this module


ifeq "${LWIP_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += platform/service/net/edp
LOCAL_MODULE_DEPENDS += platform/service/net/lwip
LOCAL_MODULE_DEPENDS += platform/service/net/app_http

ifeq "${AT_HTTP_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += platform/service/net/http/net
LOCAL_MODULE_DEPENDS += platform/service/net/http/util
LOCAL_MODULE_DEPENDS += platform/service/net/http/http
else
ifeq "${LWIP_TEST}-${LWIP_HTTP_TEST}" "y-y"
LOCAL_MODULE_DEPENDS += platform/service/net/http/net
LOCAL_MODULE_DEPENDS += platform/service/net/http/util
LOCAL_MODULE_DEPENDS += platform/service/net/http/http
endif
endif

ifeq "${AT_LWM2M_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += platform/service/net/liblwm2m
else
ifeq "${LWIP_TEST}-${LWIP_LIBWM2M_TEST}" "y-y"
LOCAL_MODULE_DEPENDS += platform/service/net/liblwm2m
endif
endif

ifeq "${AT_COAP_SUPPORT}" "y"
LOCAL_MODULE_DEPENDS += platform/service/net/libcoap
else
ifeq "${LWIP_TEST}-${LWIP_COAP_TEST}" "y-y"
LOCAL_MODULE_DEPENDS += platform/service/net/libcoap
endif
endif

LOCAL_MODULE_DEPENDS += platform/service/net/tinydtls
LOCAL_MODULE_DEPENDS += platform/service/net/mbedtls
LOCAL_MODULE_DEPENDS += platform/service/net/iotxsdk
LOCAL_MODULE_DEPENDS += platform/service/net/mupnp
LOCAL_MODULE_DEPENDS += platform/service/net/xmlparser
endif

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes


# This is a top-level module
IS_TOP_LEVEL := yes
LOCAL_EXPORT_FLAG += \
	HAVE_CFW_CONFIG \
	MF32_BASE_REG_DISABLE \
	MF32_NO_RFM_RH \
	MF32_NO_RFM \
	MF32_CFW_MMI_TWO_TASKS \
	CPP_KEYWORD_SUPPORT \
	OS_KERNEL=2 \
	RDA_HTTP \
	RDA_HTTP_LWIP \
	CG_USE_OPENSSL
	
ifneq "${AMR_SUPPORT}" "NO"	
LOCAL_EXPORT_FLAG += CFW_AMR_SUPPORT
endif
#LOCAL_EXPORT_FLAG += CLOG_DEBUG
	
ifeq "${DUAL_SIM_SUPPORT}" "1"	
LOCAL_EXPORT_FLAG += CFW_DUAL_SIM STK_DUAL_SIM
endif

ifeq "${GPRS_SUPPORT}" "y"
ifneq "${USE_KCONFIG}" "y"
LOCAL_EXPORT_FLAG += CFW_GPRS_SUPPORT
endif
endif

LOCAL_EXPORT_FLAG += MBEDTLS
# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

ifeq "${TINYDTLS_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += WITH_TINYDTLS
else ifeq "${MBEDDTLS_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += WITH_MBEDDTLS
endif

ifeq "${LWIP_SUPPORT}" "y"
#C_SRC := sys_arch.c netapi.c getopt.c cfw_sockets_api.c lwip_app_task.c mqtt_client_test.c http_test_api.c coap_client_test.c lwm2m_client_test.c
C_SRC := ${notdir ${wildcard src/*.c}}
endif

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
