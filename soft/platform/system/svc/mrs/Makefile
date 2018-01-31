## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/mrs

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
                     platform/base/sx       \
                     platform               \
                     platform/base/common   \
                     platform/chip/defs     \
                     platform/chip/hal      \
                     ${PLATFORM_SERVICE}           \
                     platform/edrv/aud      \
                     platform/edrv/lcdd     \
                     ${PLATFORM_SYSTEM}/stack         \
                     ${PLATFORM_SYSTEM}/svc       	\
                     ${PLATFORM_SYSTEM}/svc/ars       \
                     ${PLATFORM_SYSTEM}/svc/avrs      \
                     ${PLATFORM_SYSTEM}/svc/imgs      \
                     target                 \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

AUTO_XCP2H ?= yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += MRS_USES_AVRS


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

