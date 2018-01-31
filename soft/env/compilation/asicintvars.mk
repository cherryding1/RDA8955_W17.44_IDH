## Libs that we generally use only as binary
## For example, with the following line, you can avoid compiling the stack.
#GLOBAL_BINARY_LIBS += ${PLATFORM_SYSTEM}/stack
#GLOBAL_BINARY_LIBS += platform/chip/packed/${CT_ASIC}

## Extra flags exported to all makefiles.
#GLOBAL_EXPORT_FLAG += FPGA_SLOW

## This will define the default target for which you will compile,
## if you don't specify a CT_TARGET on the command line.
#CT_TARGET = granite_fpga
#CT_TARGET = granite_eng
#CT_TARGET = granite_eng_digrf
#CT_TARGET = granite_eng_norom
#CT_TARGET = greenstone_fpga
#CT_TARGET = greenstone_fpga_softrom
CT_TARGET = greenstone_eng_ct1010
#CT_TARGET = nephrite_fpga
#CT_TARGET = nephrite_fpga_softrom

## This is useful to compile your code as a Ramrun.
#CT_RAMRUN := 1

## We can force to use a Romuator here (normally this is defined by the target).
#CT_ROMULATOR = yes

## This defines the release mode in which you compile.
#CT_RELEASE = release
#CT_RELEASE = debug
#CT_RELEASE = debug_romu
CT_RELEASE = cool_profile

## Defines the type of MMI to compile for.
CT_MMI = f658

# Uses the full MDI (0) or the MDI stub plus the services (1).
#CSW_EXTENDED_API := 1

## Define VERBOSE to get more information during the make process.
#VERBOSE = 1

## To use the ccache tool (accelerate the compilation).
CCACHE_ENABLE := 1

WITH_FULL_PATH := 1
