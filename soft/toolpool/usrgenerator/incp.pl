#!/usr/bin/perl
use strict;
#arg1 待处理的文件名
#arg2 处理后的文件名
#arg3 处理的数量
#arg4 待处理的字符串
#arg5 采用哪种处理方式：0：注释(//)；1：注释(/**/)；2：替换； 3：删除；4：增加
#arg6 如果处理方式是替换或者增加，则表示被替换或增加的字符串所在的文件，否则为空

#

my ($src_file,$des_file, $pnum,@src_strs,@pmodes,@des_strs)= @_;
my $n=0;
$src_file=$ARGV[$n];
$n=$n+1;
$des_file=$ARGV[$n];
$n=$n+1;
$pnum=$ARGV[$n];
$n=$n+1;
my $tmp=$pnum;

while($tmp>0) {
$src_strs[$tmp-1]=$ARGV[$n];
$n=$n+1;
$pmodes[$tmp-1]=$ARGV[$n];
$n=$n+1;
$des_strs[$tmp-1]=$ARGV[$n];
#print $des_strs[$tmp-1];
if($des_strs[$tmp-1] ne "") {
	open(FILE,"<$des_strs[$tmp-1]");
	$des_strs[$tmp-1]="";
	while(<FILE>){
   $des_strs[$tmp-1].=$_;
    }
  }
close FILE;   
$n=$n+1;
$tmp--;
}

my $i;
open(FILE,"<$src_file");
  while(<FILE>){
   $i.=$_;
    }
close FILE;

$tmp=$pnum;
while($tmp>0) {
	if($pmodes[$tmp-1]==0) {
		$i =~s/$src_strs[$tmp-1]/\/\/$src_strs[$tmp-1]/gs;
	}
	elsif($pmodes[$tmp-1]==1) {
		$i =~s/$src_strs[$tmp-1]/\/\*$src_strs[$tmp-1]\*\//gs;
	}
	elsif($pmodes[$tmp-1]==2) {
	}
	elsif($pmodes[$tmp-1]==3) {
		$i =~s/$src_strs[$tmp-1]/ /gs;
	}
	elsif($pmodes[$tmp-1]==4) {
		#print "--$des_strs[$tmp-1]--";
		$i =~s/$src_strs[$tmp-1]/ $des_strs[$tmp-1]/g
	}
	else {
	}
	$tmp--;
}
#system("rm $des_file");
open(FILE,">$des_file");
  print FILE $i;
