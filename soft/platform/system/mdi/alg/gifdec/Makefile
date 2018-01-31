## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := ${PLATFORM_SYSTEM}/mdi/alg/gifdec

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
					 platform/base/common \
					 ${PLATFORM_SYSTEM}/calib \
					 platform/chip/${CT_ASIC} \
                     platform/chip/hal \
                     platform/base/sx \
                     platform/chip/defs \
                     ${PLATFORM_SYSTEM}/mdi/alg/gifdec \
                     ${PLATFORM_SYSTEM}/mdi \
 

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 



# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

## ------------------------------------- ##
##  Voc project to compile               ##
## ------------------------------------- ##
# only one project at a time please.
LOCAL_VOCPROJ_NAME := vpp_gifdec

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
