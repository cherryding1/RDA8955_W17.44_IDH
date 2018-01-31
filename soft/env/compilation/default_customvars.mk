#Please add the follow line in your customvars.mk

## Extra flags exported to all makefiles.
GLOBAL_EXPORT_FLAG += 

## This will define the default target for which you will compile,
## if you don't specify a CT_TARGET on the command line.
CT_TARGET ?= 5827_evb_wap_u02

## This is useful to compile your code as a Ramrun.
#CT_RAMRUN := 1

## This defines the release mode in which you compile.
CT_RELEASE ?= debug

## Define VERBOSE to get more information during the make process.
#VERBOSE = 1

## To use the ccache tool (accelerate the compilation).
#CCACHE_ENABLE := 1

AUTO_XCP2H := no
ifeq "${CT_FIRST}" "YES"
AUTO_XMD2H := yes
else
AUTO_XMD2H := no
endif

## Extract branch revision with SVN
WITH_SVN ?= 0

## Extract branch revision with GIT
WITH_GIT ?= 1

## Generate and/or update version.h
WITH_VERGEN ?= 1
CT_OPT := dbg_size
CT_USER ?= ADMIN
CT_MODEM ?= 0
WITHOUT_WERROR ?=y

ifdef CT_PRODUCT
GLOBAL_EXPORT_FLAG += CT_PRODUCT=${CT_PRODUCT}
endif

