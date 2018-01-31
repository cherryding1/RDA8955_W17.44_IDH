#!/bin/sh


function countLineModule()
{
    file=`find $1 -name "*.[ch]" 2> /dev/null`
    nbFile=`find $1 -name "*.[ch]" | wc -l 2> /dev/null`
    # C comment
    # TODO
    # cline=`(grep -n "\/\*" $file; grep -n  "\*\/" $file) | sort -n | cut -d ':' -f 2 | tr "\n" "\ "`
    # acc=0
    # i=0
    # nbLines=${#cline[*]}
    # while [ i < nbLines ];
    # do
    #     echo aa
    # done

    # C++ comment
    cppComment=`grep -n '\/\/' $file | wc -l`
    # Total lines
    lines=`wc -l $file | tail -n 1 | sed 's| *\([^ ]*\) .*|\1|'`

    if [ $cppComment -le $((25*$nbFile)) ];
    then
        echo -e $1"\t"$lines"\t"$cppComment"\t""0%""\t"$nbFile
    else
        echo -e $1"\t"$lines"\t"$cppComment"\t"$[(cppComment-25*nbFile)*100/(lines-25*nbFile)]"%""\t"$nbFile
    fi

}

function subDirectory()
{
    ls -d $1/*/src -1 | sed  "s|\(.*\)\/src|\1|" | while read mod;
    do
        countLineModule $mod
    done 
}

if [ $# -ne 0 ];
then
    subDirectory $1
    exit
fi

# echo -e "name\tlines\tcomments\t% comments\tnbfile"
subDirectory $SOFT_WORKDIR/${PLATFORM_SYSTEM}/svc
subDirectory $SOFT_WORKDIR/platform/chip
subDirectory $SOFT_WORKDIR/platform/edrv
subDirectory $SOFT_WORKDIR/platform/edrv/rfd
subDirectory $SOFT_WORKDIR/${PLATFORM_SYSTEM}/stack
subDirectory $SOFT_WORKDIR/platform