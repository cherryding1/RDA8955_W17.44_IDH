require "help.rb"

module TestsHelp
    #####################################
	#			Private section			#
	#####################################

    @@TESTHELP=Hash.new()
    
    private
    
    class TestsHelp_Err < StandardError
    end
    
    class TestSection
		def initialize()
			@entries = Hash.new
			@description = ""
		end
		
		def description
			@description
		end
		
		def description=(des)
			@description = des
		end
	
		def entries
			@entries
		end
	end
     
    class TestEntry
	    attr_reader :name, :version, :environement,:type, :help
	    @parameters=[]
	    
        def initialize(name, version, environement, type, prerequisites, parameters, coverage, results, example, author)
            @name=name
            @version=version
            @environement=environement
            @type=type
            @prerequisites=prerequisites
            @parameters=parameters
            @coverage=coverage
            @results=results
            @example=example
            @author=author	
        end
	  
	    def makeHelp
	    
    	    @help=""
            @help <<  "<table class='test'>"
            # First raw : [TestName | Version ]
            @help << "<tr bgcolor=#d0d0d0>"
            @help << "<td class='description'>Name</td>" 
            @help << "<td class='center_content'><span class='test_name'>%s</span> ( %s )</td>" %[@name,@version]
            @help << "</tr>"

            # Second raw : [ Environement | Type ]
            @help << "<tr bgcolor=#f0f0f0>"
            @help << "<td class='description'>Environement | Type</td>" 
            @help <<  "<td class='center_content'>%s | %s </td>" %[@environement,@type]
            @help << "</tr>"

            # Third raw : [ Prequisites ]
            @help << "<tr bgcolor=#d0d0d0>"
            @help << "<td class='description'>Prerequisites</td>" 
            @help <<  "<td class='justify_content'>%s</td>" % @prerequisites
            @help << "</tr>"
        
            # Fourth raw : [ Parameters ]
            @help << "<tr bgcolor=#f0f0f0>"
            @help << "<td class='description'>Parameters</td>" 
            @help <<  "<td class='justify_content'>" 
  
            @help << "<ul>"

            @parameters.each do |parameterDescription|
                @help << "<li class='parameter_li'>"
                @help << "<span class='parameter_name'>%s</span> " % parameterDescription[0]
                @help << "<span class='parameter_default'> ( default : %s )</span> " % parameterDescription[1]
                @help << "</li>"
                @help << "<span class='parameter_description'> %s</span> " % parameterDescription[2]   
            end

            @help << "</ul>"
            @help << "</td>"
            @help << "</tr>"
       
            # Fifth raw : [ Test coverage ]          
            @help << "<tr bgcolor=#d0d0d0>"
            @help << "<td class='description'>Test coverage</td>" 
            @help <<  "<td class='justify_content'>%s</td>" % @coverage
            @help << "</tr>"

            # Sixth raw : [ Results ]
            @help << "<tr bgcolor=#f0f0f0>"
            @help << "<td class='description'>Results</td>" 
            @help <<  "<td class='justify_content'>%s</td>" % @results
            @help << "</tr>"

            # Seventh raw : [ Example ]
            @help << "<tr bgcolor=#d0d0d0>"
            @help << "<td class='description'>Example</td>" 
            @help <<  "<td class='left_content'>%s</td>" % @example
            @help << "</tr>"
        
            # Eighth raw : [ Author ]
            @help << "<tr bgcolor=#f0f0f0>"
            @help << "<td class='description'>Author</td>" 
            @help <<  "<td class='justify_content'>%s</td>" % @author
            @help << "</tr>"
            @help << "</table>"
    
            return @help
    
        end
    
    end
	
	def addTestSection(str_section)
		@@TESTHELP[str_section] = TestSection.new()
	end
	
    #####################################
	#			Public section			#
	#####################################
    
    public
    
    def TestsHelp.addTestHelpEntry(name, version, environement, type, prerequisites, parameters, coverage, results, example, author)
    
        if(!@@TESTHELP[environement]) then
			addTestSection(environement)
		end
		
        @@TESTHELP[environement].entries[name]=TestEntry.new(      name,
                                                                    version,
                                                                    environement,
                                                                    type,
                                                                    prerequisites,
                                                                    parameters,
                                                                    coverage,
                                                                    results,
                                                                    example,
                                                                    author
                                                            )
    
    
    end
    
    
    def TestsHelp.saveToFiles(tests_list_file="//Atlas/n/users/cooltester/public_html/tests_list.php",
                            tests_content_file="//Atlas/n/users/cooltester/public_html/tests_content.php")
		
		tests_list=""
		tests_content=""
		test_href=0
		
		@@TESTHELP.sort.each do |section|
		    sectionName=section[0]
		    sectionObject=section[1]
		    
		    # Add testSection informations.
		    tests_list << "<a href='##{sectionName}' class='test_section_link'>#{sectionName}</a> <br />"
		    tests_content << "<br />"
		    tests_content << "<span class='test_section_main'> <a name='#{sectionName}'> <h1> #{sectionName} :</h1> </a></span>"
		    
		    
		    sectionObject.entries.sort.each do |test|
		        testName=test[0]
		        testEntry=test[1]
		        
		        tests_list << "<a href='##{test_href}' class='text_linkgray'>#{testName}</a> <br />"
		    
		        tests_content << "<span class='text_main'> <a name='#{test_href}'> <h2> #{testName} :</h2> </a></span>"
		        tests_content << testEntry.makeHelp
		        tests_content << "<br />"
		        test_href+=1
	        end
        end
			
		begin
    		
    		File.open(tests_list_file,"wb") do |file|
    		    puts "html><font color='blue'>"+"Writting tests list in #{tests_list_file}..." + "</font>"
    		    file << tests_list
    		end
    		
    	    puts "Done successfully"
    	    
    		File.open(tests_content_file,"wb") do |file|
    		    puts "html><font color='blue'>"+"Writting tests description in #{tests_content_file}..." + "</font>"
    		    file << tests_content
    		end
    		
    		puts "Done successfully"
    		
		rescue
		puts "html><font color='red'>"+ "Failed !" + "</font>"
		raise TestsHelp_Err , "Error while writting tests help" + "#{$!}"
		end
		
    end
		        
end     



