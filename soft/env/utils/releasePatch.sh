#!/bin/sh

if [ $# != 3 ]; then
    echo "Usage: $0 <olddir> <newdir> <patchdir>"
    exit 1
fi

OLDDIR=$1
NEWDIR=$2
PATCHDIR=$3

TODAY=`date +%Y%m%d`
VERSION=`date +%y.%W.%w`
WAPMMS_DIR=application/coolmmi/mmi/wapmms/jcore
STACK_DIR=platform/system/stack
DIFF_LIST=$PATCHDIR/diff_${TODAY}.txt
PATCH_DIR=$PATCHDIR/diff_${OLDDIR}_And_${NEWDIR}_${TODAY}

mkdir -p $PATCH_DIR $PATCH_DIR/$OLDDIR $PATCH_DIR/$NEWDIR

LANG= diff -rq $OLDDIR $NEWDIR > $DIFF_LIST
#cp -f $DIFF_LIST diff_files_temp.txt
sed -e "s:^Files ${OLDDIR}/::g" -e "s: and ${NEWDIR}.*$::g" -i $DIFF_LIST
sed -e "s:^Only in ${OLDDIR}/::g" -e "s:^Only in ${NEWDIR}/::g" -e "s;: ;/;g"  -i $DIFF_LIST
sed -i "/p_version.h\>/d" $DIFF_LIST

rsync -rltDv --files-from=$DIFF_LIST $OLDDIR $PATCH_DIR/$OLDDIR
rsync -rltDv --files-from=$DIFF_LIST $NEWDIR $PATCH_DIR/$NEWDIR
