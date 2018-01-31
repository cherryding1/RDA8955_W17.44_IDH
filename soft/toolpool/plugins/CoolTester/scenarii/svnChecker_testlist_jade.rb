require "CoolTester/scenarii/svncheckerpostbuild/proto.rb"


def getTestHashJade(soft_workdir)
    
    # TESTS informations :
    # Proto :
    proto_path="#{soft_workdir}/platform_test/stack/proto"
    proto_options=[
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=cool_profile" },
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=debug" },
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=release" }
    ]

    # Proto PST :
    proto_pst_path="#{soft_workdir}/platform_test/stack/proto_PST"
    proto_pst_options=[
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=cool_profile" },
    ]

    # my_test :
    my_test_path="#{soft_workdir}/platform_test/hal/mytest"
    my_test_options=[
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=cool_profile" }
    ]
    
    # reftest_skel :
    reftest_path="#{soft_workdir}/platform_test/reftest_skel"
    reftest_options=[
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=cool_profile" }
    ]
    
    # coolmmi
    coolmmi_path="#{soft_workdir}/application/coolmmi_jade/main"
    coolmmi_options=[
        { "mkopts" => "CT_TARGET=dev5_rf3_64 CT_RELEASE=cool_profile CT_MMI=f658" }
    ]
    
    begin
    testHash={
        proto_path=>proto_options,
        proto_pst_path=>proto_pst_options,
        my_test_path=>my_test_options,
        reftest_path=>reftest_options #,
        # coolmmi_path=>coolmmi_options
    }
    end
    
    testHash
    
end
