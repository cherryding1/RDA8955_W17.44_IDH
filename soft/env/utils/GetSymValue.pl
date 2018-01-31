#!/usr/bin/perl -s

use strict;

if ($main::h) {
print <<"END_HELP";
GetSymValue.pl : Get a symbol's value from different .map files
                 and pretty-print all those values

   Usage: GetSymValue.pl [options] <symbol> [<map files>]
    <symbol> is the name of the symbol to match
    <map files> is a space-separated list of map files
       If not specified, all .map files will be used.

   Options: 
    -romu : use only files ending in "romulator.map"
    -noromu : use only files not ending in "romulator.map"
END_HELP
exit 1;
}


my $sym = shift;

# pretty print and filetype options...
my $max= 0;
my @list = ();

unless (defined $ARGV[0]) {
	push @ARGV, <*.map>;
}

foreach (@ARGV) {
	next if ($main::romu and not /romulator.map$/);
	next if ($main::noromu and /romulator.map$/);
	push @list, $_;
	$max = length $_ if (length $_ > $max);
}

# parse final list
foreach (@list) {
	doFile($_);
}

sub doFile {
	my $file = shift;
	my $symval = 0;
	open IN, $file or die "Could not open $file";

  READ:
	while (<IN>) {
		/(0x[\da-f]+)\s*$sym\s/o and $symval = hex($1) and last READ;
	}

	close IN;

	if (not $symval) {
		print "\t ** Could not find value for symbol $sym in $file **\n";
	}
	else {
		printf "%-${max}s : symbol %s at : 0x%x\n", $file, $sym, $symval;
	}
}
