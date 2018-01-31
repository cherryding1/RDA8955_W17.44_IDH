begin
    # Load script only if we're in coolwatcher
    include CoolWatcher
    require "CoolTester/scenarii/svncheckerpostbuild/proto.rb"
rescue
end

def getTestHashModem2G(soft_workdir)

calib_file = "N:/projects/Greenstone/calib_files/calib_greenstone_gouinette_v2b.cfp"
flash_programmer = "C:/CSDTK/cooltools/chipgen/Modem2G/toolpool/plugins/fastpf/" + "flash_programmers/host_greenstone_flsh_k5l2731caa_d770_ramrun.lod"
proto_auto_call_number = "123" # Voicemail for SFR.

# TESTS informations :
# Proto :
proto_path="#{soft_workdir}/platform_test/stack/proto"
proto_options=[ 
      { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_fpga_softrom     CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_fpga_usb         CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_fpga             CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_cs2324d          CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_cs2308a          CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_m301             CT_RELEASE=cool_profile COOLTESTER=1" },
#      { "mkopts" => "CT_TARGET=greenstone_eng_opal         CT_RELEASE=cool_profile COOLTESTER=1",
#          "cfpfile" => calib_file, "num" => proto_auto_call_number , "flamulate" => true,
#          "postbuild" => Proc.new {|postOpt| SVNCheckerPostBuild::protoCheck(postOpt)} },
      { "mkopts" => "CT_TARGET=greenstone_eng_ct1010       CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=greenstone_eng_ct1010       CT_RELEASE=release" },
      { "mkopts" => "CT_TARGET=greenstone_eng_ct1010       CT_RELEASE=debug" },
      { "mkopts" => "CT_TARGET=gallite_fpga                CT_RELEASE=cool_profile" },
      { "mkopts" => "CT_TARGET=gallite_fpga_softrom        CT_RELEASE=cool_profile" }
]

# coolmmi
coolmmi_path="#{soft_workdir}/."
coolmmi_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile CT_MMI=f658 " +
         "CSW_EXTENDED_API=1" },
     { "mkopts" => "CT_TARGET=greenstone_cs2324d    CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_m301       CT_RELEASE=cool_profile CT_MMI=f658 " +
         "CSW_EXTENDED_API=0" },
 in workspaces CT_USER=MMI has no meaning, we will need to use an other WS
     { "mkopts" => "CT_TARGET=greenstone_eng_QVGA   CT_RELEASE=cool_profile CT_MMI=cs2324 " +
         "CSW_EXTENDED_API=0 CT_USER=MMI" }, 
     { "mkopts" => "CT_TARGET=greenstone_cs2324     CT_RELEASE=cool_profile CT_MMI=cs2324 " +
         "CSW_EXTENDED_API=0 CT_USER=MMI" },
     { "mkopts" => "CT_TARGET=gallite_fpga         CT_RELEASE=cool_profile CT_MMI=f658 " +
         "CSW_EXTENDED_API=1" }
]

# int_rom_programmer :
int_rom_programmer_path="#{soft_workdir}/platform_test/rom/int_rom_programmer"
int_rom_programmer_options=[
     { "mkopts" => "CT_ASIC_CFG=chip CT_TARGET=greenstone_fpga_softrom CT_RELEASE=cool_profile " +
         "INT_ROM_END=0xffffffff80E14ffc" },
     { "mkopts" => "CT_ASIC_CFG=fpga CT_TARGET=greenstone_fpga_softrom CT_RELEASE=cool_profile " +
         "INT_ROM_END=0xffffffff80E14ffc" },
     { "mkopts" => "CT_ASIC_CFG=chip CT_TARGET=gallite_fpga           CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_ASIC_CFG=fpga CT_TARGET=gallite_fpga           CT_RELEASE=cool_profile" }
]

