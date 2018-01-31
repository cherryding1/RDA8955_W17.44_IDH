#!/usr/bin/perl
use warnings;

#base trace idx of cfw
$trcIdx=0x08100000;
$maxParaNum=6;

$rootDir=$ENV{'PWD'};
@all_dir_lst=($rootDir);
@all_file_lst=();

print $rootDir."\n";
	
&main;

sub main {

	while(@all_dir_lst){
		&list_file;
	}
	
	foreach $iFName (@all_file_lst) {	
		my $iLine;
		my $iFHdl;

		print "$iFName.\n";
		
		open($iFHdl, "<", $iFName);
		
		while($iLine=<$iFHdl>){
			if($iLine=~/\bTSTR[ \t\\\r\n]*\("[^"]+"[ \t\\\r\n]*,[ \t\\\r\n]*(0x[0-9a-fA-F]{8})\)/){
				if($trcIdx < hex($1)){
				  $trcIdx = hex($1);
				}
			}
		}
		close $iFHdl;
	}		
		$trcIdx++;
		
		foreach $iFName (@all_file_lst) {
		my $oFName = "$iFName".".trcOut";
		my $iLine;
		my $nLine;
		my $iFHdl;
		my $oFHdl;
		
		print "$iFName.\n";
		
		open($iFHdl, "<", $iFName);
		open($oFHdl, ">", $oFName);
	
		while($iLine=<$iFHdl>){
			if ($iLine=~/\bCSW_TRACE/){
				unless ($iLine=~/\)[ |\t]*;/) {
					while($nLine=<$iFHdl>) {
						if ($nLine=~/\)( |\t)*;/){
							last;
						}
						else {
							$iLine .= $nLine;
						}
					}
					$iLine .= $nLine;
				}

				$iLine = &CSW_TRACE_hdl($iLine);
				print $oFHdl $iLine;	
			}			
			else {
				print $oFHdl $iLine;	
			}
		}
		
		close $iFHdl;
		close $oFHdl;
		
		unlink $iFName;
		rename $oFName, $iFName;
	}
}

sub list_file {
	my $myDirName = shift(@all_dir_lst);
	my $myDirHdl;	
	my $myFile;
	my $myFullFile;
	
	opendir($myDirHdl, $myDirName);
	while ($myFile=readdir $myDirHdl) {
		$myFullFile = $myDirName."/".$myFile;
		
		if(-d $myFullFile) {
			unless ($myFile eq ".svn" || $myFile eq "." || $myFile eq ".."){
				my $all_dir_idx = @all_dir_lst;
				$all_dir_lst[@all_dir_lst]=$myFullFile;
			}
		} 
		elsif (-f $myFullFile) {
			if($myFile=~/.*c$/) {
				my $all_file_idx = @all_file_lst;
				$all_file_lst[$all_file_idx]=$myFullFile;
			}
		}
	}
}

sub CSW_TRACE_hdl {
	
	my $iLine = shift(@_);
	
	if($iLine=~/\bCSW_TRACE[ \t\\\r\n]*\([ \t\\\r\n]*(\b\w[^,]*\b)[ \t\\\r\n]*,[ \t\\\r\n]*[TSTXT\s*\(]*("[^"]+")[\)]*/m)
	{
		my $i1 = $1;
		my $i2 = $2;
		
		my %myHash = fmtStr_search($2);
		my $myHashNum = keys %myHash;		
		
		if ($myHashNum <= $maxParaNum) {
		  if($1=~/TDB/){
		      return $iLine
		  }
			my $p1 = "_CSW|TLEVEL(".$1.")"."|TDB";
			my $p2 = sprintf "TSTR(%s,0x%08x)", $i2, $trcIdx++;
			
			$p1.="|TNB_ARG($myHashNum)";
			
			foreach (sort keys %myHash){
				my $myVal=$myHash{$_};
				if($myVal eq "%s"){
					my $myId = $_+1;
					$p1.="|TSMAP($myId)";
				}
			}
	
			$iLine=~s/\Q$i1\E/$p1/m;
			$iLine=~s/("[^"]+")/$p2/m;
		}
		else{
		  print $iLine;
	  }
	}

	$iLine;
}


sub fmtStr_search {
	my $iStr=shift(@_);
	my $ch="";
	my $pos=0;
	my $now=-1;
	my $upd = 0;
	my $hashIdx=0;
	my %my_hash=();
	my $oStr="";
	
	$iStr=~s/\%[0-9l]*[dD]/\%d/g;
	$iStr=~s/\%[0-9l]*[uU]/\%u/g;
	$iStr=~s/\%[0-9l]*[xX]/\%x/g;
	$iStr=~s/\%[0-9l]*[iI]/\%i/g;
		
	until($pos==-1){
		$pos0=index($iStr,"%d", $now+1);
		if($pos0>0){
			$ch="%d";
			$pos=$pos0;
			$upd=1;
		}
		
		$pos0=index($iStr,"%x", $now+1);
		if ($pos0>0)
		{
			if($upd){
				if($pos0<$pos) {
					$pos=$pos0;
					$ch="%x";
				}
			}
			else {
				$pos=$pos0;
				$ch="%x";
				$upd=1;
			}
		}
		
		$pos0=index($iStr,"%u", $now+1);
		if ($pos0>0) {
			if($upd){
				if($pos0<$pos) {
					$pos=$pos0;
					$ch="%u";
				}
			}
			else {
				$pos=$pos0;
				$ch="%u";
				$upd=1;
			}
		}
		
		$pos0=index($iStr,"%s", $now+1);
		if ($pos0>0) {
			if($upd){
				if($pos0<$pos) {
					$pos=$pos0;
					$ch="%s";
				}
			}
			else {
				$pos=$pos0;
				$ch="%s";
				$upd=1;
			}
		}
		
		$pos0=index($iStr,"%i", $now+1);
		if ($pos0>0) {
			if($upd){
				if($pos0<$pos) {
					$pos=$pos0;
					$ch="%i";
				}
			}
			else {
				$pos=$pos0;
				$ch="%i";
				$upd=1;
			}
		}
		
		if($upd) {
			$my_hash{$hashIdx}=$ch;
			$hashIdx+=1;
			$now=$pos;
			$upd=0;
		}
		else {
			$pos=-1;
		}	
	}
	
	%my_hash;
}