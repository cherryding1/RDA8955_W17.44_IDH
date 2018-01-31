#!/bin/bash -e

#make gen_opt
TARGET_DIR=${SOFT_WORKDIR}/target/$1
FLASH_DIR=${SOFT_WORKDIR}/platform/edrv/memd/$2
BIN_PATH=$3
CT_ERES=$4
USRGENDIR=${SOFT_WORKDIR}/toolpool/usrgenerator
export USRGEN_BUILD_DIR=${BIN_PATH}/usrgenerator

echo "Creating user data, please wait..."
# Clean up all the old stuff
cd $USRGENDIR
make clean

# Create directory for intermediate files
mkdir -p $USRGEN_BUILD_DIR

#这里把相关目录的头文件拷贝到resgen来保证和工程的同步
#cp cs_types.h。由于不能修改工程文件，所以需要在cp过程做些修改。下同。

cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/vds/src/* $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/ffs/src/* $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/ml/src/* $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/nvram/src/* $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/fs_simu/src/flash_simu_mem.c $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/fs_simu/src/drv_mmc_simu.c $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/base/fs_simu/src/flash_simu_mem.h $USRGENDIR
cp $SOFT_WORKDIR/application/adaptation/nvram/include/*.h  $USRGENDIR
cp $SOFT_WORKDIR/application/adaptation/nvram/src/nvram_cust_pack.c $USRGENDIR
cp $SOFT_WORKDIR/application/adaptation/nvram/src/nvram_user_config.c $USRGENDIR
cp $SOFT_WORKDIR/application/target_res/$CT_ERES/mmi_cfg/include/mmi_features.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/cfw_multi_sim.h $USRGENDIR
cp $SOFT_WORKDIR/platform/include/cos.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/cswtype.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/event.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/drv_flash.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/drv_mmc.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/fs.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/ts.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/fs_asyn.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/errorcode.h $USRGENDIR
cp $SOFT_WORKDIR/${PLATFORM_SERVICE}/include/sul.h $USRGENDIR

cp $SOFT_WORKDIR/platform/include/dbg.h $USRGENDIR
cp $SOFT_WORKDIR/target/${CT_TARGET}/include/fpconfig.h $USRGENDIR
cp $SOFT_WORKDIR/target/include/tgt_dsm_cfg.h $USRGENDIR
cp $SOFT_WORKDIR/application/coolmmi/mmi/Rwatch/include/rwatchSetting.h $USRGENDIR

sed -i "/USER_DATA_CACHE_SUPPORT\|CSW_NO_TRACE/d" $USRGENDIR/fpconfig.h

perl $USRGENDIR/incp.pl "$SOFT_WORKDIR/application/coolmmi/mmi/Audio/include/audioinc.h" \
	"$USRGENDIR/audioinc.h" \
	1 \
	"#include\s+\"queuegprot.h\"" \
	3 \
	"" 

perl $USRGENDIR/incp.pl "$USRGENDIR/custom_mmi_default_value.h" \
	"$USRGENDIR/custom_mmi_default_value.h" \
	1 \
	"#include\s+\"settingprofile.h\"" \
	3 \
	"" 

perl $USRGENDIR/incp.pl "$SOFT_WORKDIR/platform/include/cs_types.h" \
										"$USRGENDIR/cs_types.h" \
										9 \
										"typedef\s+unsigned\s+long\s+uintmax_t;" \
										3 \
										"" \
										"typedef\s+long\s+intmax_t;" \
										3 \
										"" \
										"#define\s+INTMAX_MAX\s+0x7fffffffffffffff"	\
										3 \
										"" \
										"#define\s+INTMAX_MAX\s+0x7fffffff"	\
										3 \
										"" \
										"#define\s+UINTMAX_MAX\s+0xffffffffffffffff"	\
										3 \
										"" \
										"#define\s+UINTMAX_MAX\s+0xffffffff"	\
										3 \
										"" \
										"#define\s+INTMAX_MIN\s+-\(0x7fffffff-1\)"	\
										3 \
										"" \
										"#define\s+__P\(protos\)\s+protos"	\
										3 \
										"" \
										"#define\s+offsetof\(TYPE,\s+MEMBER\)\s+\(\(u32\)\s+&\(\(TYPE\s+\*\)0\)->MEMBER\)"	\
										3 \
										"" \
										"#define\s+NULL\s+0"	\
										3 \
										"" 

perl $USRGENDIR/incp.pl "$TARGET_DIR/include/tgt_app_cfg.h" \
	"$USRGENDIR/tgt_app_cfg.h" \
	1 \
	"#include\s+\"uctls_tgt_params.h\"" \
	3 \
	"" 
	
perl $USRGENDIR/incp.pl "$FLASH_DIR/src/drv_flsh_map.c" \
	"$USRGENDIR/drv_flsh_map.c" \
	1 \
	"#include\s+\"mem_id.h\"" \
	3 \
	"" 


cp $TARGET_DIR/include/fpconfig.h $TARGET_DIR/include/fpconfig.h_bak
echo "backup done"

for flag in USER_DATA_CACHE_SUPPORT CSW_NO_TRACE
do
	find $TARGET_DIR/include/ -name "fpconfig.h" | xargs sed -i "/$flag/d"
done
echo "$1 fileter out done"

rm -f $TARGET_DIR/include/fpconfig.h
mv $TARGET_DIR/include/fpconfig.h_bak $TARGET_DIR/include/fpconfig.h
echo "$1 revert done"

make $USRGEN_BUILD_DIR/usrgen
$USRGEN_BUILD_DIR/usrgen -o $USRGEN_BUILD_DIR/user.bin
if [ $CT_USERLOD == "yes" ] ; then
	if [ ! -d $SOFT_WORKDIR/hex/$1_$CT_RELEASE ]; then
		mkdir -p $SOFT_WORKDIR/hex/$1_$CT_RELEASE
	fi
  cp $USRGENDIR/user.lod $SOFT_WORKDIR/hex/$1_$CT_RELEASE
fi
make cleanSrc
echo "usrgen done"