# flash_programmer
flash_programmer_path="#{soft_workdir}/platform_test/hal/flash_programmer"
flash_programmer_options=[
     { "mkopts" => "CT_TARGET=greenstone_fpga       CT_RELEASE=release CT_RAMRUN=1" },
     { "mkopts" => "CT_TARGET=gallite_fpga         CT_RELEASE=release CT_RAMRUN=1" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=release CT_RAMRUN=1 " +
         "FLSH_MODEL=flsh_k5l2731caa_d770 LOCAL_BATCH_NAME=flsh_k5l2731caa_d770" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=release CT_RAMRUN=1 " +
         "FLSH_MODEL=flsh_s71pl256nc0 LOCAL_BATCH_NAME=flsh_s71pl256nc0" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=release CT_RAMRUN=1 " +
         "FLSH_MODEL=flsh_s71ws256pd0 LOCAL_BATCH_NAME=flsh_s71ws256pd0" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=release CT_RAMRUN=1 " +
         "FLSH_MODEL=flsh_s71ws256nc0 LOCAL_BATCH_NAME=flsh_s71ws256nc0" },
     { "mkopts" => "CT_TARGET=greenstone_m301 CT_RELEASE=release CT_RAMRUN=1" },
     { "mkopts" => "CT_TARGET=greenstone_m309 CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=0" },
     { "mkopts" => "CT_TARGET=greenstone_cs2324d CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=0" },
     { "mkopts" => "CT_TARGET=greenstone_cs2308a CT_RELEASE=release CT_RAMRUN=1" },
     { "mkopts" => "CT_TARGET=greenstone_m301 CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=1" },
     { "mkopts" => "CT_TARGET=greenstone_m309 CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=1" },
     { "mkopts" => "CT_TARGET=greenstone_cs2324d CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=1" },
     { "mkopts" => "CT_TARGET=greenstone_cs2308a CT_RELEASE=release CT_RAMRUN=1 FASTPF_USE_USB=1" }
]

# reftest_skel :
reftest_path="#{soft_workdir}/platform_test/reftest/reftest_skel"
reftest_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# Proto PST :
proto_pst_path="#{soft_workdir}/platform_test/qc/proto_PSTv2.5"
proto_pst_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# my_test :
my_test_path="#{soft_workdir}/platform_test/hal/my_test"
my_test_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# monitoring_test :
monitoring_test_path="#{soft_workdir}/platform_test/pal/monitoring_test"
monitoring_test_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# fcch_sch_test :
fcch_sch_test_path="#{soft_workdir}/platform_test/pal/fcch_sch_test"
fcch_sch_test_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# tx_burst_test :
tx_burst_test_path="#{soft_workdir}/platform_test/pal/tx_burst_test"
tx_burst_test_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# calib_stub :
calib_stub_path="#{soft_workdir}/platform_test/calib/calib_stub"
calib_stub_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# reftest_uctls :
reftest_uctls_path="#{soft_workdir}/platform_test/svc/reftest_uctls"
reftest_uctls_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile" }
]

# reftest_mps :
reftest_mps_path="#{soft_workdir}/platform_test/svc/reftest_mps"
reftest_mps_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010_norom CT_RELEASE=cool_profile CSW_EXTENDED_API=1" },
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile CSW_EXTENDED_API=1" }
]

# reftest_mrs :
reftest_mrs_path="#{soft_workdir}/platform_test/svc/mrs/reftest_mrs"
reftest_mrs_options=[
     { "mkopts" => "CT_TARGET=greenstone_eng_ct1010 CT_RELEASE=cool_profile CSW_EXTENDED_API=1" }
]

    begin
    testHash={
                proto_path=>proto_options,
#                coolmmi_path=>coolmmi_options,
                int_rom_programmer_path=>int_rom_programmer_options,
                flash_programmer_path=>flash_programmer_options,
                reftest_path=>reftest_options,
#                proto_pst_path=>proto_pst_options,
                my_test_path=>my_test_options,
                monitoring_test_path=>monitoring_test_options,
                fcch_sch_test_path=>fcch_sch_test_options,
                tx_burst_test_path=>tx_burst_test_options,
#                calib_stub_path=>calib_stub_options,
                reftest_uctls_path=>reftest_uctls_options,
                reftest_mps_path=>reftest_mps_options,
                reftest_mrs_path=>reftest_mrs_options
    }
    end
   
    testHash
end


