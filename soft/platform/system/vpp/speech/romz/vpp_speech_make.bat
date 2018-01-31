@echo off

	echo.
	echo * ---------------------------------------------- *
	echo * MS-DOS make script for vpp_speech              *
    echo * last modified 25\06\2008                       *
	echo * ---------------------------------------------- *
	echo.

    if "%1" == "remote" goto DIR_REMOTE
    goto DIR_LOCAL
 :DIR_REMOTE   
    cd ..\..\..\..\platform\vpp\speech\romz
 :DIR_LOCAL

	
	echo.
	echo Copying the VoC tools to the obj directory.
	echo.
 
    if exist obj goto OBJ_DIR_EXISTS
    mkdir obj
:OBJ_DIR_EXISTS

    copy ..\..\..\..\platform_simu\voc\voc_tools\*.exe obj\.
        
	echo.
	echo Copying the source files to the obj directory...
	echo.
    
    copy ..\..\..\..\platform\chip\regs\granite\include\voc_ram_asm.h      obj\.
    copy ..\..\..\..\platform_simu\voc\voc_lib\voc2_define.h               obj\.
    copy src\*.c                                                           obj\.
    copy src\*.h                                                           obj\.
    copy .\*.mx                                                            obj\.
 
    cd obj

  	voc_map     -m       vpp_speech_map.mx

	copy map_addr.h      vpp_speech_asm_map.h
	move map_addr.h      ..\src\vppp_speech_asm_map.h
	move MAP_EXPORT.h    ..\src\vppp_speech_map.h
  	move map.xml         vpp_speech_map.new
    move map_main.xml    vpp_speech_map_main.new

	voc_compile -m       vpp_speech_code.mx

	cd ..
    
    del /Q obj\*.dot
	del /Q obj\*.sec
	del /Q obj\*.h
	del /Q obj\*.c   
	del /Q obj\*.exe
	del /Q obj\*.voc
	del /Q obj\*.par
	del /Q obj\*.mx
	del /Q obj\*.map

:END_GSM_RUN

    if "%1" == "remote" goto NO_PAUSE
    pause
    
:NO_PAUSE


