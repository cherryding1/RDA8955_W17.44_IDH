## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/aud


# include the target definition here to know which libs to link to
LOCAL_MODULE_DEPENDS := \
                        platform/edrv/aud/bt \
                        platform/edrv/aud/codec_gallite \
                        platform/edrv/aud/fm \
                        platform/edrv/aud/tv \

ifneq "${AD_MODEL}" ""
ifneq "${AD_MODEL}" "codec_gallite"
# E.g., maxim ti null
LOCAL_MODULE_DEPENDS += platform/edrv/aud/${AD_MODEL}
endif
endif

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/edrv \
					platform/edrv/pmd \
					platform/base/common \
					platform/chip \
					platform/base/sx \
					platform/chip/hal \
					platform/chip/defs \
					target \
						${PLATFORM_SYSTEM}/mdi/ \
						${PLATFORM_SYSTEM}/mdi/mmc/ \
						${PLATFORM_SERVICE}/ \
						platform/chip/hal/ \


# This is a top-level module
IS_TOP_LEVEL := yes

ifeq "${RELEASE_MODULE}" "1"
# in case of LIB_RELEASE - list all the aud modules 
LOCAL_DEPEND_LIBS := \
        platform/edrv/aud/codec_gallite \

#        platform/edrv/aud/maxim \
#        platform/edrv/aud/analog \

endif #RELEASE_MODULE

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
