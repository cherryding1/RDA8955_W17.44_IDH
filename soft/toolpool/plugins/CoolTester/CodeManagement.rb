require 'rubygems'
require 'net/ssh'	

begin
require 'help'
rescue LoadError
end


module CodeManagement
    
    #####################################
	#			Private section			#
	#####################################
    
    private
    
        
    #####################################
	#			 Error Classes          #
	#####################################
   
    # Virtual CodeManagement class error.
	class CM_Err < StandardError
	end
    
    class CM_Err_AuthenticationFailed < CM_Err
    end
    
    class CM_Err_ShellOpenFailed < CM_Err
    end
    
    # Virtual CodeManagement class error.
    class CM_Err_CmdFailed < CM_Err    
    end
      
    class CM_Err_SvnLogFailed < CM_Err   
    end
    
    class CM_Err_SvnUpdateFailed < CM_Err   
    end
    
    class CM_Err_SvnExternalsFailed < CM_Err   
    end

    class CM_Err_MakeFailed < CM_Err
    end
    
    # This exception is raised if the make process can't start correctly :
    # - Unknown rule to make target.
    # - Invalid option.
    # - Unable to connect to user@host.
    # etc...
    class CM_Err_MakeStartFailed < CM_Err_MakeFailed
    end
    
    # This exception is raised when the make process started but an error occured
    # in the compilation / link step.
    class CM_Err_MakeCompilationOrLinkFailed < CM_Err_MakeFailed
    end
    
    #####################################
	#			 Other Classes          #
	#####################################
    
    class Commit
        attr_reader :user, :revision, :changed_paths, :time
        # Example of oneCommitString parsing :
=begin
        ------------------------------------------------------------------------
        r8435 | mruiz | 2007-12-10 10:14:23 +0100 (Mon, 10 Dec 2007) | 1 line
        Changed paths:
            M /modem2G/trunk/toolpool/plugins/CoolTester/CodeManagement.rb

        misc improvements in CodeManagement.rb 
        ------------------------------------------------------------------------
