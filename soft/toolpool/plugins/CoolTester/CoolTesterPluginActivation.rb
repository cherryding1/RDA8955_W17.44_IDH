require "CoolWatcher/cwiconlib.rb"

require "CoolTester/TestsHelp.rb"

require "find"

include Find

module CoolTesterPluginActivation
    
    @@TestsLoadsuccessfully=[]
    
    include TestsHelp
    
    #####################################
	#			Private section			#
	#####################################
    
    private
    
    class CT_Plugin_Activ_Err < StandardError
    end
 
    def CoolTesterPluginActivation.createEntryMenu(menuName,testName,testInstance,testClass,testParameters="{}")
        cwAddMenuCommand("CoolTester","#{testName}","puts my#{testInstance}=#{testClass}.new(#{testParameters}).process",0)
    end
    
    def CoolTesterPluginActivation.addTestToMenu(testFile)
        # String operations to get test class name and test name...          
        testClass=testFile.gsub(/.rb$/,"")
        testName=testClass.gsub(/_/," ")
        testInstance=testName.gsub(/ /,"")
                               
        case testFile
    
            when /Board/
                createEntryMenu("Board tests",testName,testInstance,testClass,testParameters="{}")        
            when /Hal/
                createEntryMenu("Hal tests",testName,testInstance,testClass,testParameters="{}")
            when /Proto/
                # The Call test is specific : use start/end a call icons.
                if testFile=~/Auto_Call/
                     cwAddMenuCommand("CoolTester","#{testName}","puts 'Please use phone icons to start/stop a call'",0)  
                     # Create phone icons.    
                     cwAddScriptButton("CoolTester","myCall=Proto_Auto_Call.new({:start=>0}) ; puts myCall.process ;" , CWIconLib::REDPHONEICON,"Cooltester call")
                     cwAddScriptButton("CoolTester","myCall=Proto_Auto_Call.new({:timeout=>10, :start=>1, :number=>'112'}) ; puts myCall.process ;" , CWIconLib::GREENPHONEICON,"Cooltester call 112")
                else
                    createEntryMenu("Proto tests",testName,testInstance,testClass,testParameters="{}")   
                end 
                if testFile=~/Multi_MO/
                     cwAddMenuCommand("CoolTester","#{testName}","puts 'Please use phone icons to start/stop a Multi call'",0)  
                     # Create phone icons.    
                    cwAddScriptButton("CoolTester","myCall=Proto_Auto_Multi_MO.new({ :nbr_of_mo=>5, :call_number=>'888'}) ; puts myCall.process ;" , CWIconLib::GREENPHONEICON,"Cooltester call multi 888")
                end
            else
                 
        end #end case
    
    end
    
    def CoolTesterPluginActivation.buildHelpMenu
        coolTesterWebPlatformURL='http://atlas/~cooltester/'
        
        cwAddMenuCommand("CoolTester","","",0)
        cwAddMenuCommand("CoolTester","Tests description",
            "openUrl('#{coolTesterWebPlatformURL}tests.php')",0)
        cwAddMenuCommand("CoolTester","Scenarii description",
            "openUrl('#{coolTesterWebPlatformURL}scenarii.php')",0)
        cwAddMenuCommand("CoolTester","Reports publication",
            "openUrl('#{coolTesterWebPlatformURL}reports.php')",0)
    end
    
    
    #####################################
	#			Public section			#
	#####################################
    
    public
        
    # Add all paths stored in pathsArray to mainPath.
    # Can raise a CT_Plugin_Activ_Err exception.
    def CoolTesterPluginActivation.addLoadPaths(mainPath, pathsArray)
        begin
            pathsArray.each do | pathToAdd |
                puts "html><font color='blue'>" + "Add path #{pathToAdd}" + "</font>"
                mainPath << pathToAdd
            end
        rescue
            # An error ocurred in the addLoadPaths process.
            # Raise a CT_Plugin_Activ_Err exception.
            raise CT_Plugin_Activ_Err , "Adding load paths error : #{$!}"          
        end
    end
    
    # Load all ruby scripts in userTestPath directory.
    # Create CW menus : Board tests, Hal tests, GSM tests, Others tests with tests ready to be processed.
    # Tests parameters will be the defaults ones.
    # Can raise a CT_Plugin_Activ_Err exception.
    def CoolTesterPluginActivation.loadUserTests(userTestPath)
        
        allTestsLoadSuccessfully=true
        
        begin
            # Open userTestPath directory.    
            puts "html><font color='blue'>" + "Opening #{userTestPath}..." + "</font>"
            testsDirectory=Dir.open(userTestPath).sort
            puts "Done successfully"
            
            testsList = []
            
            # Create an array of users tests, removing dismissed files.
            Find.find(userTestPath) do |path|
                testsList << path.split("/")[-1]  if(path[-3..-1]==".rb")
            end
            
            puts "html><font color='blue'>"+"Loading tests...\n" + "</font>"
            
            testsList.each do |testFile|
                puts "html><font color='blue'>"+"Loading '"+testFile+"'..." + "</font>"
                
                # Load test file and add it to the menu.
                begin
                    load testFile                       
                    # If the load is succesful, we add the test to @@TestsLoadsuccessfully array.
                    @@TestsLoadsuccessfully<< testFile
                    puts "Done successfully"                   
                rescue 
                    puts "html><font color='red'>"+ "Failed !" + "</font>"
                    # Print the detail.
                    puts "#{$!}"
                    allTestsLoadSuccessfully=false
                    # Keep trying to load the following tests...
                end 
                                                   
            end 
                        
        rescue
            # An error occurred while opening user test directory.
            puts "html><font color='red'>"+ "Failed !" + "</font>"
            # Print error detail.
            puts "#{$!}"
            raise CT_Plugin_Activ_Err , "Opening #{userTestPath} directory failed" 
        end
        
        # A user test directory has been open correctly.
        # Check is one user test script failed while loading.
        # If yes, raise a CT_Plugin_Activ_Err to warn the caller.
        if allTestsLoadSuccessfully==false 
            raise CT_Plugin_Activ_Err , "One or more tests failed while loading"
        end
        
    end
    
    def CoolTesterPluginActivation.createTestsHelp(tests_list_file,tests_content_file)
        puts "html><font color='blue'>"+"Generating Tests Help..."+ "</font>"
        begin
            TestsHelp.saveToFiles(tests_list_file,tests_content_file)
            puts "Generating Tests Help done successfully" 
        rescue TestsHelp_Err
            puts "html><font color='red'>"+ "Generating Tests Help failed !" + "</font>"
            raise CT_Plugin_Activ_Err , "#{$!}"
        end 
    end
    
    def CoolTesterPluginActivation.setGUI
        puts "html><font color='blue'>"+"Setting CoolTester GUI..."+ "</font>"
        # Add Init Button.
        cwAddScriptButton('CoolTester',"puts CoolTester.init",CWIconLib::INITICON,"Initialize Cooltester")
        @@TestsLoadsuccessfully.each do |testFile|
            CoolTesterPluginActivation.addTestToMenu(testFile)
        end
        CoolTesterPluginActivation.buildHelpMenu
        puts "Done successfully" 
    end
      
    
end
