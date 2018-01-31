#!/bin/sh

echo " ---- begin ----"
if [ $# != 1 ]; then
	echo " You should input an dir."
	exit 1
else
	for file in $(find $1 -iname "*.c"); do
		mkcprt $file tmp.c
		mv tmp.c $file
		echo " ---- <$file>"
	done
	for file in $(find $1 -iname "*.cpp"); do
		mkcprt $file tmp.cpp
		mv tmp.cpp $file
		echo " ---- <$file>"
	done
#	for file in $(find $1 -iname "*.h"); do
#		mkcprt $file tmp.c
#		mv tmp.c $file
#		echo " ---- <$file>"
#	done
#	for file in $(find $1 -iname "*.hpp"); do
#		mkcprt $file tmp.cpp
#		mv tmp.cpp $file
#		echo " ---- <$file>"
#	done
fi 
echo "---- finish ----"
