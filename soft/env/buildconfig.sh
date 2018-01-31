#!/bin/bash

# This is the script to be integrated with buildbot
# * Master will call "$0 --linux-patch-list" and etc to get the target list to be built
# * Then, master will trigger indivisual target build on workers. And indivisual target
#   build can be performed in parellel. Target build is performed by "$0 -b name"
#
# Environment from master
# * bb_buildnumber
# * bb_revision
# * bb_branch
# * bb_product
# * bb_time
# * bb_res_host
# * bb_res_path

set -e

export bb_time=${bb_time:=$(date +%s)}
export bb_product=${bb_product:=test}
export bb_res_host=${bb_res_host:=localhost}
export bb_res_path=${bb_res_path:=$HOME/tmp/builds}

export SOFT_WORKDIR=$(readlink -f `pwd`)
export BUILD_TIME=$(date -d "@${bb_time}" "+%Y-%m-%d_%H:%M:%S")
export BUILD_DATE=$(date -d "@${bb_time}" "+%Y%m%d")
export BUILD_VERSION=$(git rev-parse --short=8 HEAD)
export bb_revision=${bb_revision:=$BUILD_VERSION}
export CT_PRODUCT=$bb_product
export PATH=/opt/CSDTK/bin:/opt/mips-rda-elf-20170629p1/bin:/opt/mingw32_3.4.4/usr/bin:$PATH

STACK_DIR=platform/system/stack # stack path in source tree
WAPMMS_DIR=application/coolmmi/mmi/wapmms/jcore # jcore path in source tree
FOTA_DIR=platform/service/fota/fota_core # fota lib path in source tree
GOKE_FLASH_TOOL_DIR=toolpool/blfpc/goke_flash_tool # goke flash tool path in source tree

cleanup() {
    if [ -n "$bb_revision" ]; then
        git clean -fdx
        git reset --hard $bb_revision
    else
        rm -rf hex build
    fi
}

upload_file() {
    local LOCAL_FILE=$1
    local REMOTE_PATH=$2
    local REMOTE_FILE=$3

    echo "Upload file to $2/$3 ..."
    chmod go+r $LOCAL_FILE
    ssh $bb_res_host mkdir -p $bb_res_path/$REMOTE_PATH
    scp $LOCAL_FILE ${bb_res_host}:/$bb_res_path/$REMOTE_PATH/$REMOTE_FILE
}

upload_lod() {
    echo "Upload lod ..."
    local HEX=${CT_TARGET}_${CT_PRODUCT}_${CT_RELEASE}
    cd hex/$HEX
    7z a -tzip ../../${HEX}.zip
    cd ../..
    upload_file ${HEX}.zip . ${HEX}.zip
}

build_modem() {
    export CT_TARGET=$1
    export CT_RELEASE=$2

    cleanup
    source $SOFT_WORKDIR/env/set_env.sh
    ctmake CT_USER=ADMIN CT_RESGEN=no CT_USERGEN=no
    upload_lod
}

build_modemdb() {
    export CT_TARGET=$1
    export CT_RELEASE=$2

    cleanup
    source $SOFT_WORKDIR/env/set_env.sh
    ctmake CT_USER=ADMIN CT_RESGEN=no CT_USERGEN=no
    cd toolpool/blfota
    ctmake CT_USER=ADMIN CT_RESGEN=no CT_USERGEN=no
    cd ../..

    local HEX=${CT_TARGET}_${CT_PRODUCT}_${CT_RELEASE}
    env/utils/dual_boot_merge.py \
        --bl hex/$HEX/blfota_${HEX}_flash.lod \
        --lod hex/$HEX/${HEX}_flash.lod \
        --output hex/$HEX/merge_${HEX}_flash.lod
    upload_lod
}

build_phone() {
    export CT_TARGET=$1
    export CT_RELEASE=$2

    cleanup
    source $SOFT_WORKDIR/env/set_env.sh
    ctmake CT_USER=ADMIN CT_RESGEN=yes CT_USERGEN=yes
    upload_lod
}

