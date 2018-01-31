#!/usr/bin/perl -s

use strict;

if ($main::h) {
print <<"END_HELP";
GetSectSize.pl : Get a section's size from different elf binaries
                 and pretty-print all those values

   Usage: GetSectSize.pl [options] <section> [<elf files>]
    <section> is the name of the section to match
    <elf files> is a space-separated list of elf files
      If none is specified, all binary files not ending in .srec
	  will be processed.

   Options: 
    -romu : use only files ending in "romulator"
    -noromu : use only files not ending in "romulator"
END_HELP
exit 1;
}


my $sect = shift;
$sect =~ /^\./ or $sect =".".$sect;

# pretty print and filetype options...
my $max= 0;
my @list = ();

if (not defined $ARGV[0]) {
	foreach (<*>) {
		-B $_ and not -d $_ and not /\.srec$/ and push @ARGV, $_;
	}
}

foreach (@ARGV) {
	next if ($main::romu and not /romulator$/);
	next if ($main::noromu and /romulator$/);
	push @list, $_;
	$max = length $_ if (length $_ > $max);
}

# parse final list
foreach (@list) {
	doFile($_);
}

sub doFile {
	my $file = shift;
	my $size = -1;
	open IN, "mips-elf-readelf -S $file|" or die "Could not read $file with mips-elf-readelf";
	

  READ:
	while (<IN>) {
		/\s+$sect\s+\w+\s+[\da-f]+\s+[\da-f]+\s+([\da-f]+)/o and $size = hex($1) and last READ;
	}

	close IN;

	if ($size == -1) {
		print "\t ** Could not find size for section $sect in $file **\n";
	}
	else {
		printf "%-${max}s : section %s : 0x%x\n", $file, $sect, $size;
	}
}
