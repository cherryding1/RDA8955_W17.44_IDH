#!/usr/bin/perl
# Author : Laurent Bastide
# Purpose: parses .map file to extract code/data size foreach module
use strict;

my $file = shift;
my @obj_tab;

open IN, "$file"
	or die "Could not open $file";
my @lines = <IN>;

foreach (@lines)
{
    if (/^ \.text/) # get code only
    {
        # Pre filtering
        s/ .text          0x00000000[0-9a-f]+//g; # mapping
        s/\/tmp\/[a-z]+_obj\///g; # temp dir

        if (/Granite\/soft\/platform\//) # get platform only
        {
            s/[ ]*0x//g; # hex
            s/Granite\/soft\/platform\/([a-zA-Z_]+)\/lib\/[a-zA-Z_]+.a/$1/g; # lib

            if (/([0-9a-f]+) ([a-zA-Z]+)\(([0-9a-zA-Z_]+).o\)/) # extract size, module and object
            {
                my $size = hex($1);
                #print "Module:$2 Object:$3 Size:$size \n";
                my %obj = (
                    NAME => $3,
                    MODULE => $2,
                    SIZE => $size,
                );
                push @obj_tab, {%obj};
            }
        }
    }
}

sub bymodule { $a->{MODULE} cmp $b->{MODULE}; }
sub bysize { $a->{SIZE} <=> $b->{SIZE}; }
sub byname { $a->{NAME} cmp $b->{NAME}; }
sub myway
{
    $a->{MODULE} cmp $b->{MODULE} or
    $a->{SIZE} <=> $b->{SIZE} or
    $a->{NAME} cmp $b->{NAME};
}

my $objref;
my $lastModule = "first";
my $totalModSize = 0;
my $totalSize = 0;
foreach $objref (sort myway @obj_tab)
{
format ModFmt =
-----------------------------------------------
Module : @<<<<
         uc $objref->{MODULE}
.

format ObjFmt =
         @<<<<<<<<<<<<<<<<<  : @<<<<<<
         $objref->{NAME},      $objref->{SIZE}
.

format ModTotalFmt =
         --------------------------------------
         @<<<<< code size             : @>>>>>>
         uc $lastModule,                $totalModSize
.

format TotalFmt =
-----------------------------------------------
Overall code size                     : @>>>>>>
         $totalSize
.

    # new module
    if ( $lastModule ne $objref->{MODULE} )
    {
        #not first time -> last module total size
        if ($lastModule ne "first")
        {
            $~ = "ModTotalFmt";
            write;
        }

        $~ = "ModFmt";
        write;

        $lastModule = $objref->{MODULE};
        $totalModSize = 0;
    }
    $~ = "ObjFmt";
    $totalModSize += $objref->{SIZE};
    write;

    $totalSize += $objref->{SIZE};
}
# last object
$~ = "ModTotalFmt";
write;
# final size
$~ = "TotalFmt";
write;


