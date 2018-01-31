## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := ${PLATFORM_SYSTEM}/svc/umss/storage/mc1

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform                           \
                     ${PLATFORM_SYSTEM}/svc                       \
                     ${PLATFORM_SYSTEM}/svc/uctls                 \
                     platform/edrv/mcd                   \
                     platform/chip/hal                  \
                     platform/chip/defs                 \
                     ${PLATFORM_SYSTEM}/svc/umss                  \
                     platform/base/sx                   \
                     ${PLATFORM_SYSTEM}/stack

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?=

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG +=

IS_TOP_LEVEL      := yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
