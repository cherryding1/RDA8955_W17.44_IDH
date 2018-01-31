#!/bin/bash

set -e

TARGET=$1
TARGET=${TARGET:=$CT_TARGET}
KCONFIG=Kconfig

export KCONFIG_CONFIG=target/$TARGET/target.config
export KCONFIG_HEADER=target/$TARGET/include/fpconfig.h
export KCONFIG_MAKEFILE=target/$TARGET/config.mk

if [ ! -f "$SOFT_WORKDIR/$KCONFIG_CONFIG" ]; then
    echo "$SOFT_WORKDIR/$KCONFIG_CONFIG doesn't exist"
    exit 1
fi

cd $SOFT_WORKDIR/
case `uname` in
    Linux)
        env/linux/mconf $KCONFIG
        ;;
    CYGWIN*)
        cygstart env/win32/mconf.exe $KCONFIG
        START=cygstart
        ;;
    MINGW*|MSYS*)
        start env/win32/mconf.exe $KCONFIG
        ;;
    Windows*)
        env/win32/mconf.exe $KCONFIG
        ;;
esac
