## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/atvd/rda5888

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS :=    \
        target \
        platform \
        ${PLATFORM_SYSTEM}/stack \
        platform/base/common \
        platform/base/sx \
        platform/chip/hal \
        platform/chip/defs \
        platform/edrv/atvd \
        platform/edrv/pmd \
        platform/edrv \
        platform/edrv/i2cgpio \
            

#C_SRC := drv_analogtv.c
#S_SRC :=
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files
## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


