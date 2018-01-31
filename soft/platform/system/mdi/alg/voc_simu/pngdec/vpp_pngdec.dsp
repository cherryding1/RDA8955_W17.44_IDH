# Microsoft Developer Studio Project File - Name="vpp_pngdec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vpp_pngdec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpp_pngdec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpp_pngdec.mak" CFG="vpp_pngdec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpp_pngdec - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vpp_pngdec - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpp_pngdec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\voc_lib" /I "..\..\..\..\platform\chip\regs\greenstone\include" /I "..\..\..\..\platform\chip\hal\include" /I "..\..\..\..\platform\vpp\pngdec\include" /I "..\..\..\..\platform\vpp\pngdec\src" /I "..\..\..\..\platform\vpp\pngdec\obj" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /Zm900 /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "vpp_pngdec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../../platform_simu/include" /I "../../../../../platform/chip/regs/gallite/include" /I "../../pngdec/src" /I "../../pngdec/include" /I "../../../../chip/hal/include" /I "../../../../../platform_simu/voc/voc_lib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /Zm900 /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"work/vpp_pngdec.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "vpp_pngdec - Win32 Release"
# Name "vpp_pngdec - Win32 Debug"
# Begin Group "voc_lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\include\cs_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\chip\hal\include\hal_error.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\chip\hal\include\hal_voc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\hal_voc_simu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_library.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_library.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_opcodes.tab
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_profile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_profile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_simulator.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_simulator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\platform_simu\voc\voc_lib\voc2_typedef.h
# End Source File
# End Group
# Begin Group "vpp_asm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec.mx

!IF  "$(CFG)" == "vpp_pngdec - Win32 Release"

!ELSEIF  "$(CFG)" == "vpp_pngdec - Win32 Debug"

# Begin Custom Build
InputDir=\projects\gallite_trunk\soft\platform\mdi\alg\pngdec\src
InputPath=..\..\pngdec\src\vpp_pngdec.mx

"$(InputDir)\vpp_pngdec.vocsrcs" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\CSDTK\cygwin\bin\bash --login -c "voc_make_ide gallite_trunk platform/mdi/alg/pngdec"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec_asm_common.c
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vppp_pngdec_asm_common.h
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vppp_pngdec_asm_map.h
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vppp_pngdec_asm_sections.h
# End Source File
# End Group
# Begin Group "vpp_simu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\vpp_pngdec_simu.c
# End Source File
# Begin Source File

SOURCE=.\src\vpp_pngdec_simu_main.c
# End Source File
# End Group
# Begin Group "vpp_tab"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec_asm_bye.tab
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec_asm_common.tab
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec_const.tab
# End Source File
# End Group
# Begin Group "vpp_target"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\pngdec\include\vpp_pngdec.h
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vpp_pngdec_asm.c
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vppp_pngdec.h
# End Source File
# Begin Source File

SOURCE=..\..\pngdec\src\vppp_pngdec_asm.h
# End Source File
# End Group
# End Target
# End Project
