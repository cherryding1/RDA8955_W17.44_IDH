
include ${SOFT_WORKDIR}/env/compilation/mmi_compilevars.mk
BB_EVENT_TIME ?= 1483410257
CC=gcc
#list all user files
USED_SRC := drv_flsh.c usrgen.c dsm_config.c usrgen_shell.c version.c usrgen.h memd_m.h memdp.h 

LOCAL_API_DEPENDS :=    ${PLATFORM_SERVICE}/base \
                        ${PLATFORM_SERVICE}/cfw \
                        application/coolmmi/debug \
                        application/adaptation/nvram \
                        application/coolmmi/mmi \
                        application/coolmmi/mmi/GUI \
                        application/coolmmi/mmi/PhoneBook \
                        application/coolmmi/mmi/DateTime \
                        application/coolmmi/mmi/Framework \
                        application/coolmmi/mmi/PROFILES \
                        application/coolmmi/mmi/Setting \
                        application/coolmmi/mmi/MiscFramework \
                        application/coolmmi/mmi/TouchPanel \
                        application/coolmmi/mmi/IdleScreen \
                        application/coolmmi/mmi_csdapp/mmiapi \
                        application/coolmmi/mmi_csdapp/GDI \
                        application/coolmmi/mmi_csdapp/EbookReader \
                        application/mmi_customer \
                        application/systeminc/sst \
                        application/systeminc/interface/adaptation \
                        application/systeminc/interface/ps \
                        application/systeminc/interface/config \
                        application/systeminc/interface/mmi \
                        application/systeminc/interface/media \
                        application/systeminc/interface/hwdrv \
                        application/systeminc/mmi \
                        application/systeminc/ps/l4 \
                        application/systeminc/ps/interfaces \
                        application/adaptation/custom \
                        application/adaptation/custom/app \
                        application/adaptation/custom/common \
                        application/adaptation/device \
                        platform/base/sx \
                        platform/chip/hal \
                        platform/chip/defs \


#list all local flag
LOCAL_EXPORT_FLAG += USER_DATA_BLOCK_SIZE=${USER_DATA_BLOCK_SIZE}
LOCAL_EXPORT_FLAG += BB_EVENT_TIME=${BB_EVENT_TIME}

ALL_EXPORT_FLAG := $(sort ${ALL_EXPORT_FLAG} ${LOCAL_EXPORT_FLAG})
MYCPPFLAGS := -D_FS_SIMULATOR_ -D_USERGEN -D_DSM_SIMUFLASH_MEMORY -DMMC_SIMU_NULL -D_USERGEN_SHELL
MYCPPFLAGS := ${MYCPPFLAGS} ${foreach tmpFlag, ${ALL_EXPORT_FLAG}, -D${tmpFlag}}

# ADD all the include from LOCAL_MODULE_DEPENDS 
# (if we depend on a module, we depend on its include also)
DEPENDENCY_INCLUDE_PATH := ${foreach module, ${LOCAL_API_DEPENDS}, -I${SOFT_WORKDIR}/${module}/include }

INC = -I . ${DEPENDENCY_INCLUDE_PATH}
CFLAGS = -Wall -g $(MYCPPFLAGS) $(INC) -std=gnu99 -m32
CFLAGS +=-include fpconfig.h
SOURCE := ${wildcard *.c} ${wildcard *.cc}
HEADER := ${wildcard *.h} ${wildcard *.hp}
SRC_FILTERED := ${filter-out ${USED_SRC}, ${SOURCE} ${HEADER}}
OBJS := ${patsubst %.cc,${USRGEN_BUILD_DIR}/%.o,${SOURCE}}
OBJS := ${patsubst %.c,${USRGEN_BUILD_DIR}/%.o,${OBJS}}

clean:
	rm -fr ${USRGEN_BUILD_DIR}
cleanSrc:
	rm -f ${SRC_FILTERED}
	rm -f *.txt

${USRGEN_BUILD_DIR}/usrgen : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	rm -f *.o

${USRGEN_BUILD_DIR}/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $*.c
