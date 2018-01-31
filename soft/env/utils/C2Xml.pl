#!/usr/bin/perl
# Author : Laurent Bastide
# Purpose: parses .c file to extract structures for coolwatcher
# Far from being bullet-proof...
use strict;

my $file = shift;
my $outfile;
my @obj_tab;

open IN, "$file"
	or die "Could not open $file";
$outfile = "out.xml";
#open OUT, "$outfile"
#	or die "Could not open $outfile";

#my @lines = <IN>;
#foreach (@lines)

#$/ = '';
undef $/;

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<archive>\n";

while (<IN>)
{
    # Pre filtering
    s/^[ ]*\/\/.*\n//g;
    s/\/\/.*\n/\n/g;
    s/\/\*.*\n/\n/g;
    s/\n+/\n/g;
    s/^[ ]*\n//g;
    s/[ ]*\n/\n/g;
    #s/\(.*\)\\\n\(.*\)/\1\2/g;

    # defines
    while (/#define[ ]+([_[:alnum:]]*)[ ]+([\(\) _[:alnum:]\+\-\*]+)/gms)
    {
        print "<var name=\"$1\" value=\"$2\"/>\n";
    }

    # enums
    while (/typedef enum\n{\n([,\n _[:alnum:]\=]+)}[ ]*([[:alnum:]_]+);/gms)
    {
        my $tmp;
        my $enumval = 0;
        print "\n<enum name=\"$2\">\n";
        $tmp = $1;
        #$tmp =~ s/[ ]*([_[:alnum:]]*)[,]*\n/<entry name="\1"\t\tvalue=""\/>\n/g;
        #print $tmp;
        while ($tmp =~ /[ ]*([_[:alnum:] \=]+)[,]*\n/gms)
        {
            my $name = $1;
            if ($name =~ /([_[:alnum:]]*) *\= *([_[:alnum:]]*)/gms)
            {
                $name = $1;
                $enumval = $2;
            }
            print "    <entry name=\"$name\"\t\tvalue=\"$enumval\"/>\n";
            $enumval++;
        }
        print "<\/enum>\n";
    }

    # structs
    while (/typedef struct\n{\n([;\+\-\*\\\n\[\]\(\) _[:alnum:]]*)} *([[:alnum:]_]*);/gms)
    {
        my $tmp;
        print "\n<typedef name=\"$2\">\n";
        $tmp = $1;
        # Transform the simple fields.
        $tmp =~ s/( *)([volatile]*) *UINT8 *([[:alnum:]_]*);/\1<uint8 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *BOOL *([[:alnum:]_]*);/\1<uint8 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT16 *([[:alnum:]_]*);/\1<uint16 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT32 *([[:alnum:]_]*);/\1<uint32 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT8 *([[:alnum:]_]*);/\1<int8 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT16 *([[:alnum:]_]*);/\1<int16 name="\3" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT32 *([[:alnum:]_]*);/\1<int32 name="\3" ckeywords="\2"\/>/g;
        # Transform the array fields.
        $tmp =~ s/( *)([volatile]*) *UINT8 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<uint8 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *BOOL *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<uint8 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT16 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<uint16 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT32 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<uint32 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT8 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<int8 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT16 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<int16 name="\3" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT32 *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<int32 name="\3" count="\4" ckeywords="\2"\/>/g;
        # Transform the pointers fields.
        $tmp =~ s/( *)([volatile]*) *UINT8 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="uint8" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *BOOL *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="uint8" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT16 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="uint16" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT32 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="uint32" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT8 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="int8" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT16 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="int16" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT32 *\* *([[:alnum:]_]*);/\1<ptr name="\3" type="int32" ckeywords="\2"\/>/g;
        # Transform the arrays of pointers fields.
        $tmp =~ s/( *)([volatile]*) *UINT8 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="uint8" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *BOOL *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="uint8" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT16 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="uint16" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *UINT32 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="uint32" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT8 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="int8" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT16 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="int16" count="\4" ckeywords="\2"\/>/g;
        $tmp =~ s/( *)([volatile]*) *INT32 *\* *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<ptr name="\3" type="int32" count="\4" ckeywords="\2"\/>/g;
        #Transform the instance of typedef fields.
        $tmp =~ s/( *)([volatile]*) *([[:alnum:]_]*) *([[:alnum:]_]*)\[([\(\)\+\- [:alnum:]_\*]*)\];/\1<instance name="\4" type="\3" ckeywords="\2" count="\5"\/>/g;
        $tmp =~ s/( *)([volatile]*) *([[:alnum:]_]*) *([[:alnum:]_]*);/\1<instance name="\4" type="\3" ckeywords="\2" count="1"\/>/g;
        print $tmp;
        print "<\/typedef>\n";
    }

}
print "</archive>\n";


