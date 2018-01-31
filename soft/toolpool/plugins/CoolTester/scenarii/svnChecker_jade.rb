# This file is the svnChecker script which sould be executed periodically.
# All svnChecker options are set here.
# Requires svnChecker.rb file.
# 
# Author::     Matthieu Ruiz  (mailto:matthieu.ruiz@gmail.com)
# Maintainer:: Benjamin Babut (mailto:benjamin@coolsand-tech.fr)
# Copyright::  2003-2008, Coolsand Technologies, Inc.

begin
require "CoolTester/scenarii/svnChecker.rb"
include SvnChecker
    
#####################################
#              Options              #
#####################################

# SvnChecker configuration options :

# Guy warn when a exception occures during the svnChecker process.
coolTesterAdmin="benjamin@coolsand-tech.fr"
# Others people who should be warn too.
coolPeopleToBeWarned="mathieu@coolsand-tech.fr"
# VPN timeout when excuting a cmd.
vpnTimeout=60*3
# Host timeout for compiling testHash.
hostTimeout=60*45
# Count of processes before sending a "Still Alive" mail to coolTesterAdmin.
# This class constant enables to check is the svnChecker is still running or not.
countProcessBeforeMail=48

configHash={:CoolTesterAdmin=>coolTesterAdmin,
            :CoolPeopleToBeWarned=>coolPeopleToBeWarned,
            :VpnTimeout=>vpnTimeout,
            :HostTimeout=>hostTimeout,
            :CountProcessBeforeMail=>countProcessBeforeMail
            }
            
# AUTHENTICATION : COOLTESTER
user= "cooltester"
passwd_file= "C:\\cooltester_passwd.txt"
            


    soft_workdir="/k/projects/Jade/cooltester/Jade/soft"
    
    # LOCAL MACHINE ( machine on which will be launch this script ) files informations :
    revisions_info_file="C:\\revisionsInfosJade.yaml"
    log_file="C:\\svnChecker_log_jade.dat"
    
    # Web report config :
    report_directory="N:/users/cooltester/public_html/"
    report_subfolder="svnChecker_reports_jade"
    report_prefix="svnChecker_report"
    report_latest="svnChecker_latest_report.php"
    
    # HOST on which will be established the SSH connection ( Thus, the compilation will be run on this machine)
    host="medoc"
    

begin
    load "CoolTester/scenarii/svnChecker_testlist_jade.rb"
    testHash=getTestHashJade(soft_workdir)
end
    
    # ####################################
    #              Process               #
    # ####################################
    
    # Config SvnCkecker.    
    SvnChecker.config(configHash)
    # Run the process with required options.
    SvnChecker.checkCodeRoutine( host, user, "JADE", passwd_file, revisions_info_file, report_directory, report_subfolder, report_prefix,report_latest, log_file, soft_workdir, testHash )

end 