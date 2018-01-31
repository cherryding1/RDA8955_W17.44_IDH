#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

print "start Granite vitac-equ synthesis-bug check"
restart(true,true,true);
print "Init test pattern";
# Clean up the BB SRAM
wb($BB_SRAM.address,Array.new(13*1024,0x0));
# Load the test data BB_SRAM:
# @ 0x0   : 156 symbols
# @ 0x300 : PM init
# @ 0x400 : BmSums
ld("N:/projects/Granite/laurent/Granite/soft/toolpool/scripts/Granite/vitac_testset.lod",true);

# Regs setup
print "Setup Vitac module"
$VITAC.command.w 0;
$VITAC.ch_symb_addr.w(0x10+0x108);
$VITAC.pm_base_addr.w 0x400;
$VITAC.exp_symb_addr.w(0x300+0x20);
$VITAC.out_base_addr.w 0x1000;
$VITAC.H0_param.w 0x00040001;
$VITAC.HL_param.w 0x0003FFFD;
$VITAC.rescale.w 0x3FFF;
$VITAC.command.nb_symbols.w 61;
$VITAC.command.int_mask.w 1;
$VITAC.command.sv_shift_lev.w 9;
$VITAC.command.bm_shift_lev.w 5;
$VITAC.command.bkwd_trellis.w 1;
# start
$VITAC.command.start_equ.w 1;
while($VITAC.status.equ_pending.r == 1) do print "Vitac running..." end;
print "...Done";
ver("N:/projects/Granite/laurent/Granite/soft/toolpool/scripts/Granite/vitac_test_results.lod");
print "Yes! ViT4C Ru13Z !!";
