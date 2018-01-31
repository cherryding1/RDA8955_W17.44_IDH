#!/usr/bin/ruby
# Functions for accessing subversion.
# See module SVN for documentation.
require 'rexml/document'
require 'time'

module SVN

    # Class for subversion exceptions.
    class SvnError < StandardError
    end

    # Class for subversion exceptions: the working directory is invalid.
    class SvnNotAWorkdir < SvnError
    end

    # Class for subversion exceptions: the branch does not come from a single revision copy.
    class SvnNotATag < SvnError
    end

    LOG_DELIMITER = '------------------------------------------------------------------------'

    # Class for subversion log
    class Commit
        attr_reader :user, :revision, :text, :time
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
            @text=oneCommitString
        end
    
        def to_s
            @text
        end
    end


    # in general parameter path can be a path or an URL as on svn command line

    # Returns url, repositoryUrl, revision, lastCommitRevision of the given
    # path (or URL).
    def SVN::info(path="")
        ENV['LANG']='C'
        svnresult=`svn info --xml #{path}`
        if $? != 0 then
            raise SvnNotAWorkdir
        end
        doc = REXML::Document.new(svnresult)
        entry=doc.root.elements['entry']
        return entry.elements['url'].text, entry.elements['repository'].elements['root'].text, entry.attributes['revision'].to_i, entry.elements['commit'].attributes['revision'].to_i
    end

    # Returns the output of a quiet svn status of path.
    def SVN::uncommittedChanges(path="")
        ENV['LANG']='C'
        svnresult=`svn st -q #{path}`
        if $? != 0 then
            raise SvnNotAWorkdir
        end
        svnresult
    end

    # Return an Array of strings listing the entries in path (or URL).
    def SVN::list(path="", hideError=false)
        ENV['LANG']='C'
        #TODO: use open3 and set stderr to raise parameter string.
        if hideError then
            err=" 2>/dev/null"
        else
            err=""
        end
        svnliststr=`svn ls #{path}#{err}`
        if $? != 0 then
            raise SvnError
        end

        # svnlist has the list of entries in path.
        return svnliststr.split("\n")
    end

    # Return an Array of integers that list releases by numbers from an output of SVN::list.
    def SVN::extractReleases(list, prefix)
        l=list.collect {|r| 
            if r=~/^#{prefix}(\d)\/$/ then
                $1.to_i
            else
                nil
            end
        }
        l.delete(nil)
        return l
    end

    # Return an Array of integers that list releases by numbers from an URL.
    def SVN::listReleases(url, prefix, hideError=false)
        return SVN::extractReleases(SVN::list(url, hideError), prefix)
    end
    
    # Execute a copy.
    # Subversion output is not redirected.
    def SVN::copy(src,dst,msg=nil)
        ENV['LANG']='C'
        msgArg=" -m \"#{msg}\"" if msg
        system "svn cp #{src} #{dst}#{msgArg}"
        if $? != 0 then
            raise SvnError
        end
    end

    # Checkout an URL to a working directory.
    # Subversion output is not redirected.
    def SVN::checkout(src,dst='',args=nil)
        ENV['LANG']='C'
        extraArgs=" #{args}" if args
        system "svn co #{src} #{dst}#{extraArgs}"
        if $? != 0 then
            raise SvnError
        end
    end

    # Create a directory on path (or remotely on URL).
    # Subversion output is not redirected.
    def SVN::mkdir(path,args=nil)
        ENV['LANG']='C'
        extraArgs=" #{args}" if args
        system "svn mkdir #{path}#{extraArgs}"
        if $? != 0 then
            raise SvnError
        end
    end

    # Execute a commit from the path.
    # Subversion output is not redirected.
    def SVN::commit(path='',msg=nil)
        ENV['LANG']='C'
        msgArg=" -m \"#{msg}\"" if msg
        system "svn ci #{path}#{msgArg}"
        if $? != 0 then
            raise SvnError
        end
    end

    # Returns the property from path (or URL).
    def SVN::propget(prop,path='')
        ENV['LANG']='C'
        svnresult=`svn propget #{prop} #{path}`
        if $? != 0 then
            raise SvnError
        end
        return svnresult
    end

    # Execute a diff with given command line arguments.
    # Subversion output is not redirected.
    def SVN::diff(args)
        system "svn diff #{args}"
        if $? != 0 then
            raise SvnError
        end        
    end

    # Execute a status with given command line arguments.
    # Subversion output is not redirected.
    def SVN::status(args)
        system "svn st #{args}"
        if $? != 0 then
            raise SvnError
        end        
    end

    # Execute an update with given command line arguments.
    # Subversion output is not redirected.
    def SVN::update(args)
        system "svn up #{args}"
        if $? != 0 then
            raise SvnError
        end        
    end

    # Execute an revert with given command line arguments.
    # Subversion output is not redirected.
    def SVN::revert(args)
        system "svn revert #{args}"
        if $? != 0 then
            raise SvnError
        end        
    end

    # Find source and rev of a tag.
    def SVN::getTagSource(path)
        ENV['LANG']='C'
        svnresult=`svn log --stop-on-copy -v --xml #{path}`
        if $? != 0 then
            raise SvnNotAWorkdir
        end
        doc = REXML::Document.new(svnresult)
        tag = doc.root.each_element { |e| e }.last
        copyFromPath={}
        createdPath=[]
        lastRev=nil
        tag.elements['paths'].each_element { |p|
            # only added path
            raise SvnNotATag,"expected added path 'A' got '#{p.attributes['action']}'" unless p.attributes['action']=='A'
            path=p.text
            rev=p.attributes['copyfrom-rev']
            if rev then
                copyFromPath[path]=p.attributes['copyfrom-path']
                if lastRev then
                    # only one rev
                    raise SvnNotATag,"rev #{lastRev},#{rev}" unless lastRev == rev
                end
                lastRev=rev
            else
                createdPath.push(path)
            end
        }
        # only added path match some copied path (mkdir)
        createdPath.each { |p|
            found=false
            copyFromPath.keys.each { |k|
                if k =~ /^#{p}/ then
                    found=true
                    break
                end
            }
            raise SvnNotATag,'path missmatch' unless found
        }
        # compute common root
        lastSrcDir=nil
        copyFromPath.each_pair { |k,v|
            dest=k.split('/')
            src=v.split('/')
            while dest.last == src.last do
                dest.pop
                src.pop
            end
            srcDir=src.join('/')
            if lastSrcDir then
                raise SvnNotATag,'source path missmatch' unless lastSrcDir == srcDir
            end
            lastSrcDir=srcDir
        }

        return lastSrcDir,lastRev.to_i
    end

    # Save the log svn to the given file,
    # if no filename, returns the log
    def SVN::logToFile(path,args,filename)
        if filename then
            system "svn log #{args} #{path}>#{filename}"
            if $? != 0 then
                raise SvnError
            end
            return nil
        else
            ENV['LANG']='C'
            svnresult=`svn log #{args} #{path}`
            if $? != 0 then
                raise SvnError
            end
            return svnresult
        end
    end

    # Execute a switch with given command line arguments.
    # Subversion output is not redirected.
    def SVN::switch(args)
        cmd= "svn switch #{args}"
        system cmd
        if $? != 0 then
            raise SvnError,"command: '#{cmd}'"
        end        
    end

    # Execute a mergeinfo with given source
    # showRev can be either :merged or :eligible
    # Return the array of mergeinfo strings
    def SVN::mergeinfo(src,dst='',showRev=nil)
        ENV['LANG']='C'
        case showRev
        when :merged
            showRevArgs = " --show-revs merged"
        when :eligible
            showRevArgs = " --show-revs eligible"
        else
            showRevArgs = ""
        end
        svnresult=`svn mergeinfo #{src} #{dst}#{showRevArgs}`
        if $? != 0 then
            raise SvnError
        end
        return svnresult.split("\n")
    end

    # Execute a merge with given command line arguments.
    # Subversion output is not redirected.
    def SVN::merge(args)
        cmd= "svn merge #{args}"
        system cmd
        if $? != 0 then
            raise SvnError,"command: '#{cmd}'"
        end        
    end

    # parse a svn log
    # Return an array of SVN::Commit
    def SVN::parseLog( log )
        commitObjArray=[]
        commitStrArray=log.split(LOG_DELIMITER)
        commitStrArray.delete_at(0)
        commitStrArray.pop
        commitStrArray.each do |oneCommitString|
            commitObjArray << Commit.new(oneCommitString)
        end
       
        return commitObjArray
    end

    # Return an array of SVN::Commit containing svn log
    def SVN::log(path,args)
        return SVN::parseLog(SVN::logToFile(path,args,nil))
    end

end
