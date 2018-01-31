#!/bin/bash

#---------------------------------------------------------------------------------------------------
# Modify config of target before generate flash programmer files..
#---------------------------------------------------------------------------------------------------

#set -e

T_DIR=${SOFT_WORKDIR}/target/$1
MAKEDIR=$SOFT_WORKDIR/toolpool/t_flash_programmer 

function modify_configmk()
{
	for flag in SPI_REG_DEBUG
	do
        find $T_DIR -name "config.mk" | xargs sed -i "/$flag/d"
	done
}

function modify_fpconfigh()
{
	for flag in SXS_NO_PRINTF SXS_NO_DUMP HAL_NO_PRINTF EDRV_NO_TRACE
	do
        find $T_DIR/include/ -name "fpconfig.h" | xargs sed -i "/$flag/d"
	done
}

function modify_for_build()
{
	cp $T_DIR/config.mk $T_DIR/config.mk_bak
	cp $T_DIR/include/fpconfig.h $T_DIR/include/fpconfig.h_bak
	echo "$1 backup done"

	modify_configmk
	modify_fpconfigh
	cd $MAKEDIR
	echo "$1 fileter out done"
}

modify_for_build
