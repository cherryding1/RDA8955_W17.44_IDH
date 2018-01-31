require "CoolTester/CoolTesterPluginActivation.rb"

include CoolTesterPluginActivation

def activateCoolTester
    
    testsPath=$PLUGINS_WORKDIR +"/CoolTester/tests"
    
    pathsToLoad=[$PLUGINS_WORKDIR +"/CoolTester",
                testsPath,
                $PLUGINS_WORKDIR +"/CoolTester/scenarii"
                ]
    
    puts "Adding paths to $LOAD_PATH : "
    begin
        CoolTesterPluginActivation.addLoadPaths( $LOAD_PATH, pathsToLoad )
        CoolTesterPluginActivation.loadUserTests(testsPath)
        if (FileTest.exists?("//Atlas/n/users/cooltester/public_html/tests_list.php"))
            CoolTesterPluginActivation.createTestsHelp("//Atlas/n/users/cooltester/public_html/tests_list.php",
            "//Atlas/n/users/cooltester/public_html/tests_content.php" )
        else
            CoolTesterPluginActivation.createTestsHelp("/n/users/cooltester/public_html/tests_list.php",
            "/n/users/cooltester/public_html/tests_content.php" )
        end
        
        CoolTesterPluginActivation.setGUI
    
         puts "html><font color='darkgreen'>"+"CoolTester Plugin activation successed !" +"</font>"
         puts "Check you have load a .lod with GTES !"
         puts "Once, init CoolTester thanks to the 'blue initialization icon' and run your tests/scenarii!"
    rescue CT_Plugin_Activ_Err
        puts "html><font color='red'>"+"An error Occured while activating CoolTester plugin : " +"</font>"
        # Print error infos.
        puts "#{$!}"
    end
    
end

#CoolWatcher specific
begin
    include CoolWatcher
    cwAddMenuCommand("Plugins","Activate CoolTester","activateCoolTester()",0)
rescue Exception
end

