## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/umss

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform                           \
                     ${PLATFORM_SERVICE}                \
                     ${PLATFORM_SYSTEM}/svc                       \
                     ${PLATFORM_SYSTEM}/svc/uctls                 \
                     ${PLATFORM_SYSTEM}/svc/uwifi \
                     ${PLATFORM_SYSTEM}/svc/uat \
                     ${PLATFORM_SYSTEM}/svc/urndis \
		     ${PLATFORM_SYSTEM}/svc/uvideos                 \
                     platform/chip/hal                  \
                     platform/chip/defs                 \
                     platform/base/sx

# list all the modules that need to be compiled prior to using this module
ifeq "$(_UMSS_DUAL_MC_)" "y"
LOCAL_MODULE_DEPENDS :=                                 \
                     ${PLATFORM_SYSTEM}/svc/umss/storage/ram      \
                     ${PLATFORM_SYSTEM}/svc/umss/transport/boscsi \
		     						 ${PLATFORM_SYSTEM}/svc/umss/storage/mc0				\
		     						 ${PLATFORM_SYSTEM}/svc/umss/storage/mc1				\
		     						 ${PLATFORM_SYSTEM}/svc/umss/storage/flash
else
LOCAL_MODULE_DEPENDS :=                                 \
                     ${PLATFORM_SYSTEM}/svc/umss/storage/ram      \
                     ${PLATFORM_SYSTEM}/svc/umss/transport/boscsi \
		     						 ${PLATFORM_SYSTEM}/svc/umss/storage/mc				\
		     						 ${PLATFORM_SYSTEM}/svc/umss/storage/flash
endif	
					
# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG +=

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
