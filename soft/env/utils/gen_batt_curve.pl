#!/usr/bin/env perl

use strict;

my $pgm_name = $0;
$pgm_name =~ s%.*/%%;

my $usage="Usage:
    $pgm_name <volt_file> <full_volt> <down_volt> <high_act_off> <step>
                [ <dischr_cur> <desired_cur> <batt_r> ]
Description:
    volt_file       The file recording the discharging battery voltages
                    periodically in mV or V, which are separated by white
                    space sor new lines
    full_volt       The full voltage in mV
    down_volt       The power down voltage in mV
    high_act_off    The voltage offset for high activity in mV
    step            The resulting battery curve step in mV

    (The following options should be provided or omitted altogether)
    dischr_cur      The discharging current for volt_file voltages in mA
    desired_cur     The desired current for the final battery curve in mA
    batt_r          The battery internal resistance in ohm";

if (scalar(@ARGV) != 5 && scalar(@ARGV) != 8) {
    print "$usage";
    exit 1;
}

use constant ROUND_FACTOR => 10;
use constant ELEM_PER_LINE => 5;
use constant STR_ALIGN => 73;

sub round($)
{
    my $t = 0.5;
    $t = -0.5 if ($_[0] < 0);
    return int($_[0] + $t);
}

sub wrap_line($)
{
    my $str = shift;
    my $align = 1;
    $align = STR_ALIGN - length($str) if (length($str) < STR_ALIGN);
    $str .= sprintf("%*s\n", $align, "\\");
    return $str;
}

sub save_list_to_file($$)
{
    my ($list, $dest_file) = @_;
    open(DEST, ">", $dest_file) || die "Failed to open file: $dest_file : $!\n\t";
    print DEST join("\n", @$list);
    close(DEST);
}

my ($volt_file, $full_volt, $down_volt, $high_act_off, $step,
    $dischr_cur, $desired_cur, $batt_r) = @ARGV;

die "full_volt ($full_volt) should > down_volt ($down_volt)\n" if ($full_volt <= $down_volt);
die "high_act_off ($high_act_off) should > 0\n" if ($high_act_off <= 0);
die "step ($step) should > 0\n" if ($step <= 0);

open(FH, "<", $volt_file) || die "Failed to open voltage file: $volt_file : $!\n\t";
my $content;
my $ret = read(FH, $content, (stat(FH))[7]);
close(FH);
die "Failed to read voltage file: $volt_file : $!\n\t" if (!defined($ret));
die "Empty voltage file: $volt_file\n\t" if ($ret == 0);

my @value = split(/\s+/, $content);

# Change the voltages from floating values in V to integer values in MV
my $offset = 0;
if (defined($dischr_cur) && defined($desired_cur) && defined($batt_r)) {
    $offset = round(($dischr_cur - $desired_cur) * $batt_r * ROUND_FACTOR);
}
my $x;
my $mv_multiplier;
if ($value[0] > 1000) {
    $mv_multiplier = 1;
} elsif ($value[0] < 10) {
    $mv_multiplier = 1000;
} else {
    die "Cannot determine the multiplier to convert battery levels into mV: $value[0]\n\t";
}
for $x (@value) { $x = round($x * $mv_multiplier * ROUND_FACTOR) + $offset; }
@value = sort({ $b <=> $a } @value);

# Delete the values smaller than the power down voltage
# (without the high activity compensation)
my $count;
my $len;
$len = scalar(@value);
for ($count = 0; $count < $len; $count++) {
    if ($value[$count] < $down_volt * ROUND_FACTOR) {
        if ($count == 0) {
            last;
        } elsif (abs($down_volt * ROUND_FACTOR - $value[$count]) >
                abs($value[$count - 1] - $down_volt * ROUND_FACTOR)) {
            last;
        }
    }
}
splice(@value, $count);

#save_list_to_file(\@value, $volt_file . "_del_down");

