# This file is the svnChecker script which sould be executed periodically.
# All svnChecker options are set here.
# Requires svnChecker.rb file.
# 
# Author::     Benjamin Babut
# Maintainer:: Benjamin Babut (mailto:benjamin@coolsand-tech.fr)
# Copyright::  2003-2008, Coolsand Technologies, Inc.

begin
puts "Loading SVN Checker..."
require "CoolTester/scenarii/svnChecker.rb"
include SvnChecker
puts "Loading postbuild libs ..."
require "CoolTester/scenarii/svncheckerpostbuild/proto.rb"

#####################################
#          Test specific stuff      #
#####################################

$FROM_REV = 'HEAD'
$TO_REV = 'HEAD'  

=begin
def testFunction(rev)
    return true if( rev < ($FROM_REV + ($TO_REV-$FROM_REV)*2/3)  )
    return false
end
=end

def testFunction(options)
  return SVNCheckerPostBuild.protoCheck(options)
end
#####################################
#              Options              #
#####################################

# SvnChecker configuration options :

# HOST on which will be established the SSH connection ( Thus, the compilation will be run on this machine)
host="medoc"
# VPN timeout when excuting a cmd.
vpnTimeout=60*10
# Host timeout for compiling testHash.
hostTimeout=60*45

configHash={:VpnTimeout=>vpnTimeout,
            :HostTimeout=>hostTimeout}
            
# AUTHENTICATION : COOLTESTER
user= "cooltester"
passwd_file= "C:\\cooltester_passwd.txt"

# Unix cooltester account informations :   
soft_workdir="/n/projects/Granite/cooltester_tests/Granite/soft"

flash_programmer = "C:/CSDTK/cooltools/chipgen/Modem2G_aint_dev/toolpool/plugins/fastpf/flash_programmers/s71ws256nc0_ramrun.lod"

from_revision = $FROM_REV
to_revision = $TO_REV   

num = "888"

test_path="#{soft_workdir}/platform_test/stack/proto"
test_hash = { "mkopts" => "CT_TARGET=greenstone_eng_ct1010  FLSH_MODEL=flsh_s71ws256nc0 CT_ROMULATOR=no CT_RELEASE=cool_profile CT_USER=ADMIN COOLTESTER=1", "flashprogrammer" => flash_programmer , "num" => num, "postbuild" => Proc.new { |opts| testFunction(opts) } }
   
# ####################################
#              Process               #
# ####################################

SvnChecker.config(configHash)
SvnChecker.dichotomyCheck( host, user, passwd_file, soft_workdir, test_path, test_hash, from_revision, to_revision )


end 