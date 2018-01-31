#!/bin/bash

#---------------------------------------------------------------------------------------------------
# Revert config of target after generate t flash programmer files..
#---------------------------------------------------------------------------------------------------

#set -e

T_DIR=${SOFT_WORKDIR}/target/$1

function revert_target()
{
	rm -f $T_DIR/config.mk
	rm -f $T_DIR/include/fpconfig.h
	mv $T_DIR/config.mk_bak $T_DIR/config.mk
	mv $T_DIR/include/fpconfig.h_bak $T_DIR/include/fpconfig.h
	echo "$1 revert done"
}

revert_target
