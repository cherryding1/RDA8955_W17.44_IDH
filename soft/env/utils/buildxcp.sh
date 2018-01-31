#!/bin/bash

XCPS="platform/edrv/wifi/include/mcd_profile_codes.xcp
platform/chip/regs/8809/include/regs_profile_codes.xcp
platform/chip/bcpu/spc/include/spc_profile_codes.xcp
platform/chip/boot/include/boot_profile_codes.xcp
platform/edrv/tsd/include/tsd_profile_codes.xcp
platform/system/calib/include/calib_profile_codes.xcp
platform/chip/bcpu/spp/include/spp_profile_codes.xcp
platform/chip/pal/include/pal_profile_codes.xcp
platform/chip/bcpu/spal/include/spal_profile_codes.xcp
platform/edrv/lcdd/include/lcdd_profile_codes.xcp
platform/system/stack/include/stk_profile_codes.xcp
platform/chip/hal/include/hal_profile_codes.xcp
platform/edrv/dualsimd/rda1203_gallite_CT1129/include/dualsimd_profile_codes.xcp
platform/service/include/csw_profile_codes.xcp
platform/edrv/pmd/include/pmd_profile_codes.xcp
platform/base/sx/include/sx_profile_codes.xcp"

# Not included

# at/include/at_profile_codes.xcp
# platform/system/vpp/speech/soft/efr/include/vpp_efr_profile_codes.xcp
# platform/system/vpp/speech/soft/hr/include/vpp_hr_profile_codes.xcp
# platform/system/vpp/speech/soft/fr/include/vpp_fr_profile_codes.xcp
# platform/system/svc/cams/include/cams_profile_codes.xcp
# platform/system/svc/avrs/include/avrs_profile_codes.xcp
# platform/system/svc/imgs/include/imgs_profile_codes.xcp
# platform/system/svc/mps/include/mps_profile_codes.xcp
# platform/system/svc/mrs/include/mrs_profile_codes.xcp
# platform/system/svc/avps/include/avps_profile_codes.xcp
# platform/edrv/mcd/include/mcd_profile_codes.xcp
# platform/edrv/dualsimd/rda1203_gallite/include/dualsimd_profile_codes.xcp
# platform/edrv/dualsimd/foursimd/include/foursimd_profile_codes.xcp
# platform/edrv/dualsimd/threesimd/include/threesimd_profile_codes.xcp

COMBINED=combined.xcp
echo "<cpconfig>" > $COMBINED
for f in $XCPS; do
    if test ! -f $f; then
        echo "$f not exist!"
    else
        echo "$f ......"
        cat $f >> $COMBINED
        xcp2h -i $f -o $(dirname $f) -u
    fi
done
echo "</cpconfig>" >> $COMBINED

# check conflict
echo "Check conflict ......"
xcp2h -i $COMBINED -o . > /dev/null