=end
        
        def initialize (oneCommitString)
            # Get revision number.
            @revision=oneCommitString.match(/r\d*/).to_s.gsub("r","").to_s
            # Get the user who did this commit ex : | mruiz |:
            @user=oneCommitString.match(/[|] [a-z]+ [|]/).to_s.gsub(/[|] | [|]/,"")
            # Get the commit time
            @time=Time.parse(oneCommitString.sub( /.*[|].*[|] (.*) [|] .*/, '\1'))
            @changed_paths=oneCommitString
        end
    
    end
    public
    def CodeManagement.checkLogCmdStatus( output )
        commitStrArray=[]
        commitObjArray=[]
        if output.status!=0
            raise CM_Err_CmdFailed, output.stderr, caller
        else
            commitStrArray=output.stdout.split("------------------------------------------------------------------------")
            commitStrArray.delete_at(0)
            commitStrArray.pop
            commitStrArray.each do |oneCommitString|
                commitObjArray << Commit.new(oneCommitString)
            end
           
            return commitObjArray,output.stdout
        end
    end
    
    
    # Check the output cmd.
    # If an error occured, raise a CM_Err_CmdFailed exception with stderr.
    # Otherwise, print the stdout.
    def CodeManagement.checkCommonCmdStatus( output )
        if output.status!=0
            raise CM_Err_CmdFailed, output.stderr, caller
        else
            
            return output.stdout
        end
    end
    
    def CodeManagement.checkMakeCmdStatus( output )
        if output.status!=0
            # The make cmd failed. Let's investigate more precisely why, parsing stderr :
            case (output.stderr)
                # Rule unknown : raise a classic CM_Err_CmdFailed.
                when / No rule to make target /
                    raise CM_Err_CmdFailed, output.stderr, caller
                # Invalid option : raise a classic CM_Err_CmdFailed.
                when / invalid option /
                    raise CM_Err_CmdFailed, output.stderr, caller
                # We assume it's a compilation/link error.
                # Raise a more specific error : CM_Err_MakeCompilationOrLinkFailed.
                else
                    raise CM_Err_MakeCompilationOrLinkFailed, output.stderr, caller
                
            end            
        else
            
            return output.stdout
        end
    end
    
    
    
    def CodeManagement.checkUpdateCmdStatus( output )
        if output.status!=0
            raise CM_Err_CmdFailed, output.stderr, caller
        else
            # Get HEAD=BASE revision number.
            headAndBaseRevision=output.stdout.match(/revision \d*/).to_s.match(/\d+/).to_s
            return headAndBaseRevision,output.stdout
        end
    end
    

    def CodeManagement.checkGetWSExternalsCmdStatus( output )
        if output.status!=0
            raise CM_Err_CmdFailed, output.stderr, caller
        else
            externaldir=[]
            output.stdout.each_line do |l|
                l.strip!
                case l
                #matching relative external only
                when /^\^\/[^ ]*[ \t]*([^ ]*)$/
                    #puts "found external #{$1}"
                    externaldir.push($1)
                when /^[ \t]*$/
                    # skip empty lines
                else
                    #puts "svn:externals line '#{l}'"
                    raise CM_Err_SvnExternalsFailed, output.stderr, caller
                end
            end
            return externaldir
        end
    end
    
    #####################################
	#			Public section			#
	#####################################
    
    #public
    
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.sendCommonCmd",
                        "shell, cmd , params=nil",
                        "stdout",
                        "Execute the given cmd with the specified params. Print the ouptut of the cmd. Could raise an CM_Err_CmdFailed exception with stderr in $!.")   
    
    end
    
    def CodeManagement.sendCommonCmd ( shell, cmd , params=nil )
        output=shell.send_command cmd, params
        return CodeManagement.checkCommonCmdStatus( output )    
    end
             
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.openSyncShell",
                        "host, user, passwd",
                        "",
                        "Open a shell ( bash ) in the session in a SYNCHRONOUS way. Thus, we can execute a cmd and wait until the end of its execution. By this way we can easily get the output send on stdout.So, this shell is a NO interactive one ! Send a shell object to the given block where you can execute commands. Could raise a CM_Err_ShellOpenFailed or a CM_Err_AuthenticationFailed exceptions with informations in $!")   
    
    end
      
    def CodeManagement.openSyncShell( host, user, passwd )
        
        begin
            Net::SSH.start( host, user, passwd ) do |session|
                
                shell = session.shell.sync()
                
                # Export the PS1 variable with default value when opening an interactive
                # shell...
                CodeManagement.sendCommonCmd( shell, "export PS1='{debian_chroot:+($debian_chroot)}\u@\h:\w\$'")
                
                # Source again .bashrc with $PS1 defined.
                CodeManagement.sendCommonCmd( shell, "source .bashrc")
                                
                # Call the block and give it the object #shell as a parameter.
                yield shell
                
                # Close the shell and as a result the session.
                CodeManagement.sendCommonCmd( shell, "exit")
                
            end
        
        rescue Errno::ECONNREFUSED
            raise CM_Err_ShellOpenFailed, "Connection refused",caller
        end 
                
        rescue Net::SSH::AuthenticationFailed 
            raise CM_Err_AuthenticationFailed, "Authentication failed : #{$!}",caller        
    
    end
               
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.svnLog",
                        "shell, path, username='', fromRevision='BASE',toRevision='HEAD'",
                        "revisionNumber, userWhoDidIt, 'svn log' stdout",
                        "Svn log informations at given revision with the verbose option about given path directory. Print the ouptut of the 'svn log' command. Could raise an CM_Err_SvnLogFailed exception with stderr in $!.")   
    
    end
    
    def CodeManagement.svnLog(shell, path, username="", fromRevision="BASE",toRevision="HEAD")
        
        begin
        
        usernameOption=""
        usernameOption="--username "+username if username!=""
                      
        # Get log informations about the specified revision.
        output=shell.send_command("svn log #{path} #{usernameOption} -r #{fromRevision}:#{toRevision} -v", nil)
        # parse log and return it in ruby format
        return CodeManagement.checkLogCmdStatus(output) 
                                          
        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            puts exception    
            # Raise a CM_Err_SvnLogFailed to the upper caller.
            raise CM_Err_SvnLogFailed, "#{$!}", caller     
        end
        
    end
      
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.svnUpdate",
                        "shell, revision='HEAD', username='', files=''",
                        "HEADandBASErevision,stdout",
                        "Update the current path or the specified files at revision. Print the ouptut of the 'svn up' command. Could raise an CM_Err_SvnUpdateFailed exception with stderr in $!.")   
    
    end
 
    def CodeManagement.svnUpdate( shell , revision= "HEAD", username="", files="")
        
        begin
        
        usernameOption=""
        usernameOption="--username "+username if username!=""
                                   
        # Update the directory or the specified files at the given revision.
        output=shell.send_command("svn update --non-interactive --accept theirs-full #{usernameOption} -r #{revision} #{files}", nil)
        # parse output and return it in ruby format
        return CodeManagement.checkUpdateCmdStatus(output) 
                                             
        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            puts exception    
            # Raise a CM_Err_SvnUpdateFailed to the upper caller.
            raise CM_Err_SvnUpdateFailed, "#{$!}", caller     
        end
        
    end
    
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.make",
                        "shell, options=''",
                        "'make stdout'",
                        "Execute the makefile in the current path with options. Could raise two kind of exceptions : CM_Err_MakeStartFailed or CM_Err_MakeCompilationOrLinkFailed with stderr in $! .")   
    
    end
    
    def CodeManagement.make( shell, options="")
    
        begin
                        
        # Execute the Makefile with the given option.
        output=shell.send_command("make #{options}", nil)
        # parse output and return it in ruby format
        return CodeManagement.checkMakeCmdStatus( output )   
                   
        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            if(exception.to_s().length()<10000)
                puts exception 
            else
                puts "Exception too long to be printed"
            end
            
            # We check if it is a compilation/link error.
            if exception.class ==  CM_Err_MakeCompilationOrLinkFailed
                # We propagate this exception to the upper caller, to identify this kind of error.
                raise 
            else
                # Raise a CM_Err_MakeStartFailed to the upper caller.
                raise CM_Err_MakeStartFailed, "#{$!}", caller   
            end  
        end
          
    end
   
    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.svnGetWSExternals",
                        "shell, path, username=''",
                        "list of directories",
                        "Svn Propget svn:externals parsed to get relative externals as used for defining a workspace. Could raise an CM_Err_SvnExternalsFailed exception with stderr in $!.")   
    
    end
    
    def CodeManagement.svnGetWSExternals(shell, path, username="")
        
        begin
        
        usernameOption=""
        usernameOption="--username "+username if username!=""
                      
        # Get log informations about the specified revision.
        output=shell.send_command("svn propget svn:externals #{path} #{usernameOption}", nil)
        # parse log and return it in ruby format
        return CodeManagement.checkGetWSExternalsCmdStatus(output) 
                                          
        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            puts exception    
            # Raise a CM_Err_SvnExternalsFailed to the upper caller.
            raise CM_Err_SvnExternalsFailed, "#{$!}", caller     
        end
        
    end

    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.svnLogWS",
                        "shell, path, username='', fromRevision='BASE',toRevision='HEAD'",
                        "revisionNumber, userWhoDidIt, 'svn log' stdout",
                        "Svn log informations at given revision with the verbose option about workspace given by its path. Print the ouptut of the 'svn log' command. Could raise an CM_Err_SvnLogFailed exception with stderr in $!.")   
    
    end
    
    def CodeManagement.svnLogWS(shell, path, username="", fromRevision="BASE",toRevision="HEAD")
        
        begin
            # get externals
            dirs = CodeManagement.svnGetWSExternals(shell, path, username)

            commitObjArray=[]
            outstring=""
            # get each sub path log
            dirs.each do |d|
                subpath=path + '/' + d
                commits,out = CodeManagement.svnLog(shell, subpath, username, fromRevision, toRevision)
                commitObjArray.concat(commits)
                outstring.concat("# svn log #{subpath}\n")
                outstring.concat(out)
            end
            # get workspace log
            commits,out = CodeManagement.svnLog(shell, path, username, fromRevision, toRevision)
            commitObjArray.concat(commits)
            outstring.concat("# svn log #{path}\n")
            outstring.concat(out)

            return commitObjArray,outstring
                                          
        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            puts exception    
            # Raise the same exception to the upper caller.
            raise exception, "#{$!}", caller     
        end
        
    end

    if defined?(addHelpEntry)
        addHelpEntry(   "CodeManagement",
                        "CodeManagement.svnUpdateWS",
                        "shell, revision='HEAD', username='', path=''",
                        "HEADandBASErevision,stdout",
                        "Update the workspace at the current path or the specified path to revision. Print the ouptut of the 'svn up' command. Could raise an CM_Err_SvnUpdateFailed exception with stderr in $!.")   
    
    end
 
    # using CodeManagement.svnUpdate on a workspace will get the
    def CodeManagement.svnUpdateWS( shell , revision= "HEAD", username="", path="")
        
        begin
        
            # get externals
            dirs = CodeManagement.svnGetWSExternals(shell, path, username)

            outstring=""
            # update each sub path
            dirs.each do |d|
                subpath=path + '/' + d
                headAndBaseRevision,out = CodeManagement.svnUpdate(shell, revision, username, subpath)
                outstring.concat("# svn update #{subpath}\n")
                outstring.concat(out)
            end
            # update workspace ignoring externals
            headAndBaseRevision,out = CodeManagement.svnUpdate(shell, revision, username, path+" --ignore-externals")
            outstring.concat("# svn update #{path}\n")
            outstring.concat(out)

            return headAndBaseRevision,outstring

        # Rescue all exceptions which inherits from CM_Err.
        rescue CM_Err => exception
            # Print the exception.
            puts exception    
            # Raise a CM_Err_SvnUpdateFailed to the upper caller.
            raise CM_Err_SvnUpdateFailed, "#{$!}", caller     
        end
        
    end

end
