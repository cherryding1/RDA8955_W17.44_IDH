#!/bin/bash
if [ $# != 2 ]; then

    echo '------Param error'
    echo 'Usage:'
    echo '      global_var_tool.sh  global_var_tool.awk   xxxx.map'
    echo '      result: create a global_var.txt'
    exit
fi

awk_map_file=$1
if [ "${awk_map_file##*.}" != "awk" ];then
    echo "Param1 must be  *.awk"
    exit
fi

awk_map_file=$2
if [ "${awk_map_file##*.}" != "map" ];then
    echo "Param2 must be *.map"
    exit
fi

var_in_order_flag=`grep 'Common symbol       size'  $@`

if [ "${var_in_order_flag}" != "" ]; then
    cat $2 |awk -v var_in_order=0 -f $1  > global_var.txt
else
    cat $2 |awk -v var_in_order=1 -f $1  > global_var.txt
fi
