#!/bin/bash
get_pathname()
{
	echo "get_pathname $1"
	s="/"
	ispath=`echo $1 | grep "/"`
	if [ "$ispath" = "" ]
	then
		isit=`ls $newpath | grep -w $1`
                IsIt=`ls $newpath | grep -i -w $1`
                if [ "$isit" = "" ] && [ "$IsIt" != "" ]
                then
                        path_name="$IsIt"
                elif [ "$isit" != "" ]
                then
                        path_name="$isit"
                else
                        return 0
                fi
                #echo "path_name = $path_name"
                newpath="$newpath$s$path_name"
                echo "newpath=$newpath"
		return 0
	else
		path1=""
		path2=""
		path3=""
		path1=`echo $1 | awk -F"/" '{print $1}'`
		path2=`echo $1 | awk -F"/" '{print $2}'`
		path3=`echo $1 | awk -F"/" '{print $3}'`
		if [ "$path1" = "$path2" ]
		then
			next_path=`echo $1 | awk -F"$path1/$path2/" '{print $2}'`
			if [ "$path2" = "$path3" ]
			then
				next_path=`echo $1 | awk -F"$path1/$path2/$path3" '{print $2}'`
			fi
		else
			next_path=`echo $1 | awk -F"$path1/" '{print $2}'`
		fi
		#echo "path=$path next_path=$next_path"
		isit=`ls $newpath | grep -w $path1`
		IsIt=`ls $newpath | grep -i -w $path1`
		if [ "$path1" = ".." ]
		then
			path_name=""
			newpath="$path1$s$path2"
		else
			if [ "$isit" = "" ] && [ "$IsIt" != "" ]
			then 
				path_name="$IsIt"
			elif [ "$isit" != "" ]
			then
				path_name="$isit"
			else
				echo "newpath=$newpath"
				return 1
			fi
		fi
		if [ "$path1" = "$path2" ]
                then
                        newpath="$newpath$s$path_name$s$path_name"
                        if [ "$path2" = "$path3" ]
                        then
                                newpath="$newpath$s$path_name"
                        fi
                else
                        newpath="$newpath$s$path_name"
                fi
		get_pathname $next_path
	fi
}
process_delete_image()
{
	#newpath="./newpath.txt"
    	#echo "s/this_is_first_line/don'twork/I" >$resultfile
	echo "process_iamge $1"
	#name="$1"
	for name in $1
	do
		echo -e " name : $name"
		if [ -f $name ]
		then
			:
		else
			newpath=""
			get_pathname $name
			if [ $? -eq 1 ]
			then
				echo "$name" >>$waringfile
				continue
			fi
			name="$newpath"	
		fi
		if [ -d $name ]
		then
			echo "$name" >>$waringfile
		fi
 	#	fbl=`identify $name |awk '{print $3}'| sed -n -e "1p"`
	#    	fbl_w=`echo "$fbl" | awk -F"x" '{print $1}'`
	#    	fbl_h=`echo "$fbl" | awk -F"x" '{print $2}'`
	 #   	echo "fbl_w:$fbl_w fbl_h:$fbl_h"
	 #   	new_fbl_w=`expr $fbl_w \* 80 / 100`
	 #   	new_fbl_h=`expr $fbl_h \* 80 / 100`
	 #   	echo "new_fbl_w:$new_fbl_w new_fbl_h:$new_fbl_h"
		suffix=`echo $name | awk -F. '{print $NF}'`
		namenopath=`echo $name | awk -F"/" '{print $NF}'`
            	nopathsuffix=`echo $namenopath | awk -F. '{print $1}'`
            	dot="[.]"
            	gif=".gif"
            	jpg=".jpg"
            	png=".png"
            	pbm=".pbm"
            	bmp=".bmp"
            	defaultcase=""
            	t_name="$nopathsuffix$png"
	    	pfilename=`echo $name | sed -e "s/$namenopath/$t_name/g"`
            	t_name="$nopathsuffix$jpg"
		jfilename=`echo $name | sed -e "s/$namenopath/$t_name/g"`
            	t_name="$nopathsuffix$gif"
		gfilename=`echo $name | sed -e "s/$namenopath/$t_name/g"`
		t_name="$nopathsuffix$pbm"
		pbmfilename=`echo $name | sed -e "s/$namenopath/$t_name/g"`
		t_name="$nopathsuffix$bmp"
		bmpfilename=`echo $name | sed -e "s/$namenopath/$t_name/g"`
		#echo -e "pfile : $pfilename \njfile : $jfilename \ngfile : $gfilename"
             	filesize=55555540
            	jfilesize=55555550
            	pfilesize=55555550
            	gfilesize=55555550
            	case "$suffix" in
                	"png" | "PNG")
                	       echo "case in png name is $name";
                            	rm -f $jfilename
				rm -f $gfilename
				rm -f $pbmfilename
				rm -f $bmpfilename
                        	;;
                	"bmp" | "BMP")
                        	echo "case in bmp name is $name";
                        	rm -f $jfilename
				rm -f $gfilename
				rm -f $pbmfilename
				rm -f $pfilename
                        	;;
                        "pbm" | "PBM")
                        	echo "case in pbm name is $name";
                        	rm -f $jfilename
				rm -f $gfilename
				rm -f $pfilename
				rm -f $bmpfilename
                        	;;
                	"gif" | "GIF")
                        	echo "case in gif name is $name"
                        	rm -f $jfilename
				rm -f $pfilename
				rm -f $pbmfilename
				rm -f $bmpfilename
                        	;;
                        "jpg" | "JPG")
                                echo "case in jpg name is $name"
                        	rm -f $gfilename
				rm -f $pfilename
				rm -f $pbmfilename
				rm -f $bmpfilename
				;;
                	*)
              			echo "case in default name is $name"
                        	defaultcase="1"
				;;
			esac
		case "$defaultcase" in
                "1")
                        ;;
                "")
                        if [ -f $name ]
                        then
                                filesize=`ls -l $name |awk -F" " '{print $5}'`
                        fi
                        if [ -f $pfilename ]
                        then
                                pfilesize=`ls -l $pfilename |awk -F" " '{print $5}'`
                        fi
                        if [ -f $jfilename ]
                        then
                                jfilesize=`ls -l $jfilename |awk -F" " '{print $5}'`
                        fi
                        if [ -f $gfilename ]
                        then
                                gfilesize=`ls -l $gfilename |awk -F" " '{print $5}'`
                        fi
                        if [ $jfilesize -eq 0 ]
                        then
                                jfilesize=`expr $pfilesize + 1`
                        fi
                        echo -e "$name\t\t$filesize"
                        echo -e "$pfilename\t\t$pfilesize"
                        echo -e "$jfilename\t\t$jfilesize"
                        echo -e "$gfilename\t\t$gfilesize"
                        if [ $pfilesize -lt $jfilesize ]
                        then
                                #rm -f $jfilename
                                if [ $pfilesize -lt $gfilesize ]
                                then
                                        #rm -f $gfilename
                                        if [ "$name" = "$pfilename" ]
                                        then
                                                :
                                        else
                                                if [ $filesize -lt $pfilesize ]
                                                then
                                                        :#rm -f $pfilename
                                                else
                                                        count=`expr $count + 1`
                                                        echo -e "$name\n\t$pfilename"
                                                        #echo "s/\([\/\"]\)$nopathsuffix$dot$suffix/\1$nopathsuffix$png/I">>$resultfile
                                                fi
                                        fi
                                else
                                        #rm -f $pfilename
                                        if [ "$name" = "$gfilename" ]
                                        then
                                                :
                                        else
                                                if [ $filesize -lt $gfilesize ]
                                                then
                                                        :#rm -f $gfilename
                                                else
                                                        count=`expr $count + 1`
                                                        echo -e "$name\t\t$gfilename"
                                                        #echo "s/\([\/\"]\)$nopathsuffix$dot$suffix/\1$nopathsuffix$gif/I">>$resultfile
                                                fi
                                        fi
                                fi
                        else
                                #rm -f $pfilename
                                if [ $jfilesize -lt $gfilesize ]
                                then
                                        #rm -f $gfilename
                                        if [ "$name" = "$jfilename" ]
                                        then
                                                :
                                        else
                                                if [ $filesize -lt $jfilesize ]
                                                then
                                                        :#rm -f $jfilename
                                                else
                                                        count=`expr $count + 1`
                                                        echo -e "$name\t\t$jfilename"
                                                        #echo "s/\([\/\"]\)$nopathsuffix$dot$suffix/\1$nopathsuffix$jpg/I">>$resultfile
                                                fi
                                        fi
                                else
                                        #rm -f $jfilename
                                        if [ "$name" = "$gfilename" ]
                                        then
                                                :
                                        else
                                                if [ $filesize -lt $gfilesize ]
                                                then
                                                        :#rm -f $gfilename
                                                else
                                                        count=`expr $count + 1`
                                                        echo -e "$name\n$gfilename"
                                                        #echo "s/\([\/\"]\)$nopathsuffix$dot$suffix/\1$nopathsuffix$gif/I">>$resultfile
                                                fi
                                        fi
                                fi
                        fi
        		;;
                *)
                        ;;
		esac
	done
}
	ResDir="$SOFT_WORKDIR/application/target_res/"
        GetTransColor="$SOFT_WORKDIR/application/target_res/utils/rtc"
        if [ -f $GetTransColor ]
        then
                :
        else
                echo "SOFT_WORKDIR not set,please set project env"
                return 0
        fi
	unalias ls
    	count="0"
	isResize="0"
    	resultfile="./tmp.sed"        #转换结果
    	anigiffile="./aniGIF.txt"        #gif动画格式，不能修�?    	waringfile="./waring.txt"        #同名图片转换成不同格式，冲突
	allsed="./full.sed"
    	rm -f $resultfile
    	rm -f $anigiffile
    	rm -f $waringfile
	rm -f $allsed
    	#echo "s/this_is_first_line/don'twork/I" >>$resultfile
	usedImageList="./usedImageList"
	isright=`sed -n -e '/\r/p' $usedImageList`
	if [ "$isright" != "" ]
	then
		sed -i -e 's/.$//' $usedImageList
		#sed -i -e 's/$/ /' $usedImageList
	fi
	cfilelist=`ls ../Res_MMI/*.c`
	#cfile=`cat $usedImageList | awk -F":" '{print $1}'`
	echo "$cfilelist"
	imagefile=`cat $usedImageList | awk -F":" '{print $2}'`
	for cfile in $cfilelist
	do
		echo "$cfile"
		imagelist=`cat $usedImageList | awk -F":" '$1=="'$cfile'" {print $2}'`
		echo "$imagelist"
		if [ "$imagelist" == "" ]
		then
			:
		else
			#index="1"
			#image=`awk '$NR=="'$index'" {print $0}'`
			#for image in $imagelist
			#while [ "$image" != "" ]
			#echo $imagelist | while read image
			#do
				process_delete_image "$imagelist"
				if [ -f $resultfile ]
				then
					sed -i -f $resultfile $cfile
					cat $resultfile >>$allsed
				fi
				#index=`expr $index + 1`
			#done
		fi
	done
	cfile="PopulateRes.c"
	echo "$cfile"
        imagelist=`cat $usedImageList | awk -F":" '$1=="'$cfile'" {print $2}'`
        #echo "$imagelist"
        if [ "$imagelist" == "" ]
        then
              :
        else
		process_image "$imagelist"
               	if [ -f $resultfile ]
                then
                      sed -i -f $resultfile $cfile
                      cat $resultfile >>$allsed
                fi
        fi
	rm -f $resultfile	
	echo "end"


