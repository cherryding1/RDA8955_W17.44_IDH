#!/bin/sh

CLOC="${SOFT_WORKDIR}/env/utils/cloc-1.08.pl --no3 --quiet --progress-rate=0"


function countLineModule()
{
    # Offset submodules. (Remove Soft workdir part of the path,
    # and tabulate of 4 spaces per directory level (ie per /)
    moduleName=`echo $1 | sed "s@$SOFT_WORKDIR/*@@"`
    tabulation=`echo $moduleName | sed 's/[^/]//g' | sed 's/\//    /g'`
    echo -e "$tabulation""$moduleName" | sed 's/\.\///'
    $CLOC $1 | sed "s/^/$tabulation/"
    # Newline.
    echo -e 
}

function subDirectory()
{
    # Highly inefficient...
    find $1 -type d -not -path "*.svn*" -not -name "include" -not -name "src" -not -name "lib" -not -path "*packed*" -not -path "*docgen*" | while read mod;
    do
        countLineModule $mod
#        echo -e $mod
    done 
}

if [ $# -ne 0 ];
then
    subDirectory $1
    exit
fi

#echo -e "name\tlines\tcomments\t% comments\tnbfile"
# subDirectory $SOFT_WORKDIR/${PLATFORM_SYSTEM}/svc
# subDirectory $SOFT_WORKDIR/platform/chip
# subDirectory $SOFT_WORKDIR/platform/edrv
# subDirectory $SOFT_WORKDIR/platform/edrv/rfd
# subDirectory $SOFT_WORKDIR/${PLATFORM_SYSTEM}/stack
subDirectory $SOFT_WORKDIR/platform
