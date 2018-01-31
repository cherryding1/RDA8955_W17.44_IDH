# Microsoft Developer Studio Project File - Name="analogtv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=analogtv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "analogtv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "analogtv.mak" CFG="analogtv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "analogtv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "analogtv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "analogtv - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "analogtv - Win32 Debug"

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
# ADD CPP /nologo /Gm /GX /ZI /Od /I "../../../../../platform_simu/include" /I "../../../../../platform/chip/regs/greenstone/include" /I "../../analogtv/src" /I "../../analogtv/include" /I "../../../../chip/hal/include" /I "../../../../../platform_simu/voc/voc_lib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "CS_PLATFORM_SIMU" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"tests/analogtv.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "analogtv - Win32 Release"
# Name "analogtv - Win32 Debug"
# Begin Group "voc_lib"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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
# Begin Source File

SOURCE=..\..\..\..\chip\regs\greenstone\include\voc_ram_asm.h
# End Source File
# End Group
# Begin Group "vpp_tab"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv_asm_main.tab
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv_const_x.tab
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv_const_y.tab
# End Source File
# End Group
# Begin Group "vpp_asm"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv.mx

!IF  "$(CFG)" == "analogtv - Win32 Release"

!ELSEIF  "$(CFG)" == "analogtv - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=\projectroot\greenstone_calib\soft\platform\mdi\alg\analogtv\src
InputPath=..\..\analogtv\src\vpp_analogtv.mx

"$(InputDir)\vpp_analogtv.vocsrcs" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\CSDTK\cygwin\bin\bash --login -c "voc_make_ide greenstone_calib platform/mdi/alg/analogtv/"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv_asm_main.c
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vppp_analogtv_asm_map.h
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vppp_analogtv_common.h
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vppp_analogtv_map.h
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vppp_analogtv_sections.h
# End Source File
# End Group
# Begin Group "vpp_simu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\vpp_analogtv_simu.c
# End Source File
# Begin Source File

SOURCE=.\src\vpp_analogtv_simu_main.c
# End Source File
# End Group
# Begin Group "vpp_target"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\analogtv\include\vpp_analogtv.h
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vpp_analogtv_asm.c
# End Source File
# Begin Source File

SOURCE=..\..\analogtv\src\vppp_analogtv_asm.h
# End Source File
# End Group
# End Target
# End Project
