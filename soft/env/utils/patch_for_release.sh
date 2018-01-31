#!/bin/bash
#  
# Usage:
# ./run_script.sh old_commit_ID new_commit_ID Product_name
# Before running the script, you must commit what you have modified.
# After the script is done, you should checkout back to the branch you use.

TODAY=$(date +%Y%m%d)
VERSION=$(date +%y.%W.%w)
# SET the temp path:
NOTE=$SOFT_WORKDIR/patchdir/diff_$TODAY
WAPMMS_DIR=application/coolmmi/mmi/wapmms/jcore
STACK_DIR=platform/system/stack

function create_diff_dirtory
{
	mkdir patchdir
	# create branch $1 and $2
	git checkout -b old origin/master
	git reset --hard $1
	git checkout -b new origin/master
	git reset --hard $2
	
    # find out the diff file name:
    git diff old new --name-only > $NOTE.txt
    # del last line:
    #sed -i '$d' $NOTE.txt
	echo "Diff file create"
}

function check_source_code
{
	find patchdir/ -name "*.txt" | xargs sed -i "\%$WAPMMS_DIR%{\%src%d}"
	find patchdir/ -name "*.txt" | xargs sed -i "\%$STACK_DIR%{\%src%d}"
	echo "Check Done"
}

function export_diff_files
{
    local PATCH_PACKAGE=$SOFT_WORKDIR/patchdir/diff_$1_$2_$TODAY
    local NEW=$PATCH_PACKAGE/new_$3_W$VERSION
    local OLD=$PATCH_PACKAGE/old_$3_W$VERSION
    # Make the patch directory:
    mkdir $PATCH_PACKAGE
    mkdir $NEW
    mkdir $OLD
    git checkout new
    /usr/bin/rsync -rltDv --files-from=$NOTE.txt $SOFT_WORKDIR $NEW
    git checkout old
    /usr/bin/rsync -rltDv --files-from=$NOTE.txt $SOFT_WORKDIR $OLD
    echo "Export Done"
    ### tar as a tar.gz file.
    #tar -czPf $PATCH_PACKAGE/code_$1_$2_$TODAY.tar.gz  $PATCH_PACKAGE
    #echo "tar ok!"; 
}

if [ $# -eq 3  ];then
create_diff_dirtory $1 $2
check_source_code
export_diff_files $1 $2 $3

else
echo "Usage:
    ./run_script.sh old_commit_ID new_commit_ID Product_name
Note:
    Before running the script, you must commit what you have modified.
    After the script is done, you should checkout back to the branch you use.
    "
fi
