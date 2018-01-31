#!/bin/bash

svn pl -v

echo svn info
svn info
echo svn info application 
svn info application 
echo svn info env 
svn info env 
echo svn info platform 
svn info platform 
echo svn info platform/base 
svn info platform/base 
echo svn info platform/chip 
svn info platform/chip 
echo svn info ${PLATFORM_SERVICE} 
svn info ${PLATFORM_SERVICE} 
echo svn info platform/edrv 
svn info platform/edrv 
echo svn info ${PLATFORM_SYSTEM}/mdi 
svn info ${PLATFORM_SYSTEM}/mdi 
echo svn info ${PLATFORM_SYSTEM}/stack 
svn info ${PLATFORM_SYSTEM}/stack 
echo svn info ${PLATFORM_SYSTEM}/svc 
svn info ${PLATFORM_SYSTEM}/svc 
echo svn info ${PLATFORM_SYSTEM}/vpp 
svn info ${PLATFORM_SYSTEM}/vpp 
echo svn info platform_test 
svn info platform_test 
echo svn info target 
svn info target 
echo svn info toolpool 
svn info toolpool 