build_flash_programmer() {
    cleanup
    source $SOFT_WORKDIR/env/set_env.sh
    cd toolpool/flash_programmer
    ./install.sh
    cd ../../toolpool/plugins/fastpf/flash_programmers/
    7z a -tzip ../../../../ramrun.zip
    cd ../../../..
    upload_file ramrun.zip . ramrun.zip

    cleanup
    cd toolpool/blfpc
    ./install.sh
    cd ../../toolpool/plugins/fastpf/flash_programmers/
    7z a -tzip ../../../../blramrun_${CT_PRODUCT}.zip
    cd ../../../..
    upload_file blramrun_${CT_PRODUCT}.zip . blramrun_${CT_PRODUCT}.zip
}

build_stack_lib() {
    export CT_TARGET=$1
    local ASIC=$2
    local TYPE=$3

    for release in debug release; do
        export CT_RELEASE=$release
        cleanup
        source $SOFT_WORKDIR/env/set_env.sh
        pushd $STACK_DIR
        ctmake CT_USER=ADMIN
        popd

        upload_file build/$CT_TARGET/_default_/$STACK_DIR/lib/libstack_$CT_RELEASE.a \
            $STACK_DIR/packed/$ASIC/$TYPE/lib lib${TYPE}_${CT_RELEASE}.a
    done
}

build_wap_lib() {
    export CT_TARGET=$1
    local TYPE=$2

    for release in debug release; do
        export CT_RELEASE=$release
        cleanup
        pushd $WAPMMS_DIR
        ctmake CT_USER=ADMIN
        popd

        upload_file build/$CT_TARGET/_default_/$WAPMMS_DIR/lib/libjcore_$CT_RELEASE.a \
            $WAPMMS_DIR/packed/lib lib${TYPE}_${CT_RELEASE}.a
    done
}

build_fota_lib() {
    export CT_TARGET=$1

    for release in debug release; do
        export CT_RELEASE=$release
        cleanup
        pushd $FOTA_DIR
        ctmake CT_USER=ADMIN
        popd

        upload_file build/$CT_TARGET/_default_/$FOTA_DIR/lib/libfota_core_${CT_RELEASE}.a \
            $FOTA_DIR/packed/lib libfota_core_${CT_RELEASE}.a
    done
}

build_goke_flash_tool_lib() {
    export CT_TARGET=$1

    for release in debug release; do
        export CT_RELEASE=$release
        cleanup
        pushd $GOKE_FLASH_TOOL_DIR
        ctmake CT_USER=ADMIN
        popd

        upload_file build/${CT_TARGET}/_default_/${GOKE_FLASH_TOOL_DIR}/lib/libgoke_flash_tool_${CT_RELEASE}.a \
            ${GOKE_FLASH_TOOL_DIR}/packed/lib libgoke_flash_tool_${CT_RELEASE}.a
    done
}

run_vergen() {
    local mod_dir
    local mod_name
    local ver_file
    local vergen_target=5827_evb_wap # any existed target
    source $SOFT_WORKDIR/env/set_env.sh
    for m in $(find . -name Makefile); do
        if grep ^AUTO_GEN_REVISION_HEADER $m | grep yes 2> /dev/null; then
            local mod_dir=$(dirname $m)
            pushd $mod_dir
            ctmake CT_TARGET=$vergen_target CT_RELEASE=release CT_USER=FAE updateversion || true
            popd
        fi
    done
}

verify_build() {
    export CT_TARGET=$1
    export CT_RELEASE=$2
    local IDH_NAME=$3
    local BUILD_OPTIONS="$4"
    local VER_DIR=${CT_TARGET}_${CT_RELEASE}

    rm -rf $VER_DIR; mkdir $VER_DIR
    cd $VER_DIR
    tar xfj ../$IDH_NAME
    cd soft

    export SOFT_WORKDIR=$(readlink -f .)
    source $SOFT_WORKDIR/env/set_env.sh
    ctmake $BUILD_OPTIONS

    local HEX=${CT_TARGET}_${CT_PRODUCT}_${CT_RELEASE}
    scp ${bb_res_host}:${bb_res_path}/${HEX}.zip .
    7z x ${HEX}.zip

    cmp hex/$HEX/${HEX}_flash.lod ${HEX}_flash.lod
    echo "${HEX}_flash.lod matched"

    cd ../..
    export SOFT_WORKDIR=$(readlink -f .)
}

