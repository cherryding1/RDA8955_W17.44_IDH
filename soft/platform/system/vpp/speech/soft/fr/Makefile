## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := ${PLATFORM_SYSTEM}/vpp/speech/soft/fr

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?=    platform \
                        platform/base/common \
                        ${PLATFORM_SYSTEM}/vpp/speech/soft/efr \
                        platform/chip/hal
                         

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 
                    
## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

AUTO_XCP2H ?= yes


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
