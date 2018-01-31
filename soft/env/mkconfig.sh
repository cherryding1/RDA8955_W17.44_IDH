#!/bin/bash

set -e

TARGET=$1
TARGET=${TARGET:=$CT_TARGET}
KCONFIG=Kconfig
CONFIG=target/$TARGET/target.config
HEADER=target/$TARGET/include/fpconfig.h
MAKEFILE=target/$TARGET/config.mk

# silent return when there are no target.config
if [ ! -f "$SOFT_WORKDIR/$CONFIG" ]; then
    exit 0
fi

cd $SOFT_WORKDIR/
case `uname` in
    Linux)
        CONF=env/linux/conf
        ;;
    CYGWIN*|MINGW*|MSYS*|Windows*)
        CONF=env/win32/conf.exe
        ;;
esac

export KCONFIG_CONFIG=$CONFIG
$CONF --olddefconfig $KCONFIG

if ! cmp config.mk $MAKEFILE 2> /dev/null; then
    echo "update $MAKEFILE"
    mv config.mk $MAKEFILE
else
    rm -f config.mk
fi

if ! cmp fpconfig.h $HEADER 2> /dev/null; then
    echo "update $HEADER"
    mv fpconfig.h $HEADER
else
    rm -f fpconfig.h
fi