# Add the high activity offset
for $x (@value) { $x += $high_act_off * ROUND_FACTOR; }

# Delete the values greater than the full voltage
# (with the high activity compensation)
$count = 0;
$len = scalar(@value);
for ($count = $len; $count > 0; $count--) {
    if ($value[$count] > $full_volt * ROUND_FACTOR) {
        if ($count == $len) {
            last;
        } elsif (abs($value[$count] - $full_volt * ROUND_FACTOR) >
                abs($full_volt * ROUND_FACTOR - $value[$count + 1])) {
            last;
        }
    }
}
$count++ if ($count < $len);
splice(@value, 0, $count);

#save_list_to_file(\@value, $volt_file . "_del_full");

# Calculate the battery capacity curve
my $cap;
my $index;
my $len = scalar(@value);

=comment
my @raw_curve;
for ($index = 0; $index < $len; $index++) {
    $cap = round(($len - 1 - $index) * 100 / ($len - 1));
    push(@raw_curve, sprintf("%5d %3d", $value[$index], $cap));
}
save_list_to_file(\@raw_curve, $volt_file . "_raw_curve");
=cut

my $volt;
my $prev_index = $len - 1;
my @curve_cap;
my @curve_volt;
for ($volt = $down_volt; $volt <= $full_volt; $volt += $step) {
    for ($index = $prev_index; $index > 0; $index--) {
        last if ($volt * ROUND_FACTOR <= $value[$index]);
    }
    if ($index >= $len) {
        $cap = 0;
        $prev_index = $len;
    } elsif ($index <= 0) {
        $cap = 100;
        $prev_index = 0;
    } else {
        if (abs($volt * ROUND_FACTOR - $value[$index]) >
                abs($value[$index + 1] - $volt * ROUND_FACTOR)) {
            $index++;
        }
        $cap = round(($len - 1 - $index) * 100 / ($len - 1));
        $prev_index = $index - 1;
    }
    push(@curve_cap, $cap);
    push(@curve_volt, sprintf("%4d %5d %4d", $index, $value[$index], $volt));
}
if ($volt - $step != $full_volt) {
    push(@curve_cap, 100);
    push(@curve_volt, sprintf("%4d %5d %4d", $index, $value[$index], $volt));
}

#save_list_to_file(\@curve_volt, $volt_file . "_curve_volt");

print "-----------------------------------------------\n";
print "batteryLevelFullMV          = $full_volt\n";
print "powerDownVoltageMV          = $down_volt\n";
print "batteryOffsetHighActivityMV = $high_act_off\n";
print "The battery capacity curve step is $step mV\n";
if (defined($dischr_cur) && defined($desired_cur) && defined($batt_r)) {
    print "\nThe voltages on $dischr_cur mA have been converted to those on $desired_cur mA, assuming \nthe battery internal resistance $batt_r ohm\n";
}
print "\nPlease copy and paste the battery capacity curve macros to tgt_board_cfg.h, \nright behind TGT_PMD_CONFIG macro ...\n";
print "-----------------------------------------------\n";
print "\n";

my $str;
$str = "#define TGT_PMD_BATT_CAP_CURVE_ARRAY";
$str = wrap_line($str);
print $str;
$str =  "    {";
$str = wrap_line($str);
print $str;

$len = scalar(@curve_cap);
for ($count = 0; $count < $len; $count++) {
    if ($count == $len - 1) {
        $volt = $full_volt;
    } else {
        $volt = $down_volt + $count * $step;
    }
    $str = (" " x 8) if (($count + 1) % ELEM_PER_LINE == 1);
    $str .= sprintf("{%4d,%3d}, ", $volt, $curve_cap[$count]);
    if (($count + 1) % ELEM_PER_LINE == 0) {
        $str = wrap_line($str);
        print $str;
    }
}
if ($count % ELEM_PER_LINE != 0) {
    $str = wrap_line($str);
    print $str;
}
print "    }";
print "\n";

exit 0;
