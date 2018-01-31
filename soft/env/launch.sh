# This script should be sourced from the top of source tree.
# It will set necessary environment variables.

function project_select_from
{
    local BAR="$1"
    local ITEMS="$2"
    local DEF=$3
    local VAR=$4

    echo
    echo "$BAR"

    local MENUS
    local i=0
    local item
    for item in $ITEMS; do
        echo "   $i. $item"
        i=$(($i+1))
        MENUS=(${MENUS[@]} $item)
    done

    echo -n "which would you like? [$DEF] "
    local answer
    read answer

    local choice=$DEF
    if test -n "$answer"; then
        if (echo -n $answer | grep -q -e "^[0-9][0-9]*$"); then
            if [ $answer -lt ${#MENUS[@]} ]; then
                choice=${MENUS[$(($answer))]}
            fi
        fi
    fi

    export $VAR=$choice
}

function project_select_01
{
    local BAR="$1"
    local DEF=$2
    local VAR=$3

    echo
    echo "$BAR (0/1)"
    echo -n "which would you like? [$DEF] "
    local answer
    read answer

    local choice=$DEF
    if [ "x$answer" = "x0" ]; then
        choice=0
    elif [ "x$answer" = "x1" ]; then
        choice=1
    fi

    export $VAR=$choice
}

function project_select
{
    local target
    local TARGETS=
    for target in $(cd target; /bin/ls); do
        if test $target != "template"; then
            if test -f target/$target/target.def; then
                TARGETS="$TARGETS $target"
            fi
        fi
    done

    local SVN=0
    if svn info >& /dev/null; then SVN=1; fi
    local GIT=0
    if git rev-parse HEAD >& /dev/null; then GIT=1; fi

    project_select_from "build target:" "$TARGETS" "" CT_TARGET
    project_select_from "release/debug:" "debug release" "debug" CT_RELEASE
    project_select_from "build usrgen:" "no yes" "no" CT_USERGEN
    project_select_01 "with svn:" "$SVN" WITH_SVN
    project_select_01 "with git:" "$GIT" WITH_GIT
}

function project_show
{
    echo
    echo "=========================="
    echo "PATH=$PATH"
    echo "SOFT_WORKDIR=$SOFT_WORKDIR"
    echo "CT_TARGET=$CT_TARGET"
    echo "CT_PRODUCT=$CT_PRODUCT"
    echo "CT_RELEASE=$CT_RELEASE"
    echo "CT_USERGEN=$CT_USERGEN"
    echo "CT_USER=$CT_USER"
    echo "CT_OPT=$CT_OPT"
    echo "WITH_SVN=$WITH_SVN"
    echo "WITH_GIT=$WITH_GIT"
    echo "WITHOUT_WERROR=$WITHOUT_WERROR"
}

function launch
{
    export CT_RESGEN=no
    export CT_PRODUCT=test
    export CT_OPT=dbg_size
    export CT_USER=ADMIN
    export WITHOUT_WERROR=y

    project_select
    project_show
}

launch
unset -f launch
unset -f project_select
unset -f project_select_from
