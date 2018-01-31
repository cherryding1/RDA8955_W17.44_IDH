#!/usr/bin/perl
# Note : this script uses the BSD-like glob behavior, which became default
# with perl 5.6.0
# It won't work with an older perl (if needed, try bsd_glob from File::Glob)

use strict;
use warnings;
use File::Copy;
# for rel2abs...
use File::Spec::Functions;
use Getopt::Long;

# options
my $o_debug;
my $o_verbose;
my $o_getopt;

eval {
    $o_getopt = GetOptions ("verbose+" => \$o_verbose,
                            "debug"    => sub { $o_verbose = 2 },
                            "quiet"    => sub { $o_verbose = -1 },
                            "help"     => sub { printHelp(); exit 0}
                           )
};
if ($@ or not $o_getopt) {
    print STDERR $@,"\n";
    printHelp();
    exit 1;
}

#########################

unless (defined $ARGV[1]) {
	printHelp();
	die "Wrong command line, aborted\n";
}

unless (defined $ENV{'SOFT_WORKDIR'}) {
	die "SOFT_WORKDIR variable not defined, aborted\n";
}

#########################

# hash table for version numbers
my %versions = ();

my $source_path = chompSlash(shift);
my $config_file = shift;
my $stack_dir = chompSlash($ENV{'SOFT_WORKDIR'})."/stack";

# convert relative path to absolute ones if needed
$source_path = File::Spec->rel2abs ($source_path) 
	unless File::Spec->file_name_is_absolute ($source_path);
$stack_dir = File::Spec->rel2abs ($stack_dir)
	unless File::Spec->file_name_is_absolute ($stack_dir);

# max length of a module name, so we can print them properly aligned, achtung :)
my $maxlen = 0;
#########################

# parse version config file, build version hash
# expected syntax is "<directory> : <version>", e.g. "api : 1.01"
open IN, "<", "$config_file" 
	or die "Could not open configuration file $config_file : error is : $!\n";

PARSELOOP:
while (<IN>) {
	if (/^#/ or /^PALDIR/) {
		next PARSELOOP;
	}
	if (/^[A-Z].+\/(\w+)\/(v\d\.\d+)/) {
		$versions{$1} = $2;
		$maxlen = length $1 if (length $1 > $maxlen);
	}
}
close IN;

#########################

# main copy loop
foreach my $dir (sort keys %versions) {
	printf "Copy from %-${maxlen}s with version %s\n", $dir, $versions{$dir} 
		if ($o_verbose >= 0);
	doModule($dir);
}

print "Copy complete\n" if ($o_verbose >= 0);


##################################################################################
#                      End main script                                           #
##################################################################################

# print short help message
sub printHelp {
	print << "ENDHELP";
ImportStackSource.pl : integrate Stackcom source into new env directory
     Usage : ImportStackSource.pl [options] <source path> <config file>
         <source path> is the path to the original, Stackcom-style source tree
         <config file> is the path to a configuration file defining which version to
                       integrate for each module.
         \$SOFT_WORKDIR (environment variable) is used to define the destination
     Options :
         -h : display this help and exit
         -v : more verbose output
         -q : quiet mode (no output except errors/warnings)
         -debug : debug mode, very noisy output
         
ENDHELP
}

# remove trailing slashes from path variables
sub chompSlash {
		my $path = shift;
		$path =~ s:/$::;
		return $path;
}

# Called on each module (layer), calculates source and dest path 
# and calls copy sub on src, inc and tst subdirs
sub doModule {
	my $mod = shift;

	my $srcpath = "$source_path/$mod/$versions{$mod}";
	my $destpath = "$stack_dir/$mod";

	if (not -d $srcpath) {
		die "Could not find a $versions{$mod} directory for $mod, aborting\n";
	}
	mkdir $destpath, 0775 unless ( -d $destpath);

	doCopy("$srcpath/src","$destpath/src");
	doCopy("$srcpath/inc","$destpath/include");
	doCopy("$srcpath/tst","$destpath/tst");
	
}

# Do the actual copying from src to dest
# Note : we use File::Copy instead of system calls to "cp -rf", even though the latter 
# might be faster if there are many files, because a system call makes it hard to 
# distinguish source files, and is unreliable (e.g. if there are spaces in dir names)
sub doCopy {
	my ($src, $dest) = @_;
	my (@destlist, @srclist); 
	
	# to print debug lists
	local $, = " ";

	# if the dest directory already exists, we need to remove 
	# all source files from it before we copy, so there are no
	# leftovers (would mess up ${wildcard *.c} makefiles)
	if ( -d $dest ) {
		printf "Destination directory $dest exists, cleaning up...\n"
			if ($o_verbose > 0);

		@destlist = <$dest/*.{h,c,S,hp}>;
		foreach (@destlist) {
			unlink $_ or warn "Could not remove $_ : error is : $!\n";
		}

		print "\tFiles removed from $dest:\n", 
				(map {s:.*/::; $_} @destlist),"\n\n" 
			if ($o_verbose > 1);
	}
	else {
		printf "Creating destination directory $dest\n"
			if ($o_verbose > 0);
		mkdir $dest, 0775;
	}

	# copy the new files

	@srclist = <$src/*.{h,c,S,hp}>;
	foreach (@srclist) {
		copy ($_,$dest) or warn "Could not copy $_ : error is $!\n";
	}

	print "\tFiles copied from $src:\n",
			(map {s:.*/::; $_} @srclist),"\n\n" 
		if ($o_verbose > 1);
}

