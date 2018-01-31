#This script will construct a minimal public toolpool to release to the client.

require "fileutils"
require "find"


def mkPublicPoolDir(subdir)
    Dir.mkdir("../../Modem2GEnterprise/toolpool/"+subdir)
end

def copyFile(fromdir,file)
    FileUtils.cp(fromdir+"/"+file,"../../Modem2GEnterprise/toolpool/"+fromdir)
end

def copyDir(dirfrom,dirname)
    FileUtils.cp_r(dirfrom+"/"+dirname,"../../Modem2GEnterprise/toolpool/"+dirfrom)
end

def removeSvnEntries(rootfolder)
    rootfolder = File.expand_path(rootfolder)
    Find.find(rootfolder) do |path|
        if FileTest.directory?(path)
            if(File.basename(path) == ".svn")
                FileUtils.rm_rf(path)
                Find.prune
            elsif File.basename(path)[0] == ?.
                Find.prune       # Don't look any further into this directory.
            else
                next
            end
        end
    end
end

torestore = File.expand_path(Dir.pwd)
scriptdir = File.expand_path(File.dirname(__FILE__))

begin
    Dir.chdir(scriptdir)
    
    FileUtils.rm_rf("../../Modem2GEnterprise")
    Dir.mkdir("../../Modem2GEnterprise")
    mkPublicPoolDir("")
    mkPublicPoolDir("map")
    mkPublicPoolDir("map/ChipStd")
    mkPublicPoolDir("map/Greenstone")
    mkPublicPoolDir("scripts")
    
    mkPublicPoolDir("plugins")
    mkPublicPoolDir("plugins/Calib")
    mkPublicPoolDir("plugins/fastpf")
    mkPublicPoolDir("plugins/cwmodem2ggui")
    
    mkPublicPoolDir("scripts/ChipStd")
    mkPublicPoolDir("scripts/Granite")
    mkPublicPoolDir("scripts/Greenstone")
    
    copyFile("scripts/ChipStd","breakpoint.rb")
    copyFile("scripts/ChipStd","base.rb")
    copyFile("scripts/ChipStd","opal.rb")
#    copyFile("scripts/ChipStd","calibauto.rb")
    copyFile("scripts/ChipStd","chip_control.rb")
    copyFile("scripts/ChipStd","eventhandlers.rb")
    copyFile("scripts/ChipStd","ramrun.rb")
    copyFile("scripts/ChipStd","target_executor.rb")
    copyFile("scripts/ChipStd","versions.rb")
    
#    copyFile("scripts/Granite","calib.rb")
    copyFile("scripts/Granite","target_executor.rb")
    
    copyFile("scripts/Greenstone","target_executor.rb")
    
    copyFile("map/ChipStd","traceDb.yaml")
    copyFile("map/Greenstone","greenstone_hard_minimal.xml")
    copyFile("map/Greenstone","greenstone_soft_pkg.xmd")
    
    copyDir("plugins","Calib")
    copyDir("plugins","fastpf")
    copyFile("plugins/cwmodem2ggui","plugin.rb")
    copyFile("plugins/cwmodem2ggui","cwmodem2ggui.rb")
    copyFile("plugins/cwmodem2ggui","m2gevents.rb")
    
    removeSvnEntries("../../Modem2GEnterprise")
ensure
    Dir.chdir(torestore)
end