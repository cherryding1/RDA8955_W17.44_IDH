#!/usr/bin/perl -s

BEGIN {
	unless ($^O eq "solaris") {
			require warnings;
			import warnings qw(all);
	}
}

use strict;
use CT::ctio;

my $term= new CT::ctio ("NewTest.pl");
my $prompt = "> ";

######### Help ##################################
if ($main::h) {
		print <<"ENDHELP";
NewModule.pl : 
creates (if necessary) a new module directory structure, and integrate
it in the new environment, writing a custom Makefile and Readme.
   
To use, call it in the directory where you want to create a new 
module (e.g. phy/) and give as name the name of the 
directory to create. If that directory already exists, it will be
kept, and only the Makefile and Readme will be overwritten.
ENDHELP
		exit $main::h;
}

######### Set Up name and path ################## 
my $name;
if (defined $ARGV[0]) {
		$name = $ARGV[0];
}
else {
	$term->display("Enter the name of your module");
	$name = $term->readline($prompt);
	if (not $name) {
			$term->error("Must have a name!\n");
	}
}

(my $local_name = $ENV{'PWD'}) =~ s:^$ENV{'SOFT_WORKDIR'}/::;
$local_name .= "/$name";

######### Create Directory ######################
if ( -d $name ) {
		$term->confirmAbort("****","Directory already exists, continue ? (y/n)","****");
}
else {
		mkdir $name,0777 or die "Could not create directory:\n$!";
}

foreach (("src","include")) {
		-d "$name/$_" or (mkdir "$name/$_",0777 or die "Could not create directory $name/$_:\n$!");
}

chmod 0664, "$name/Makefile" if ( -f "$name/Makefile" );
open OUTFILE, ">$name/Makefile" or die "Could not open Makefile for writing:\n$!";

######### Set Up Makefile variables #############
my $local_depapi =  $term->askWithDefault("Enter modules to include as header files :\n(you probably want \"phy stack\" at least)", "none", "");

### C files magic ###
my $c_files = $term->askWithDefault("Enter list of C files in your test :\n['extern' for external make.srcs ]","all .c files", "\${notdir \${wildcard src/*.c}}");
my $extern_make;
$c_files =~ /^extern$/  and $extern_make="include make.srcs" and $c_files="";

### S files ###
my $s_files;
if (not $extern_make) {
		$s_files = $term->askWithDefault("Enter list of S files in your test :","all .S files", "\${notdir \${wildcard *.S}}");
}


### Misc flags ###
my $mips16_cflag = $term->askYesNo ("Support mips16 code ?", "y", "\${CT_MIPS16}", "");

######### Write to Makefile #####################
		print OUTFILE <<"ENDMK";
\#\# --------------Don't change the line below------------------ \#\#
\# For more information on the different variables, and others you
\# may define, see \${SOFT_WORKDIR}/Makefile.skel and/or read the doc
include \${SOFT_WORKDIR}/make.toplevel

LOCAL_NAME := $local_name

LOCAL_API_DEPENDS ?= $local_depapi

CT_BOARD ?= fpga
CT_WATCH_REG ?= yes
CT_ROMULATOR ?= y
CT_MIPS16 ?= -mips16
CT_MIPS16_CFLAGS ?= $mips16_cflag

C_SRC := $c_files
S_SRC := $s_files
$extern_make

\#\# --------------Don't change the line below------------------ \#\#
include \${SOFT_WORKDIR}/compile.rules
ENDMK
close OUTFILE;

$term->display( "\n\#\#\# Makefile Generated \#\#\#");

######### Write Readme ##########################
my $lines = $term->readMultiline("Now enter a short description of your test");
if (open OUTFILE, ">$name/Readme_$name.txt") {
		$_ = `date "+%Y/%m/%d"`;
		chomp;
		print OUTFILE qq(### $name test, created by $ENV{'USER'} on $_ ###\n);
		print OUTFILE @{$lines};
		close OUTFILE;
		$term->display("\#\#\# Readme Generated \#\#\#");
}
else {
		warn "Could not write $name/Readme_$name.txt. Sorry\n";
}

############## END MAIN SCRIPT ############################

