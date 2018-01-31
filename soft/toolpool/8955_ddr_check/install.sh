#!/bin/bash

set -e

echo "Generating 8955 DDR check ramrun..."
ctmake CT_TARGET=8955_ramrun CT_RELEASE=release CT_OPT=dbg_size CT_USER=ADMIN CT_ROM_VERSION=u02 CT_PRODUCT=u02
ctmake CT_TARGET=8955_ramrun CT_RELEASE=release CT_OPT=dbg_size CT_USER=ADMIN CT_ROM_VERSION=u01 CT_PRODUCT=u01
echo "done"


