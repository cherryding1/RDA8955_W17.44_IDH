#!/usr/bin/perl -s

BEGIN {
	unless ($^O eq "solaris") {
			require warnings;
			import warnings qw(all);
	}
}
use strict;

if ($main::h) {
		print <<"ENDHELP";
		Remove all files that are not commited to svn in <arg>/include, 
		where <arg> is the first argument if present, or \$PWD if not.
		
		Use : clean up the top-level include directory, removing the 
		headers copied from submodules.
ENDHELP
		exit $main::h;
}

my $topdir;
my $param;

if (defined ($param = shift))
{
    $topdir = "$ENV{SOFT_WORKDIR}/$param";
}
else
{
    $topdir = $ENV{'PWD'};
}

my @unknownlist;
my @rmfilelist;
my @statuslist;

@statuslist = `svn status $topdir/include`;

foreach my $line (<@statuslist>) {
	push @unknownlist, $line;
}

foreach my $file (<$topdir/include/*>) {
	if (-f $file) { 
		push @rmfilelist, $file
			if grep /^$file$/, @unknownlist;
	}
}

print "Cleaning up... \n";
unlink @rmfilelist;
print "Done !\n";

