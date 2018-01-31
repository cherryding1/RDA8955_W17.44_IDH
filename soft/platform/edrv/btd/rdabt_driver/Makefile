## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/mmi_compilevars.mk
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/edrv/btd/rdabt_driver

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS :=

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
                ${MMI_CONFIG} \
                application/systeminc \
                application/systeminc/mmi \
                application/systeminc/interface \
                application/systeminc/interface/media \
                application/systeminc/interface/l1audio \
                application/adaptation/communication \
                application/adaptation/custom/app \
                application/adaptation/custom \
                application/adaptation/custom/common \
                application/adaptation/device \
                application/adaptation/debug \
                application/adaptation \
                application/systeminc/interface/adaptation \
                application/systeminc/interface/config \
                application/systeminc/interface/hwdrv \
                application/systeminc/interface/l1audio \
                application/systeminc/interface/l1interface \
                application/systeminc/interface/media \
                application/systeminc/interface/mmi \
                application/systeminc/interface/os \
                application/systeminc/interface/ps \
                application/coolmmi/mmi \
                application/coolmmi/mmi/Alarm \
                application/coolmmi/mmi/AsyncEvents \
                application/coolmmi/mmi/Audio \
                application/coolmmi/mmi/CallManagement \
                application/coolmmi/mmi/CommonScreens \
                application/coolmmi/mmi/DateTime \
                application/coolmmi/mmi/DebugLevels \
                application/coolmmi/mmi/FileSystem \
                application/coolmmi/mmi/Framework \
                application/coolmmi/mmi/Framework/MemManager \
                application/coolmmi/mmi/GUI \
                application/coolmmi/mmi/IdleScreen \
                application/coolmmi/mmi/MiscFramework \
                application/coolmmi/mmi/Organizer \
                application/coolmmi/mmi/PROFILES \
                application/coolmmi/mmi/PhoneBook \
                application/coolmmi/mmi/Resource \
                application/coolmmi/mmi/Setting \
                application/coolmmi/mmi/Calls \
                application/coolmmi/mmi/MainMenu \
                application/coolmmi/mmi/ToDoList \
                application/coolmmi/mmi/dualsim \
                application/coolmmi/mmi_csdapp/EngineerMode \
                application/coolmmi/mmi_csdapp/GDI \
                application/coolmmi/mmi_csdapp/mmiapi \
                application/coolmmi/mmi_csdapp/FileMgr \
                application/coolmmi/mmi_csdapp/MDI \
                application/systeminc/ps/interfaces \
                application/systeminc/ps/l4 \
                application/systeminc/sst \
                platform \
                ${PLATFORM_SYSTEM}/mdi \
                platform/base/std \
                platform/base/sx \
                platform/chip/defs \
                platform/chip/hal \
                platform/chip/hal \
                ${PLATFORM_SERVICE} \
                platform/edrv/pmd \
                application/coolmmi/debug \
                application/coolmmi/media/common \
                application/coolmmi/mmi_csdapp/EbookReader \
                ${MMI_CUSTOMER} \
                application/coolmmi/mmi/TouchPanel \
                platform/edrv \
                application/coolmmi/mmi/Bluetooth/rda5868/BluetoothMMI/BtComon \
                application/coolmmi/mmi/Bluetooth/rda5868/BluetoothMMI/BtApp \
                application/coolmmi/mmi/Bluetooth/rda5868/BluetoothMMI/xmlp \
                application/coolmmi/mmi/Bluetooth/rda5868 \
                application/mgapollo/apps/bluetooth/tmply \
                platform/edrv/btd/rdaCommon/BluetoothAdapter \
                platform/edrv/btd/rdabt_stack \
                platform/edrv/btd/rdabt_adapter \
                platform/edrv/btd/rdabt_driver	\
                target \


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG += 
WITHOUT_WERROR := y

## ------------------------------------------------------------------------- ##
##                           Source files                                      ##
## ------------------------------------------------------------------------- ##
S_SRC := ${notdir ${wildcard src/*.S}} # uncomment to take all .S files
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


