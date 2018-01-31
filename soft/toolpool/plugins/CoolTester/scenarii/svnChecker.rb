# This file contains the SvnChecker module.
# It allows to check the SVN database through a dedicated user soft Workdir ( typically cooltester ).
# Requires CodeManagement.rb and Mail.rb files.
# Requires also 'rubygems' and more especially the 'yaml' one.
#
# Author::     Matthieu Ruiz  (mailto:matthieu.ruiz@gmail.com)
# Maintainer:: Benjamin Babut (mailto:benjamin@coolsand-tech.fr)
# Copyright::  2003-2007, Coolsand Technologies, Inc. 

require 'rubygems'
require 'yaml'

=begin
# Windows implementation
$LOAD_PATH<<"N:/projects/Granite/mruiz/Granite/soft/toolpool/plugins/CoolTester/"
require 'CodeManagement.rb'
=end
require 'CoolTester/CodeManagement.rb'
require 'CoolTester/Mail.rb'

module SvnChecker
    
    include CodeManagement
    
    #####################################
	#			Constant section		#
	#####################################
    
    # Report msgs.
    
    Succeeded="<b> <font color='darkgreen'> Succeeded !</font> </b> "
    Failed="<b> <font color='red'> Failed !</font> </b>"
    Skipped="<b> <font color='orange'> Skipped !</font> </b>"
        
    #####################################
	#		Class Constant section		#
	#####################################
    
    # Internal string storing the log of the process.
    # Each time the process is run, this log string is stored into a file specified in #checkCodeRoutine.
    # Furthermore, if a problem occures during the process, this string is sent by mail to @@CoolTesterAdmin.
    @@log_str=""
    
    # Internal string storing the web report of the process.
    @@web_report_str=""
    
    # Array storing commits which have brought $SOFT_WORKDIR modifications.
    @@softCommitFromBaseArray=[]
    
    # Object storing last revision processed by svnChecker and last revision Succeeded, count of svnChecker process.
    @@svnCheckerinfos=""
    
    # Date of the report. Uses as a php variable.
    @@date_suffix=""
    
    # New BASE revision.
    @@newBaseRevision=""
    
    @@report_path=""
    
    @@report_subfolder=""
    
    @@soft_workdir=""

    # Class constants which can be CONFIGURED in #config:
    
    # Guy warned when a exception occures during the svnChecker process.
    @@CoolTesterAdmin="benjamin@coolsand-tech.fr"
    # Others people who should be warned too.
    @@CoolPeopleToBeWarned=""  
    # VPN timeout when excuting a cmd : 3 min by default.
    @@VpnTimeout=60*3
    # Host Timeout : one hour by default.
    @@HostTimeout=3600
    # Count of processes before sending a "Still Alive" mail to @@CoolTesterAdmin.
    # This class constant enables to check is the svnChecker is still running or not.
    @@CountProcessBeforeMail=48
    # Used to save the status of the update command
    @@UpdateHasModification=false
    
    # List of users who can receive a guilty email.
    @@UserList = ["benjamin", "cooltester", "eric", "fei", "jba", "julien",
        "laurent", "lilian", "mathieu", "mike", "nadia", "philippe", "pierre",
        "romuald", "samia", "victor", "yordan"]
    
    #####################################
	#			Private section			#
	#####################################
    
    private
    
    #####################################
	#			 Error Classes          #
	#####################################
    
    # 'Virtual' classes :
    class SvnChecker_Err < StandardError
    end
    
    class SvnChecker_Err_Timeout < SvnChecker_Err
    end
    
    class SvnChecker_Err_File < SvnChecker_Err
    end
    
    class SvnChecker_Err_File_Yaml < SvnChecker_Err_File
    end
    
    # Inherited from 'virtual' error classes :
    class SvnChecker_Err_Timeout_VPN < SvnChecker_Err_Timeout
    end
    
    class SvnChecker_Err_Timeout_Host < SvnChecker_Err_Timeout
    end
    
    class SvnChecker_Err_File_GetUserPasswd < SvnChecker_Err_File
    end
    
    class SvnChecker_Err_File_StoreWebReport < SvnChecker_Err_File
    end
    
    class SvnChecker_Err_File_StoreLog < SvnChecker_Err_File
    end
     
    class SvnChecker_Err_File_Yaml_Load < SvnChecker_Err_File_Yaml
    end
    
    class SvnChecker_Err_File_Yaml_Dump < SvnChecker_Err_File_Yaml
    end
        
    #####################################
	#	    Error Classes end           #
	#####################################
    
    #####################################
	#			 Other Classes          #
	#####################################
    
    class UserCommiterStats
        attr_accessor :commitsDone
        attr_accessor :commitsFailed
        # Ratio is the % of your successful commits.
        attr_accessor :ratio
        attr_accessor :lastCommitDate
        
        private
        
        def updateRatio
            @ratio=100-(@commitsFailed*100.0)/@commitsDone
        end
        
        public
        
        # When adding a new Commiter profile, add him one commit done and zero commit failed :).
        def initialize(time=nil)
            @commitsDone=1
            @commitsFailed=0
            @ratio=100
            @lastCommitDate=time
        end
        
        # Raise @commitsDone and update the ratio.
        def addCommitDone(time)
            @commitsDone+=1
            updateRatio
            @lastCommitDate=time
        end
        
        # Raise @commitsFailed and update the ratio.
        def addCommitFailed
            @commitsFailed+=1
            updateRatio
        end
        
        # Compare two UserCommiterStats.  
        def <=> anOtherUserCommiterStats
            if self.ratio <  anOtherUserCommiterStats.ratio
                # self is a badder 'commiter' than anOtherUserCommiterStats.
                return -1
            else
                if self.ratio ==  anOtherUserCommiterStats.ratio
                    # Same ratio, self is a better 'commiter' if did more commit.
                    if self.commitsDone > anOtherUserCommiterStats.commitsDone
                        return 1
                    else    
                        # self.commitsDone <= anOtherUserCommiterStats.commitsDone
                        if self.commitsDone == anOtherUserCommiterStats.commitsDone
                            return 0
                        else
                            # self.commitsDone > anOtherUserCommiterStats.commitsDone
                            return -1
                        end
                    end
                else
                    # self.ratio >  anOtherUserCommiterStats.ratio
                    return 1
                end
            end          
        end
    end
    
    class SvnCheckerInformations
        attr_accessor :lastRevisionProcessed , :lastRevisionSucceeded, :count, :statsUsers
        @statsUsers={}
        
        private
        
        def addTitletoWebTable(title, html)
            html << "<th align='center'>"
            html << "%s" % title
            html << "</th>"
        end
           
        def addDataToWebTable(data, html)
            html << "<td align='center'>"
            html << "%s" % data
            html << "</td>"
        end
          
        public
        
        def to_s
            return "lastRevisionProcessed = %s \n lastRevisionSucceeded = %s \n count = %s " % [@lastRevisionProcessed,@lastRevisionSucceeded,@count]
        end
        
        def addCommitDone(user, time)
            if @statsUsers.has_key?(user)
                # This user has already done at least one commit.
                # Add this new commit in its commitsDone field.
                @statsUsers[user].addCommitDone(time)
            else
                # It's the first time this user has done a commit.
                # Add user key.
                @statsUsers[user]=UserCommiterStats.new(time)
            end 
        end
        
        def addCommitFailed(user)
            # We assume @statsUsers has already the key 'user'
            @statsUsers[user].addCommitFailed
        end
        
        def generateCommitRatio
            reportDays=30
            rankedIndex=0
            ratio_hash={}
            html="<table class='test'>"
            html << "<caption align='bottom'> Top ranked soft coolsand 'committers'#{if reportDays then ' of last '+reportDays.to_s+' days' end} </caption>"
            html<< "<tr>"
            addTitletoWebTable("Rank", html)
            addTitletoWebTable("User", html)
            addTitletoWebTable("Commits Done", html)
            addTitletoWebTable("Commits Failed", html)
            addTitletoWebTable("Ratio", html)
            html<< "</tr>"
            
            fitered_user=@statsUsers.clone
            if reportDays then
                fitered_user.delete_if do |userName,userStats|
                    if userStats.lastCommitDate then
                        (Time.new - userStats.lastCommitDate) > (60*60*24*reportDays)
                    else
                        true
                    end
                end
            end

            fitered_user.sort {|a,b| a[1]<=>b[1] }.reverse.each do |userProfile|
                rankedIndex+=1
                userName=userProfile[0]
                userStats=userProfile[1]
                
                html<< "<tr>"
                addDataToWebTable(rankedIndex, html)
                addDataToWebTable(userName, html)
                addDataToWebTable(userStats.commitsDone, html)
                addDataToWebTable(userStats.commitsFailed, html)
                html << "<td align='center'>"
                html << "%.2f %" %  userStats.ratio
                html << "</td>"
                 
                html<< "</tr>"
            end
            
            html << "</table>"
          
        end
           
    end

    #####################################
	#	    Other Classes end           #
	#####################################  
    
    #####################################
	#	        Annexe Methods          #
	#####################################  
     
    # Add info to @@log_str, plus timing information.
    # Furthermore, print this info to stdout.
    def SvnChecker.addToLog(info)
        @@log_str << "#{Time.new} : " + info +"\n"
        if(info && info.to_s.length()<10000)
            puts info.to_s
        else
            puts "Information too big to be stdouted. Check log."
        end
    end
    
    # Add info into @@web_report_str. Add the html tag "<br />" if not precised.
    def SvnChecker.addToReport(info, br=true)
         @@web_report_str << info
         @@web_report_str << "<br />\n" if br==true   
    end
    
    # Process the given exception, adding the exception class name with the detail stored in the global variable $!.
    def SvnChecker.processException(exception, add_to_report = false)
        addToLog(exception.class.name)
        addToLog(exception.backtrace.to_s)
        addToLog($!)
        
        if add_to_report
            addToReport "Error detail (#{exception.class.name}): "
            addToReport(exception.backtrace.to_s)
            addToReport("#{$!}".gsub("\n","<br />\n"))
            addToReport "</li>", false
        end
    end
    
    
    # Load svnChecker informations from a *.yaml file, and store its into @@svnCheckerinfos.
    # Can raise SvnChecker_Err_File_Yaml_Load to warn the upper caller.
    def SvnChecker.getYamlInformations(yaml_file)
        @@svnCheckerinfos=SvnCheckerInformations.new
        addToLog("Loading svnChecker process informations from #{yaml_file} ... ")
        begin
            # Read yaml informations from yaml_file and convert it into a ruby object.
            File.open( yaml_file, 'r' ) do |input|
                @@svnCheckerinfos=YAML::load( input )
            end
            addToLog(:Succeeded.to_s)
            addToLog("Last revision processed by svnChecker : %s" % @@svnCheckerinfos.lastRevisionProcessed )
            addToLog("Last successful revision  : %s" % @@svnCheckerinfos.lastRevisionSucceeded )
            addToLog("Count svnChecker process  : %s" % @@svnCheckerinfos.count  )
        rescue Exception=> e
            processException(e)
            raise SvnChecker_Err_File_Yaml_Load
        end
                 
    end
    
    # Dump svnChecker informations from @@svnCheckerinfos object into the given *.yaml file.
    # Can raise SvnChecker_Err_File_Yaml_Dump to warn the upper caller.
    def SvnChecker.storeYamlInformations(yaml_file)
        addToLog("Dumping svnChecker process informations into #{yaml_file} ... ")
        begin
            # Convert ruby_object into a yaml_object and store it into yaml_file.
            File.open( yaml_file, 'w' ) do |output|
                YAML::dump(@@svnCheckerinfos, output )
            end
            addToLog(:Succeeded.to_s)
        rescue Exception=> e
            processException(e)
            raise SvnChecker_Err_File_Yaml_Dump
        end      
    end
    
    # Get #{user} unix password from local file, and return it.
    # Can raise SvnChecker_Err_File_GetUserPasswd to warn the upper caller.
    def SvnChecker.getCoolTesterPasswd(passwd_file, user)
        addToLog("Getting #{user} passwd from #{passwd_file} ... ")
        passwd=""
        begin
            File.open(passwd_file,"rb") do |file|
                passwd << file.gets    
            end 
            addToLog(:Succeeded.to_s)  
        rescue Exception=> e
            processException(e)
            raise SvnChecker_Err_File_GetUserPasswd
        end
        passwd  
    end
    
    # Store @@web_report_str into the given web_file ( typically a *.php one).
    # Update the latest report file via a php include in report_latest_file.
    # Can raise SvnChecker_Err_File_StoreWebReport to warn the upper caller.
    # Build a list of reports.
    def SvnChecker.storeWebReport(report_directory, report_subfolder, report_file, report_latest_file,
                                  report_list, report_date)
        begin
            addToLog("Writing the web report into #{report_file}..." )
            File.open(report_directory+"/"+report_subfolder+"/"+report_file,"wb") do |file|
        		    file << @@web_report_str
    		end  
    		addToLog(:Succeeded.to_s)  
    		
    		addToLog("Updating report list #{report_list}..." )
    		File.open(report_directory+"/"+report_subfolder+"/"+report_list,"r+") do |file|
    		        content = file.read()
                    file.rewind()
        		    file << "<a href='svnChecker.php?file=%s' class='text_linkgray'>" % [report_subfolder+"/"+report_file]
        		    file << "Report #{report_date}</a><br/>\n"
        		    file << content
    		end  
    		addToLog(:Succeeded.to_s)  
    		
    		addToLog("Updating latest report into #{report_latest_file}..." )
    		File.open(report_directory+"/"+report_subfolder+"/"+report_latest_file,"wb") do |file|
        		    file << "<?php include '#{report_file}' ; ?>"
    		end  
    		addToLog(:Succeeded.to_s)  
    				
        rescue Exception=> e
            processException(e)
            raise SvnChecker_Err_File_StoreWebReport
        end   
    end
    
  
    
    
    
    # Store @@log_str into the given log_file.
    # Can raise SvnChecker_Err_File_StoreLog to warn the upper caller.
    def SvnChecker.storeLog(log_file)
        addToLog("Writing the process log into #{log_file}..." )
        begin
            File.open(log_file,"wb") do |file|
        		    file << @@log_str
    		end  
    		addToLog(:Succeeded.to_s)  
        rescue Exception=> e
            processException(e)
            raise SvnChecker_Err_File_StoreLog
        end   
    end
    
    # Check if the svnChecker process has been executed @@CountProcessBeforeMail times.
    # If true, send a mail to @@CoolTesterAdmin, indicating the process is still running.
    def SvnChecker.stillAliveProcess
        addToLog("Still Alive Process ...")  
        if @@svnCheckerinfos.count==@@CountProcessBeforeMail
            # It's time to send a 'Still Alive mail' to @@CoolTesterAdmin.
            @@svnCheckerinfos.count=0
            CoolTesterMailer.deliver_report("SVN Checker still alive !",@@svnCheckerinfos.to_s, @@CoolTesterAdmin)
        else
            @@svnCheckerinfos.count=@@svnCheckerinfos.count+1          
        end
        addToReport "<b> #{@@CountProcessBeforeMail-@@svnCheckerinfos.count} </b>  process(es) last before sending a 'Still alive' mail to the CoolTester admin"                
        addToReport "Last revision processed :<b> %s </b>" % @@svnCheckerinfos.lastRevisionProcessed   
        addToReport "Last successful revision  :<b> %s </b>" % @@svnCheckerinfos.lastRevisionSucceeded  
        addToLog(:Succeeded.to_s)  
    end
    
    # Update commit done stats for each user. If a user has done 2 commits from last update, add 2 commits.
    def SvnChecker.updateCommitDone
        @@softCommitFromBaseArray.each do |commit|
                @@svnCheckerinfos.addCommitDone(commit.user, commit.time)     
            end
    end
    
    # Update commit failed stats for each user. If a user has done 2 commits from last update, add 2 commits.
    def SvnChecker.updateCommitFailed
        @@softCommitFromBaseArray.each do |commit|
                @@svnCheckerinfos.addCommitFailed(commit.user)     
            end
    end
    
    # "unixName" -> "unixName@coolsand-tech.fr"
    def SvnChecker.buildMailAddress(unixName)
        if (@@UserList.include? unixName) then
            return unixName + "@coolsand-tech.fr"
        else
            return nil
        end
    end
    
    # Return a mail list with all user mail who did a commit since last update.
    def SvnChecker.buildGuiltyMailList()    
        addToLog("Building 'Guilty people' mail list..." )
        guiltyMailList=[]                
        @@softCommitFromBaseArray.each do |commit|
            guiltyMailList << buildMailAddress(commit.user)
        end
        # Remove duplicate elements.
        guiltyMailList.uniq!
        addToLog("Successful : %s" % guiltyMailList.join(","))
        return guiltyMailList.join(",")
    end
    
    # Return a customize compilation error mail.
    def SvnChecker.buildCompilationErrorMail()
        return "A compilation error has been detected by CoolTester (#{@@report_subfolder}). This error has occurred since your recent commit. That's why CoolTester has an inkling that you are involved in this problem. Please go to <a href='http://atlas/~cooltester/svnChecker.php?file=#{@@report_path}'>coolTester web internal platform</a> for more information.<br />If you think this mail is unfair, please contact your CoolTester admin.<br />Thanks.<br /><br />CoolTester." 
    end
       
    # Send an alert mail with @@log_str to @@CoolTesterAdmin and @@CoolPeopleToBeWarned.
    def SvnChecker.sendAlertMail
        others=""
        others=","+@@CoolPeopleToBeWarned if @@CoolPeopleToBeWarned !=""
        CoolTesterMailer.deliver_report("SVN Checker: Alert!",@@log_str, @@CoolTesterAdmin+others) 
    end
    
    # Send an alert mail to @@CoolTesterAdmin, @@CoolPeopleToBeWarned and guilty people.
    def SvnChecker.sendCompilationErrorMail( guiltyMailList )
        others=""
        others=","+@@CoolPeopleToBeWarned if @@CoolPeopleToBeWarned !=""
        guiltyGuys=""
        guiltyGuys=","+guiltyMailList if guiltyMailList !=""
        mailHeader="SVN Checker: Same error!"
        # If there is not guilty people, it means that nothing has been changed.
        mailHeader="SVN Checker: Compilation error!" if guiltyMailList !=""

         mail_content=buildCompilationErrorMail()+"<hr>"+@@svnCheckerinfos.generateCommitRatio 
        CoolTesterMailer.deliver_report(
                                        mailHeader,
                                        mail_content, 
                                        @@CoolTesterAdmin+others+guiltyGuys,
                                        "text/html")

    end
    
    # Add the ranked user table to the report.
    def SvnChecker.addRankedTableToReport
        addToReport "<hr />"
        addToReport @@svnCheckerinfos.generateCommitRatio
    end
    
    # Add all the configurations options to the report.
    def SvnChecker.addProcessOptionsToReport  
       addToReport "<hr />"
       addToReport "<u>SVN Checker configuration options : </u>"
       addToReport "<br />",false
       addToReport "SVN Checker admin : <b>%s</b> " % @@CoolTesterAdmin
       addToReport "People to be warned in case of issues : <b>%s</b> " % @@CoolPeopleToBeWarned
       addToReport "VPN timeout : <b>%s</b> min " % (@@VpnTimeout/60).to_s
       addToReport "Host timeout : <b>%s</b> min " % (@@HostTimeout/60).to_s
       addToReport "Count of svnChecker process execution before sending a 'Still Alive' mail : <b>%s</b>" % @@CountProcessBeforeMail.to_s       
    end
    
    #####################################
	#	     Shell cmd Methods          #
	#####################################  
    
    # Execute a shell cmd and store the result process into @@log_str.
    # Can raise CM_Err_CmdFailed to warn the upper caller.
    def SvnChecker.executeAndLogShellCmd( shell, cmd)
        begin 
            addToLog(cmd+"...") 
            # Can raise CM_Err_CmdFailed => rescued.
            CodeManagement.sendCommonCmd( shell, cmd)
            # The cmd succeeded, store it into the log.
            addToLog(:Succeeded.to_s) 
        rescue CM_Err_CmdFailed => e
            processException(e)
            # Raise the cmd again to warn the upper caller.
            raise
        end       
    end
    

    
    # Execute svn log cmd from BASE to HEAD in givent path and with given username account.
    # This cmd has a @@VpnTimeout timeout.
    # Add modifications in @@web_report_str.
    # Can raise CM_Err_SvnLogFailed or SvnChecker_Err_Timeout_VPN( to be processed ) to warn the upper caller.
    def SvnChecker.executeSvnLogCmdViaVPN( shell, path, username)
        stdout=""
        addToLog("svn log #{path} --username #{username} -r BASE:HEAD -v...") 
   
        begin
            
            Timeout::timeout(@@VpnTimeout) do
                if @@UseWorkspace then
        		    @@softCommitFromBaseArray,stdout=CodeManagement.svnLogWS(shell, path, username)
                else
        		    @@softCommitFromBaseArray,stdout=CodeManagement.svnLog(shell, path, username)
                end
    		end
            
            addToLog(:Succeeded.to_s)  
            addToLog("logStdout = \n %s" % stdout )  
            
            # Remove the BASE log commit since we have already taken into account this one.
            @@softCommitFromBaseArray.delete_if do |commit|
                commit.revision==@@svnCheckerinfos.lastRevisionProcessed   
            end
            
            # We check if @@softCommitFromBaseArray is empty or not.
            if @@softCommitFromBaseArray.empty? == false
                    
                # Commit in $SOFT_WORKDIR have been done still last update.
                # Add modifications in the web_report.
                addToReport "Below, modifications from BASE to HEAD in $SOFT_WORKDIR :"
                
                @@softCommitFromBaseArray.each do |commit|
                    addToReport commit.changed_paths.gsub("\n","<br />")
                end
                
                # Update users stat.
                addToLog("Updating users commits done stats...") 
                updateCommitDone
                addToLog(:Succeeded.to_s)  
                
            else
                # No commit in $SOFT_WORKDIR have been done still last update.
                addToReport "No modification from BASE to HEAD in $SOFT_WORKDIR"     
            end
                       
        rescue Timeout::Error
           raise SvnChecker_Err_Timeout_VPN, "VPN seems to be down ! "
        rescue CM_Err_SvnLogFailed => e
            processException(e) 
            raise
        end  
    end

    #Used by the dichotomy tool
    def SvnChecker.executeSvnLogFromRevToRevCmdViaVPN(shell,path,username,from,to)
        stdout=""
        addToLog("svn log #{path} --username #{username} -r #{from}:#{to} -v...") 
   
        begin
            
            Timeout::timeout(@@VpnTimeout) do
    		    @@softCommitFromBaseArray,stdout=CodeManagement.svnLog(shell, path, username,from,to)
    		end
            
            addToLog(:Succeeded.to_s)  
            
            return @@softCommitFromBaseArray
            
        rescue Timeout::Error
           raise SvnChecker_Err_Timeout_VPN, "VPN seems to be down ! "
        rescue CM_Err_SvnLogFailed => e
            processException(e) 
            raise
        end  
    end    
    
    
    # Execute svn update cmd at revision HEAD with given username account.
    # This cmd has a @@VpnTimeout timeout.
    # Store the new revision into @@newBaseRevision.
    # Can raise CM_Err_SvnUpdateFailed or SvnChecker_Err_Timeout_VPN ( to be processed ) to warn the upper caller.
    def SvnChecker.executeSvnUpdateCmdViaVPN( shell, username)
        addToLog("svn up --username mirror -r HEAD ...") 
        stdout=""
        begin
            
            Timeout::timeout(@@VpnTimeout) do
    		    @@newBaseRevision,stdout=CodeManagement.svnUpdate( shell, 'HEAD', "mirror" )  	
    		end
            @@UpdateHasModification = ( (stdout =~ /\n[UAD] /) != false )
            
            addToLog(:Succeeded.to_s) 
            addToLog(stdout)
            # Update succeeded ! Add this information in web_report.
            addToReport "svn up --mirror : #{:Succeeded} "
            # Now, BASE=HEAD= the new @@newBaseRevision.
            addToLog("Now, BASE=HEAD=#{@@newBaseRevision}") 
            addToReport "Now, BASE=HEAD=#{@@newBaseRevision}"           
   
        rescue Timeout::Error
            addToReport "svn up --mirror : #{Failed} "
            raise SvnChecker_Err_Timeout_VPN, "VPN seems to be down ! "
        rescue CM_Err_SvnUpdateFailed => e
            # The svn log cmd failed.
            processException(e) 
            addToReport "svn up --mirror : #{Failed} "
            addToReport "#{$!}".gsub("\n","<br />")
            raise
        end  
    end
    
    #This function is intended to be used for the dichotomy bug finder
    #Feature of the SVN checker
    def SvnChecker.svnUpdateToRevisionViaVPN(shell, username, revision)
        addToLog("svn up --username mirror -r %s ..." % revision.to_s) 
        stdout=""
        begin
            
            Timeout::timeout(@@VpnTimeout) do
    		    rev,stdout=CodeManagement.svnUpdate( shell, revision.to_s, "mirror" )  	
    		end
            
            addToLog(:Succeeded.to_s) 
            addToLog(stdout)  
            
        rescue Timeout::Error
            raise SvnChecker_Err_Timeout_VPN, "VPN seems to be down ! "
        rescue CM_Err_SvnUpdateFailed => e
            # The svn log cmd failed.
            processException(e) 
            raise e
        end  
    end    
    
    
    def SvnChecker.postBuild( shell, testPath, testOptions )
        #Construct a list of post operation parameters
        #Call the postbuild function associated to this target if there's one.
        
        postbuild = testOptions["postbuild"]
        return true if(!postbuild)
        
        testname=testPath.split("/").last
        
        #Transfort make options into a hash
        makopts = {}
        testOptions["mkopts"].split(/ /).collect{ |x| x.split( /=/ ) }.each{ |e| makopts[e[0]]=e[1] }        
        
        loc     = (makopts["LOCAL_BATCH_NAME"])?(makopts["LOCAL_BATCH_NAME"]+"_"):""
        targ    = makopts["CT_TARGET"] + "_"
        rel     = makopts["CT_RELEASE"]
        prefix  = testname + "_" + targ + loc + rel 
        
        postopts = testOptions.clone
        postopts["mkopts"]      = makopts
        postopts["testname"]    = testname
        postopts["loddir"]      = @@soft_workdir + "/hex/" + prefix
        
        if(RUBY_PLATFORM.split("-")[1]=="mswin32")
            postopts["loddir"] = postopts["loddir"].sub("/n/","N:/")
            postopts["loddir"] = postopts["loddir"].sub("/k/","K:/")
        end
        
        ramrun = false
        
        if(makopts["CT_RAMRUN"])
            ramrun = (makopts["CT_RAMRUN"]=="yes")?true:false
        end
        
        if(ramrun)
            postopts["lodname"] = prefix + "_ramrun.lod"
        else
            if(File.exists?(postopts["loddir"]+"/" + prefix + "_flash.lod"))
                postopts["lodname"] = prefix + "_flash.lod"
            else
                postopts["lodname"] = prefix + "_romulator.lod"
            end
        end
        
        postopts["logprint"] = Proc.new{ |l| SvnChecker.addToLog(l) }
        postopts["reportprint"] = Proc.new{ |l| SvnChecker.addToReport(l) }
        
        begin
            res = postbuild.call( postopts )
            addToReport "Post build operation. => <b> <font color='green'> Succeeded !</font> </b>"
        rescue Exception => e
            processException(e, true)
            addToLog "Failure during post build operation!"
            addToReport "Post build operation. => <b> <font color='red'> Failed !</font> </b>"
            res = false
        end
        
        return res
    end
    
    # Check last mofications, compiling tests in testHash with the required comilation options.
    # If an error occured, it is locally rescued to let others tests been compiled too.
    # Return true if all compilations succeeded. Otherwise, return false.
    def SvnChecker.checkCommits( shell, testHash )
        
        # Let's assume all 'make' had succeeded before processing #checkCommits.
        # If one compilation failed, this variable becomes 'false'. 
        allMakesucceeded = true
        
        startCompilation = Time.new
        compilationDuration = 0
        
        addToLog("Checking modifications...")                                 
        testHash.each_pair do |test_path, test_options|
               
            test_name = test_path.split("/").last
            
            begin
                addToReport "<ul> <b> #{test_name} </b>",false
                executeAndLogShellCmd( shell, "cd #{test_path}")
                
                # test_options is an array with required pool of options.
                test_options.each do |optionhash|
                
                    # Local CM_Err rescue to allow following compilations.
                    begin
                        option = optionhash["mkopts"]    
                            
                        # Make clean lod still with test_options !
                        # Can raise CM_Err_MakeStartFailed,CM_Err_MakeCompilationOrLinkFailed.
                        # Locally rescued.
                        addToLog("#{test_name}: make clean lod #{option}")  
                        
                        addToReport "<li>", false
                        CodeManagement.make( shell, " clean " + " lod " + option)
                         
                        # Make succeeded ! Add this information in web_report.
                        addToLog(:Succeeded.to_s )
                        addToReport "make clean lod #{option} => #{Succeeded} "
                        addToReport "</li>", false
                        
                        #Do the postbuild
                        pbres = postBuild(shell, test_path, optionhash)
                        
                        # If the post build failed, inform SVN Checker about that.
                        allMakesucceeded = false if pbres == false
                        
                    rescue Exception => e
                        # Can rescue CM_Err_MakeStartFailed or CM_Err_MakeCompilationOrLinkFailed              
                        # The current 'make' had just failed.
                        allMakesucceeded = false
                        processException(e)
                        addToReport "#{test_name}: make clean lod #{option} => #{Failed} "
                               
                        addToReport "Error detail (#{e.class.name}): "
                        # Add exception detail stored in the global variable $!.
                        addToReport "#{$!}".gsub("\n","<br />\n")
                                
                        addToReport "</li>", false
                        
                    end # end begin rescue local compilation errors.
            
                end # end test_options.each do |option|.
            
            rescue CM_Err_CmdFailed => e
                # The current can not be processed.
                allMakesucceeded = false
                
                # An error occured in executeAndLogShellCmd(shell, "cd #{test_path}", log).
                processException(e)
                
                # Make can't have been processed :
                addToReport "#{test_name}: make clean lod  => #{Skipped} "
                 
                addToReport "Warning detail (Make can't have been processed) : "
                addToReport "cd #{test_path} failed!"
                addToReport "#{$!}".gsub("\n","<br />")
                
                addToReport "</li>",false
            rescue Exception => e
                # The current make can not be processed.
                # Unknown cause ...
                processException(e)     
            ensure
                addToReport "</ul>"
            end # end begin rescue locally in ".each_pair do |test_path, test_options|..end "loop
                   
        end # end @@Tests.each_pair do |test_path, test_options|.
        compilationDuration=(Time.new-startCompilation)/60
        addToLog("Checking modifications : #{:Succeeded.to_s}") 
        addToLog("Compilation duration : %.2f min" % compilationDuration)      
        addToReport "Compilation lasted <b> %.2f </b> min" % compilationDuration
        return allMakesucceeded
    end
    
    #####################################
	#	     Algorithm Methods          #
	#####################################  
	
    # Change svn Checker state ( @@svnCheckerinfos.lastRevisionSucceeded and @@svnCheckerinfos.lastRevisionProcessed.)
    # Depends of modification and allMakeSuccessful entries.
    def SvnChecker.processStateMachine(modification, allMakeSuccessful)
        
        if modification==true
            if allMakeSuccessful==true
              @@svnCheckerinfos.lastRevisionSucceeded=@@newBaseRevision
            else
                # @@svnCheckerinfos.lastRevisionSucceeded stays unchanged.
            end
        else
            if @@svnCheckerinfos.lastRevisionProcessed==@@svnCheckerinfos.lastRevisionSucceeded
                # Since no modification occured, the new current revision should also be a succeeded one!
                @@svnCheckerinfos.lastRevisionSucceeded=@@newBaseRevision
            else
                # @@svnCheckerinfos.lastRevisionSucceeded stays unchanged.
            end
        end   
        
        # Update @@svnCheckerinfos.lastRevisionProcessed.
        @@svnCheckerinfos.lastRevisionProcessed=@@newBaseRevision
    
    end
    
    # Analyze modifications from last update.
    # If modifications, check it. If errors occurred while processing test compilation, send a mail to "Guilty people"
    # Finally, update svnChecker state.
    def SvnChecker.analyzeUpdate(shell, testHash)
        
        modification=false
        allMakeSuccessful=false
        
        # Recompile if updated has modifications
        if @@UpdateHasModification == true
            
            modification=true                            
            
            # Let's check modifications added to $SOFT_WORKDIR since last time.
            if checkCommits( shell, testHash)==false
                # One or more compilation processes failed.
                # Warn involved people.
                
                # Update users stat.
                addToLog("Updating users failed commits stats...") 
                updateCommitFailed
                addToLog(:Succeeded.to_s)  
                
                sendCompilationErrorMail( buildGuiltyMailList )
                
            else
                # All compilations succeeded ! The current revision successeed.
                allMakeSuccessful=true
            end
            
        else
            # No modification, modification=false                          
        end 
        addToLog("SM state : modification=#{modification}...") 
        addToLog("SM state : allMakeSuccessful=#{allMakeSuccessful}...") 
        # Change the internal state machine.
        processStateMachine(modification, allMakeSuccessful)       
    end
    
    
     
    #####################################
	#			Public section			#
	#####################################
          
    public
    
    # Configurate SvnChecker process via an hash parameter. Possible keys are the following :
    # :CoolTesterAdmin -> Guy warned when a exception occures during the svnChecker process ( mail )
    # :CoolPeopleToBeWarned -> Others people who should be warned too ( mail split with a comma)
    # :VpnTimeout -> VPN timeout when excuting a cmd : 3 min by default.
    # :HostTimeout ->Host Timeout for compiling all tests : one hour by default.
    # :CountProcessBeforeMail -> Count of processes before sending a "Still Alive" mail to @@CoolTesterAdmin.
    def SvnChecker.config(hash)
       @@CoolTesterAdmin=hash[:CoolTesterAdmin]     if hash[:CoolTesterAdmin]!= nil
       @@CoolPeopleToBeWarned=hash[:CoolPeopleToBeWarned]   if hash[:CoolPeopleToBeWarned]!= nil
       @@VpnTimeout=hash[:VpnTimeout]   if hash[:VpnTimeout]!=nil
       @@HostTimeout=hash[:HostTimeout] if hash[:HostTimeout]!=nil
       @@CountProcessBeforeMail=hash[:CountProcessBeforeMail]   if hash[:CountProcessBeforeMail]!=nil
       @@UseWorkspace=(hash[:UseWorkspace])?true:false
    end
    
    def SvnChecker.setShellEnvironment(shell,soft_workdir)
        Timeout::timeout(@@HostTimeout) {
            # Export SOFT_WORKDIR env variable to be cooltester soft workdir compliant.
            # Can raise CM_Err_CmdFailed => rescued.
            executeAndLogShellCmd( shell, "export SOFT_WORKDIR=#{soft_workdir}")
        }
        Timeout::timeout(@@HostTimeout) {
            # Open ${SOFT_WORKDIR}/env directory.
            # Can raise CM_Err_CmdFailed => rescued.
            executeAndLogShellCmd( shell, "cd ${SOFT_WORKDIR}/env")
        }
        Timeout::timeout(@@HostTimeout) {
            # Source set_env.sh, fixing the environement( paths etc...) in the same shell.
            # Can raise CM_Err_CmdFailed => rescued.
            executeAndLogShellCmd( shell, "source set_env.sh")
        }
        Timeout::timeout(@@HostTimeout) {       
            # Open ${SOFT_WORKDIR} directory.
            # Can raise CM_Err_CmdFailed => rescued.
            executeAndLogShellCmd( shell, "cd ${SOFT_WORKDIR}")   
        } 
    end

    #For dichotomy tool purpose. Will update, compile, and check a revision 
    #using the posbuild operation defined in the option hash
    def SvnChecker.checkRevision(host,user,passwd_file,soft_workdir, test_path,optionhash,revision)
        
        passwd=getCoolTesterPasswd(passwd_file, user)
        @@soft_workdir = soft_workdir
        
        begin
                
            
                  
            addToLog("Opening a synchronous shell via SSH on #{host}...") 
            # Open a synchronous no interactive shell.
            # Can raise CM_Err_AuthenticationFailed, or CM_Err_ShellOpenFailed exceptions => rescued.
            CodeManagement.openSyncShell( host, user, passwd )  do |shell|
                
                begin #rescue Timeout::Error
                    
                    addToLog(:Succeeded.to_s)

                    SvnChecker.setShellEnvironment(shell,soft_workdir)                    

                    Timeout::timeout(@@HostTimeout) {
                        # Update 'HEAD' revision with 'mirror' chinese account.
                        # Can raise CM_Err_SvnUpdateFailed or Timeout::Error  => rescued.                  
                        svnUpdateToRevisionViaVPN( shell, "mirror",revision)
                    }
                          
                    test_name=test_path.split("/").last
                    Timeout::timeout(@@HostTimeout) {
                        executeAndLogShellCmd( shell, "cd #{test_path}")
                    }
    
                    option = optionhash["mkopts"]   
                    optionhash["rev"] = revision 
                            
                    # Make clean lod still with test_options !
                    # Can raise CM_Err_MakeStartFailed,CM_Err_MakeCompilationOrLinkFailed.
                    # Locally rescued.
                    addToLog("#{test_name}: make clean lod #{option}")  
     
                    CodeManagement.make( shell, " clean " + " lod " + option)
                     
                    # Make succeeded ! Add this information in web_report.
                    addToLog(:Succeeded.to_s )
               
                    #Do the postbuild
                    return postBuild(shell,test_path,optionhash)  
                                           
                rescue Timeout::Error
                    raise SvnChecker_Err_Timeout_Host, "Host seems to be down !"
                end
            end # end CodeManagement.openSyncShell( host, user, passwd )  do |shell| 
        end 
    end

    # SVNChecker feature to do a dichotomy check on a special target. To be run in CW seperatly.
    # host : host on which to compile
    # user : user used to log on the machine (seemingly cooltester)
    # passwd_file : file containing user password
    # soft_workdir : svn soft repository path
    # test_path : path to the target
    # optionhash : hash containing compilation options, and parameters for the revision check. See the postBuild function
    # for more information of how to configure this hash to pass custom code to be tested.
    # from_revision : bottom of the checked range
    # to_revision : top of the checked range
    def SvnChecker.dichotomyCheck(host,user,passwd_file,soft_workdir,test_path,optionhash,from_revision,to_revision)
        
        
        @@log_str = ""
        @@soft_workdir = soft_workdir
        
        low = from_revision
        high = to_revision
        
        first = false
        last = true
        
        passwd=getCoolTesterPasswd(passwd_file, user)
              
        addToLog("Opening a synchronous shell via SSH on #{host}...") 
        
        commits = []
        
        # Open a synchronous no interactive shell.
        CodeManagement.openSyncShell( host, user, passwd )  do |shell|
            
            SvnChecker.setShellEnvironment(shell,soft_workdir)
    
            puts "Getting commit list between revisions."
            Timeout::timeout(@@HostTimeout) {
                commits = SvnChecker.executeSvnLogFromRevToRevCmdViaVPN(shell,soft_workdir, "mirror",low,high)
            } 
        end
      
        commits.collect! { |e| e = e.revision.to_i }
            
        commits.unshift(low) if(commits[0]!=low)
        (commits << high)  if(commits[-1]!=high)
        puts "html><br><b>Revisions to inspect : #{commits.inspect}</b><br>"
        
        puts "html><b>Trying to find lowest compiling revision.</b>"
        loop do
             if(!commits[0])
                puts "html><br><b>Could not find any compiling lowest revision!</b>"
                puts "html><b>Stopping process!</b>"
                return
             end
             
             begin
                puts "html><br><b>Testing revision #{commits[0]} (first)</b>"
                first = SvnChecker.checkRevision(host,user,passwd_file,soft_workdir,test_path,optionhash,commits[0])
               
                #If we get here the compilation has succeeded. So break.
                break
             rescue CM_Err_MakeFailed
                puts "html><br><b>Make failed! Eliminating this boundary, trying next lowest one.</b>"
                commits = commits[1..-1]
             end
        end
        puts "html><br><b>Revision #{commits[0]} is eligible for lowest compiling revision. The test status is : #{first}</b><br>" 

        puts "html><b>Trying to find highest compiling revision.</b>"
        loop do
             if(!commits[-1])
                puts "<br><b>Could not find any compiling highest revision!</b>"
                puts "<b>Stopping process!</b>"
                return
             end
             
             begin
                puts "html><br><b>Testing revision #{commits[-1]} (last)</b>"
                last = SvnChecker.checkRevision(host,user,passwd_file,soft_workdir,test_path,optionhash,commits[-1])
               
                #If we get here the compilation has succeeded. So break.
                break
             rescue CM_Err_MakeFailed
                puts "html><br><b>Make failed! Eliminating this boundary, trying next highest one.</b>"
                commits.pop
             end
        end
        puts "html><br><b>Revision #{commits[-1]} is eligible for highest compiling revision. The test status is : #{last}</b><br>" 

        if(last==first)
            puts "html><br><b>Revision #{commits[0]} and #{commits[-1]} are both %s.</b>" % [(first)?("working"):("not working")]
            puts "html><b>Stopping dichotomy process.</b>"
            return 
        else
            puts "html><br><b>Using revisions #{commits[0]} and #{commits[-1]} as valid boundaries.</b>"
            puts "html><b>Starting dichotomy process.</b>"
        end
        
        while(commits.size>2)    
            puts "html><br><b>Still to go : #{commits.inspect}.</b><br>"
            middle = (commits.size-1)/2
            puts "html><br><b>Testing revision #{commits[middle]}.</b>"
            begin
                res = SvnChecker.checkRevision(host,user,passwd_file,soft_workdir,test_path,optionhash,commits[middle])
                puts "html><br><b>Revision #{commits[middle]} is #{res}.</b><br>"
                if(res==last)
                    commits = commits[0..middle]
                else
                    commits = commits[middle..-1]
                end
            rescue CM_Err_MakeFailed
                puts "html><br><b><b>Make failed! Eliminating this revision.</b><br>"
                commits.delete_at(middle)
            end
                
        end
        
        puts "html><br>======================="
        puts "Dichotomy tool is over."
        puts ("Revision #{commits[0]} is %s." % [(first)?("working"):("not working")])
        puts ("Revision #{commits[-1]} is %s." % [(last)?("working"):("not working")])

    end
    
    def SvnChecker.setSoftWorkdir(soft_workdir)
        @@soft_workdir = soft_workdir
    end
    
    
    # Check SVN soft database.
    # Open an SSH session on host with user account. ( user passwd is stored locally in passwd_file)
    # Get last process information from yaml file lastRevision_file.
    # Update if soft_workdir path, and compile if necessary all tests in testHash with their givent compilation options.
    # Store a log of the whole process in the local file log_file.
    # Create and store a web report in report_directory.
    # SvnChecker.config should have been executed before to set config options.
    def SvnChecker.checkCodeRoutine( host, user, project, passwd_file, lastRevision_file, report_directory, report_subfolder, report_prefix,report_latest, log_file, soft_workdir, testHash )
        
        #reinitialize all globals
        @@log_str=""
        @@web_report_str=""
        @@softCommitFromBaseArray=[]
        @@svnCheckerinfos=""
        @@date_suffix=""
        @@newBaseRevision=""        
        @@report_path=""
        @@report_subfolder=""
        @@soft_workdir = soft_workdir
            
        # cooltester unix passwd.
        passwd=""
        date=Time.new
        @@date_suffix=date.strftime("%Y_%m_%d_%H_%M")
        report_file=report_prefix+"_#{@@date_suffix}.php"
        report_list=report_prefix+"_list.php"
        
        @@report_path=report_subfolder+"/"+report_file
        @@report_subfolder=report_subfolder

        puts report_file
        addToLog "%s SvnCheker log file\n\n" % project
        addToReport "Project : <b>%s</b>" % project
        addToReport "Process launched on <b> #{date} </b> on <b> #{host} </b> with <b> #{user} unix account </b>"
                       
        begin
                
            passwd=getCoolTesterPasswd(passwd_file, user)
            
            getYamlInformations(lastRevision_file)
           
            SvnChecker.stillAliveProcess 
                  
            addToLog("Opening a synchronous shell via SSH on #{host}...") 
            # Open a synchronous no interactive shell.
            # Can raise CM_Err_AuthenticationFailed, or CM_Err_ShellOpenFailed exceptions => rescued.
            CodeManagement.openSyncShell( host, user, passwd )  do |shell|
                
                begin #rescue Timeout::Error
                    
                    addToLog(:Succeeded.to_s)
                    
                    SvnChecker.setShellEnvironment(shell,soft_workdir)
                    
                    Timeout::timeout(@@HostTimeout) {
                        # Get BASE from HEAD revision informations. 
                        # Store each commit which has modificated the path in @@softCommitFromBaseArray.
                        # Can raise CM_Err_SvnLogFailed or Timeout::Error => rescued.
                        executeSvnLogCmdViaVPN( shell, soft_workdir, "mirror")
                    }
                    Timeout::timeout(@@HostTimeout) {
                        # Update 'HEAD' revision with 'mirror' chinese account.
                        # Can raise CM_Err_SvnUpdateFailed or Timeout::Error  => rescued.                      
                        executeSvnUpdateCmdViaVPN( shell, "mirror")
                    }
                    #Timeout::timeout(@@HostTimeout) {
                    # Analyze the previous Update.
                    #Julien & Ben : No good idea... this command do a lot of stuff and can take a lot time so...
                    analyzeUpdate(shell,testHash) 
                    #}                             
                                                 
                rescue Timeout::Error
                    raise SvnChecker_Err_Timeout_Host, "Host seems to be down !"
                end
                    addRankedTableToReport
                    
                    addProcessOptionsToReport  
                    
                    # Convert lastRevision ruby object into YAML object and write it in lastRevision_file.
                    storeYamlInformations(lastRevision_file)
                    
                    # Write the web_report into the .php file publishing on the CoolTester internal web platform.
                    storeWebReport(report_directory, report_subfolder,report_file, report_latest,
                                   report_list, @@date_suffix)
                    
            end # end CodeManagement.openSyncShell( host, user, passwd )  do |shell|
        
        #####################################
    	#	    Critical rescue section     #
    	#####################################
        
        # Chronological rescue :
            	
        rescue SvnChecker_Err_File, CM_Err_CmdFailed, CM_Err_SvnLogFailed, CM_Err_SvnUpdateFailed
            # An error occured while getting cooltester passwd.
            # Or, an error occured while getting Yaml informations. 
            # Or, one of the following cmds failed : 
            # -CodeManagement.sendCommonCmd( shell, "export SOFT_WORKDIR=#{soft_workdir}")
            # -CodeManagement.sendCommonCmd( shell, "cd ${SOFT_WORKDIR}/env")
            # -CodeManagement.sendCommonCmd( shell, "source set_env.sh")   
            # These cmds are critical since the database can't be check ( Paths not set ):
            # Compilation will failed consistently.
            # Or, the svn log/ svn up cmd failed.
            # Or, an error occured while storing the web report or svnChecker process informations.
                
            # Warn CoolTester Admin.
        
            sendAlertMail  
        
        rescue SvnChecker_Err_Timeout,CM_Err_AuthenticationFailed,CM_Err_ShellOpenFailed  => e
            # VPN down or Host down...
            # Or, opening the SSH connection/shell failed. 
            processException(e)
            sendAlertMail  
                 
        rescue Exception => e
            # An unknown exception has been raised
            # Report the problem to coolster admin.  
            processException(e)
            # Warn CoolTester Admin.
            sendAlertMail           
        ensure              
            begin
                storeLog(log_file)
            rescue SvnChecker_Err_File_StoreLog => e
                processException(e)
                # Warn CoolTester Admin.
                sendAlertMail     
            end
                  
        end # end begin rescue ensure SvnChecker.checkCodeRoutine exceptions.
 
    end # end SvnChecker.checkCodeRoutine method.

end # end SvnChecker module

    
   
        