verify_builddb() {
    export CT_TARGET=$1
    export CT_RELEASE=$2
    local IDH_NAME=$3
    local BUILD_OPTIONS="$4"
    local VER_DIR=${CT_TARGET}_${CT_RELEASE}

    rm -rf $VER_DIR; mkdir $VER_DIR
    cd $VER_DIR
    tar xfj ../$IDH_NAME
    cd soft

    export SOFT_WORKDIR=$(readlink -f .)
    source $SOFT_WORKDIR/env/set_env.sh
    ctmake $BUILD_OPTIONS
    cd toolpool/blfota
    ctmake $BUILD_OPTIONS
    cd ../..

    local HEX=${CT_TARGET}_${CT_PRODUCT}_${CT_RELEASE}
    env/utils/dual_boot_merge.py \
        --bl hex/$HEX/blfota_${HEX}_flash.lod \
        --lod hex/$HEX/${HEX}_flash.lod \
        --output hex/$HEX/merge_${HEX}_flash.lod

    local HEX=${CT_TARGET}_${CT_PRODUCT}_${CT_RELEASE}
    scp ${bb_res_host}:${bb_res_path}/${HEX}.zip .
    7z x ${HEX}.zip

    cmp hex/$HEX/merge_${HEX}_flash.lod merge_${HEX}_flash.lod
    echo "merge_${HEX}_flash.lod matched"

    cd ../..
    export SOFT_WORKDIR=$(readlink -f .)
}

verify_flash_programmer() {
    local IDH_NAME=$1
    local VER_DIR=blramrun

    rm -rf $VER_DIR; mkdir $VER_DIR
    cd $VER_DIR
    tar xfj ../$IDH_NAME
    cd soft

    export SOFT_WORKDIR=$(readlink -f .)
    cd toolpool/blfpc
    ./install.sh
    cd ../..

    scp ${bb_res_host}:${bb_res_path}/blramrun_${CT_PRODUCT}.zip .
    7z x blramrun_${CT_PRODUCT}.zip

    for f in host_8955g_flsh_spi32m_ramrun.lod usb_8955g_flsh_spi32m_ramrun.lod; do
        cmp toolpool/plugins/fastpf/flash_programmers/$f $f
        echo "$f matched"
    done
    cd ../..
    export SOFT_WORKDIR=$(readlink -f .)
}

verify_phone() {
    verify_build $1 $2 $3 "CT_USER=FAE CT_RESGEN=yes CT_USERGEN=yes"
}

verify_modem() {
    verify_build $1 $2 $3 "CT_USER=FAE CT_RESGEN=no CT_USERGEN=no"
}

verify_modemdb() {
    verify_builddb $1 $2 $3 "CT_USER=FAE CT_RESGEN=no CT_USERGEN=no"
}

build_package() {
    build_package_modem
    build_package_phone
}

package_remove_files() {
    ## remove not released sources
    rm -rf $(find $STACK_DIR -name src)
    rm -rf $(find $FOTA_DIR -name src)
    rm -rf $(find $GOKE_FLASH_TOOL_DIR -name src)
    rm -rf $(find . -name internal_doc)
    rm -rf $(find . -name internal_src)

    ## remove 8909
    rm -rf platform/system/nbiot \
        platform/chip/patch/8909 \
        platform/chip/rom/8909 \
        platform/chip/boot/8909 \
        platform/chip/defs/8909* \
        platform/chip/regs/8909 \
        platform/chip/hal/8909 \
        platform/chip/hal/src/8909 \
        platform/chip/bootloader/8909 \
        platform/chip/nbcpu \
        platform/edrv/pmd/pmu_8909 \
        platform/edrv/rfd/xcv_8909 \
        platform/edrv/fmd/rdafm_8909

    ## remove unneeded toolpool
    rm -rf toolpool/8809e2_factory_ate \
        toolpool/8809e2_ram_check \
        toolpool/8955_factory_test \
        toolpool/check_all \
        toolpool/flash_update \
        toolpool/modem_recovery \
        toolpool/ram_check_all_904 \
        toolpool/chip_soft_bist \
        toolpool/map \
        toolpool/burn_effuse_host
}

