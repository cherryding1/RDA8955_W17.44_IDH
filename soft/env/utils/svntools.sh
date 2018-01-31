#!/bin/bash

function up2date()
{
    if [ "$#" != 1 ];then
        echo "need date!!! 2010-xx-xx"
        return
    fi

    branchs=`find ${SOFT_WORKDIR} -type d -name .svn |sed -e '/soft\/\.svn/d' -e '/platform\/.svn/d'|sed 's/\.svn//'`
    for br in $branchs
    do
        echo "=====update $br to $1===="
        cd $br;
        svn update --ignore-externals -r "{$1}"
        cd -;
    done
}

function list_commits()
{
    if [ "$#" != 2 ];then
        echo "list commits!!!between xxxx-xx-xx:xxxx-xx-xx"
        return
    fi

    echo "======between $1:$2 ======"
    branchs=`find ${SOFT_WORKDIR} -type d -name .svn |sed -e '/soft\/\.svn/d' -e '/platform\/.svn/d'|sed 's/\.svn//'`
    for br in $branchs
    do
        echo "======= $br ======="
        cd $br;
        svn log -v  -r "{$1}:{$2}"
        cd -;
    done
    
}
