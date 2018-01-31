puts "Updating the calibration gains for M310."

activateCalibGui

cwAddWatch($map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].inGain)
cwAddWatch($map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain)
#cwAddWatch($map_table.>.calib_access.>.bb.>.audio[0].audioGains[0].inGain)
#cwAddWatch($map_table.>.calib_access.>.bb.>.audio[0].audioGains[0].outGain)
cwRefreshWatches

calib_start_stub
sleep 2

$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[0].w 8
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[1].w 2
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[2].w 3
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[3].w 4
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[4].w 5
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[5].w 7
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[6].w 6
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].outGain[7].w 7
$map_table.>.calib_access.>.hstBb.>.audio[0].audioGains[0].inGain.w 13

calib_init
calib_update
calib_burn_flash(true)

rst
puts "Done."
