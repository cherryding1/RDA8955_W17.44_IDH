include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := toolpool/blfpc/goke_flash_tool

LOCAL_API_DEPENDS := platform \
                     platform/edrv/pmd/pmu_$(CT_ASIC) \
                     platform/chip/regs/$(CT_ASIC) \
                     platform/chip/bootloader

ifeq ($(findstring toolpool/blfpc/goke_flash_tool/packed,${GLOBAL_BINARY_LIBS}),)

IS_TOP_LEVEL := no
C++_SRC := $(notdir $(wildcard src/*.cpp))
LOCAL_LIB_INCR_LINK := yes

else

IS_TOP_LEVEL := yes
LOCAL_LIBS += $(LOCAL_NAME)/packed/lib/libgoke_flash_tool_$(CT_RELEASE).a

endif

include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
