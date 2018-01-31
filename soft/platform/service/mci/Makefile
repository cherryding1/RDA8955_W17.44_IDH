## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SERVICE}/mci


# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
                     application/coolmmi \
                     application/coolmmi/media/common \
                     application/coolmmi/mmi_csdapp/MDI \
                     ${PLATFORM_SYSTEM}/mdi \
                     platform/base/sx \
                     platform/chip/defs \
                     application/coolmmi/debug \
                     platform/chip/hal \
                     ${PLATFORM_SERVICE} \
                     platform/edrv/lcdd \
                     ${PLATFORM_SYSTEM}/svc/cams \
                     ${PLATFORM_SYSTEM}/svc/imgs \
                     ${PLATFORM_SYSTEM}/svc/mps \
                     ${PLATFORM_SYSTEM}/svc/mrs \
                     ${PLATFORM_SYSTEM}/svc/uctls \



# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=  

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
