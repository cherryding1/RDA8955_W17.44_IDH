#!/usr/bin/perl

# Fsckin' Sun blows me...
BEGIN {
	unless ($^O eq "solaris") {
			require warnings;
			import warnings qw(all);
	}
}

use strict;
use Getopt::Long;
use File::Copy;

sub chompSlash ($);
sub tryFile ($);
sub doFile ($$);
sub doDir ($);
sub doIfDef ($$);
sub printHelpExit ($);

my $o_verbose = 0;
my $o_invert = 0;
my $o_list = '';
my $o_getopt = 0;

eval {
    $o_getopt = GetOptions ( "verbose+"     => \$o_verbose,
                             "invert"       => \$o_invert,
                             "list=s"       => \$o_list,
                             "help"         => sub {printHelpExit(0)}
                           );
};
if ($@ or not $o_getopt) {
    print STDERR $@,"\n";
    printHelpExit(1);
}


unless (defined $ARGV[1]) {
		die "Not enough arguments... Hey, I'm just a stoopid perl script, I can't guess everything for you !\n";
}

my $define = shift;
my @to_commit = (); # hold list of modified files

# actually do something
foreach (@ARGV) {
		chompSlash($_);
		-d $_ and doDir($_);
		-f $_ and tryFile($_);
}

# try to save list of modified files to "list" file, output to STDERR as last resort
if ($o_list and @to_commit) {
		if (open LIST, ">", "$o_list") {
			print "Outputting list of modified files to $o_list\n" if ($o_verbose);
			local $, = "\n";
			print LIST @to_commit;
			close LIST;
		}

		else {
			warn "Could not open $o_list : $!\nOutputing to STDERR instead...\n";
			local $, = "\n";
			print STDERR @to_commit;
			print STDERR "\n";
		}
}

###########################################################################################################
#                                      END MAIN SCRIPT                                                    #
###########################################################################################################

# Remove ugly-looking trailing slashes
sub chompSlash ($) {
		$_ = shift;
		s:/$::;
		return $_;
}

# Recursively parse a directory
sub doDir ($) {
		my $dir = shift;
		print "# Parsing directory $dir\n" if ($o_verbose > 1);

		foreach (<$dir/*>) {
			-d $_ and doDir($_) and next;
			-f $_ and /\.[chS]p?$/ and tryFile($_);
		}
}

# Read a file to look for matching ifdefs...
# Pass reference to list of read lines to doFile() if matched
sub tryFile ($) {
		my $filename  = shift;
		print " # Parsing file $filename\n" if ($o_verbose > 1);
		open IN, "<", "$filename" or die "Can't open input file $filename : $!";
		# when 900 000 lines you read, not so good your memory  will look :)
		my @lines = <IN>; 
		close IN;

		my $match = 0;

	LOOP:
		foreach (@lines) {
			if (/^\s*#ifn?def\s*$define/) {
				$match = 1;
			  last LOOP;
			}
		}

		if ($match) {
			doFile($filename,\@lines);
			push @to_commit, $filename;
		}		
}


# Parse a matching lines list, consuming the referenced list as it goes
sub doFile ($$) {
		my ($filename, $lines_ref) = @_;
        local *OUT;

        move($filename, "$filename.perlbkp") 
			or die "Could not move $filename : error $!\n";


		open OUT, "> $filename" or die "Can't open output file $filename for writing : $!";

		my $count = 0;

		while (defined ($_ = shift @{$lines_ref}) ) {
				$count++;

				if (/^#if(n?)def\s*$define/o) {
						printf "Matched if$1def at $filename, line $count\n" if ($o_verbose);
						doIfDef((not $1 xor $o_invert), $lines_ref);
				}

				else {
						print OUT ;
				}

		}

		close OUT;
		close IN;
		print "Done with $filename\n" if ($o_verbose);
}
		

# Parse the #ifn?def'ed part itself
	# $_[0] = 1 means #ifdef :  we print until #endif or #else
	# $_[0] = 0 means #ifndef : we don't print until #endif or #else
# Gets reference to partially consumed list of lines from doFile(), 
# consumes it itself up to the closing "endif"
sub doIfDef ($$) {
		my ($ok_print, $lines_ref) = @_;
		my $other_define = 0; # count nested defines
		my $count = 0;
		while (defined($_ = shift @{$lines_ref}) and  ($other_define > -1)) {
			$count++;

			if (/^\s*#\s*else/) {
				if ($other_define) { print OUT if ($ok_print); }
				else { $ok_print = not $ok_print; }
			}

			elsif (/^\s*#\s*(\S*if\S*)/) {
				if ($1 eq "endif") { 
					print OUT if ($other_define-- and $ok_print);
				}
				elsif ($1 =~ /^ifn?def/) {
					print OUT if ($ok_print); 
					$other_define++; 
				}
				else {
					print STDERR "Buh ? at line $count\n" if ($o_verbose);
					print OUT;
				}
			}

			else {
				print OUT if ($ok_print);
			}

		} # end while, make sure we got the last #endif before EOF

		die "** How the hell did I end up here ? **\nCould not find matching #endif before EOF" if ($other_define > -1);
		# we've still got the line following #endif (or nothing if EOF) in $_
		print OUT;
}

# print help and exit with a proper exit code
sub printHelpExit ($) {
    my $exit_code = shift;
    my $out_stream = ($exit_code) ? *STDERR{IO} : *STDOUT{IO};

    print $out_stream <<"ENDHELP";
    Remove outdated #ifdef constructs to clean-up the code.
    Usage : CleanUpIfDefs.pl [options] <keyword> <file/dir> (<other file/dir> ...)
        <keyword> to match #ifn?def <keyword> patterns
        <file>    file to match from and output to, backed-up
                  to <file>.perlbkp
        <dir>     directory to parse recursively
    Options : -v (--verbose) for verbose (use several times for even more verbose)
              -i (--invert) for inverted match (keep parts where <keyword>
                 is not defined rather than parts where it is)
              --list <filename> to output a list of modified files
                 to <filename>, e.g. for subsequent cvs commit
ENDHELP
	exit $exit_code;
}


