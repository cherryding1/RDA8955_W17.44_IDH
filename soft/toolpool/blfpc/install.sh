#!/bin/bash

#---------------------------------------------------------------------------------------------------
# Generating all the flash programmer files..
#---------------------------------------------------------------------------------------------------

set -e

function build_target_usb ()
{
    local OPTS="CT_TARGET=$1 CT_RELEASE=release CT_OPT=dbg_size
        FLSHPROG_CT_ASIC=$2 FLSH_MODEL=$3 FASTPF_USE_USB=1"
    ctmake ${OPTS} clean
    ctmake ${OPTS} lod
    ctmake ${OPTS} flshprog_install_ff
}

function build_target_host ()
{
    local OPTS="CT_TARGET=$1 CT_RELEASE=release CT_OPT=dbg_size
        FLSHPROG_CT_ASIC=$2 FLSH_MODEL=$3 FASTPF_USE_USB=0"
    ctmake ${OPTS} clean
    ctmake ${OPTS} lod
    ctmake ${OPTS} flshprog_install_ff
}

# Building
echo "Generating all the flash programmer files..."
if [ -d $SOFT_WORKDIR/target/8955_ramrun ]; then
    build_target_host 8955_ramrun 8955 flsh_spi32m
    build_target_usb 8955_ramrun 8955 flsh_spi32m
    GOKE_FLASH_TOOL_SUPPORT=y build_target_host 8955_ramrun 8955g flsh_spi32m
    GOKE_FLASH_TOOL_SUPPORT=y build_target_usb 8955_ramrun 8955g flsh_spi32m
fi
if [ -d $SOFT_WORKDIR/target/8909_ramrun ]; then
    build_target_host 8909_ramrun 8909 flsh_spi32m
    build_target_usb 8909_ramrun 8909 flsh_spi32m
fi
echo "Build done"
