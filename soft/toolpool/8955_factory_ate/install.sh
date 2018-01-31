#!/bin/bash

#---------------------------------------------------------------------------------------------------
# Generating all the flash programmer files..  CT_ROM_VERSION=u01
#---------------------------------------------------------------------------------------------------

set -e

function build_target_host ()
{
    local OPTS="CT_TARGET=$1 CT_RELEASE=release CT_OPT=dbg_size CT_USER=ADMIN 
        FLSHPROG_CT_ASIC=$2 FLSH_MODEL=$3 FASTPF_USE_USB=0 "
    ctmake ${OPTS} clean
    ctmake ${OPTS} lod
}

# Building
echo "Generating all the flash programmer files..."
build_target_host 8955_ramrun 8955 flsh_spi32m
echo "done"