build_package_phone() {
    echo "Skip package phone"
    build_goke_flash_tool_lib 8955g_modem
    build_flash_programmer
    ## fetch goke flash tool libs
    rm -rf $GOKE_FLASH_TOOL_DIR/packed; mkdir -p $GOKE_FLASH_TOOL_DIR/packed
    scp -r ${bb_res_host}:${bb_res_path}/$GOKE_FLASH_TOOL_DIR/packed/* $GOKE_FLASH_TOOL_DIR/packed

    verify_flash_programmer $IDH_PHONE_NAME
}

build_package_modem() {
    build_stack_lib 5827_gprs_libs 8955 stack_dualsim_gprs
    build_stack_lib 5827_gprs_trace_libs 8955 stack_dualsim_gprs_trace
    build_fota_lib 8955_modem
    build_goke_flash_tool_lib 8955g_modem

    build_modemdb 8955_modem debug
    build_modemdb 8955_modem release
    build_flash_programmer

    cleanup
    run_vergen
    sed -i "/^WITH_SVN/c\WITH_SVN ?= 0" env/compilation/customvars.mk
    sed -i "/^WITH_GIT/c\WITH_GIT ?= 0" env/compilation/customvars.mk
    sed -i "/^WITH_VERGEN/c\WITH_VERGEN ?= 0" env/compilation/customvars.mk
    sed -i "/^CT_USER/c\CT_USER ?= FAE" env/compilation/customvars.mk

    ## fetch stack libs
    rm -rf $STACK_DIR/packed; mkdir -p $STACK_DIR/packed
    scp -r ${bb_res_host}:${bb_res_path}/$STACK_DIR/packed/* $STACK_DIR/packed

    ## fetch fota libs
    rm -rf $FOTA_DIR/packed; mkdir -p $FOTA_DIR/packed
    scp -r ${bb_res_host}:${bb_res_path}/$FOTA_DIR/packed/* $FOTA_DIR/packed

    ## fetch goke flash tool libs
    rm -rf $GOKE_FLASH_TOOL_DIR/packed; mkdir -p $GOKE_FLASH_TOOL_DIR/packed
    scp -r ${bb_res_host}:${bb_res_path}/$GOKE_FLASH_TOOL_DIR/packed/* $GOKE_FLASH_TOOL_DIR/packed

    ## remove files not to be released
    package_remove_files

    local REVISION=$(echo $bb_revision | cut -c1-8)
    local IDH_MODEM_NAME=idh_modem_$REVISION.tar.bz2

    pushd ..
    tar cfj soft/${IDH_MODEM_NAME} --owner=0 --group=0 \
        --exclude-vcs \
        soft/application/systeminc/interface/hwdrv/include/lcd_if.h \
        soft/application/systeminc/mmi/include/kal_non_specific_general_types.h \
        soft/at \
        soft/newfcfg \
        soft/platform \
        soft/env \
        soft/target/include \
        soft/target/src \
        soft/target/8955g_modem \
        soft/target/8955_modem \
        soft/target/8955_modem_media \
        soft/target/8955_ramrun \
        soft/target/Makefile \
        soft/target/target_common.def \
        soft/toolpool \
        soft/Makefile \
        soft/Kconfig
    popd

    upload_file $IDH_MODEM_NAME . $IDH_MODEM_NAME

    verify_modemdb 8955_modem debug $IDH_MODEM_NAME
    verify_modemdb 8955_modem release $IDH_MODEM_NAME
    verify_flash_programmer $IDH_MODEM_NAME
}

extract_target_release() {
    # example: modem-target-debug
    echo "$1" | cut -d- --output-delimiter=' ' -f2,3
}

build() {
    local TARGET=$1
    echo "build $TARGET ......"

    case "$TARGET" in
    modem-*-debug|modem-*-release) build_modem $(extract_target_release $TARGET) ;;
    modemdb-*-debug|phone-*-release) build_modemdb $(extract_target_release $TARGET) ;;
    phone-*-debug|phone-*-release) build_phone $(extract_target_release $TARGET) ;;
    flash_programmer) build_flash_programmer ;;
    package) build_package ;;
    *) echo "Unknown target: $TARGET"; false ;;
    esac
}

usage() { echo "$0 usage:" && grep ")\ #" $0; exit 0; }

if ! options=$(getopt -o hb: -l help,linux-patch-list,windows-patch-list,linux-merge-list,windows-merge-list,linux-daily-list,windows-daily-list,build: -n "$0" -- "$@"); then
    exit 1
fi

eval set -- "$options"

while [ $# -gt 0 ]; do
    case "$1" in
    --linux-patch-list|--linux-merge-list) #   list Linux patch/merge target list
        echo "modemdb-8955_modem-debug"
        echo "modemdb-8955_modem_media-debug"
        echo "modemdb-8955g_modem-debug"
        # echo "phone-5827_evb_wap-debug"
        # echo "phone-5827_watch_240_tp-debug"
        # echo "phone-5827_watch_128_tp-debug"
        # echo "phone-5827_evb-debug"
        # echo "phone-r630_8809e2-debug"
        # echo "phone-5827_watch_x3-debug"
        # echo "phone-forme_test_8809e2_open_all_features-debug"
        echo "flash_programmer"
        # echo "modem-8909_nb2g-debug"
        # echo "modem-8909_nbiot-debug"
        shift
        ;;
    --windows-patch-list|--windows-merge-list) # list Windows patch/merge target list
        echo "modemdb-8955_modem-debug"
        # echo "phone-5827_evb_wap-debug"
        # echo "phonesim-5827_watch_240_tp-debug"
        shift
        ;;
    --linux-daily-list) #   list Linux daily target list
        echo "package"
        echo "modemdb-8955g_modem-debug"
        echo "modemdb-8955_modem_media-debug"
        echo "modemdb-8955_modem_gcf-debug"
        # echo "phone-5827_evb_wap_audiocalib-debug"
        # echo "phone-5827_evb_wap_4.3v-debug"
        # echo "phone-5827_watch_240_tp-debug"
        # echo "phone-5827_watch_240_kids-debug"
        # echo "phone-5827_watch_240_kids_audiocalib-debug"
        # echo "phone-5827_watch_128_tp-debug"
        # echo "phone-5827_watch_128-debug"
        # echo "phone-5827_watch_x3-debug"
        # echo "phone-forme_test_8809e2_open_all_features-debug"
        # echo "phone-forme_test_8809e2_qvga-debug"
        # echo "phone-r630_8809e2-debug"
        # echo "phone-5827_evb_gcf-debug"
        # echo "phone-5827_evb_gcf_ldo-debug"
        echo "flash_programmer"
        # echo "modem-8909_nb2g-debug"
        # echo "modem-8909_nbiot-debug"
        shift
        ;;
    --windows-daily-list) # list Windows daily target list
        echo "modemdb-8955_modem-debug"
        # echo "phone-5827_evb_wap-debug"
        # echo "phone-5827_watch_240_tp-debug"
        # echo "phone-forme_test_8809e2_open_all_features-debug"
        # echo "phonesim-5827_watch_240_tp-debug"
        # echo "phonesim-5827_evb_wap-debug"
        shift
        ;;
    -b|--build) #           build a target
        build $2
        shift; shift
        ;;
    -h|--help) #            display help
        usage
        ;;
    *) shift;;
    esac
done
