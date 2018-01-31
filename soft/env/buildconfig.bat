@echo off

rem This script will be used by buildbot to build on Windows.
rem It should be executed from top directory "soft".

rem For local test
if not defined bb_product set bb_product=test
if not defined BB_GIT_EXE set BB_GIT_EXE=C:\Program Files\Git\bin\git.exe
if not defined BB_PYTHON_EXE set BB_PYTHON_EXE=C:\Python27\python.exe
if not defined BB_GIT_BASH_DIR set BB_GIT_BASH_DIR=C:\Program Files\Git\usr\bin\
if not defined BB_CSDTK4_DIR set BB_CSDTK4_DIR=C:\CSDTK4\

rem Refer to buildconfig.sh for format of %1
set TARGET_PARAM=%1
set GIT="%BB_GIT_EXE%"
set PYTHON="%BB_PYTHON_EXE%"
set CYGPATH="%BB_GIT_BASH_DIR%cygpath.exe"
set BUSYBOX="%BB_CSDTK4_DIR%make\busybox.exe"

set SDISK=
for %%x in (I: J: K: L: M: N: O: P: Q: R: S: T:) do (
	subst %%x %CD% > NUL && set SDISK=%%x && goto found
)

:not_found
echo "subst failed"
exit /B 1

:found
call :trim %SDISK% SDISK
echo "subst %SDISK%"

(echo %TARGET_PARAM%| findstr /R /C:"modem-.*-debug")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"modem-.*-release")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"modemdb-.*-debug")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"modemdb-.*-release")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"phone-.*-debug")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"phone-.*-release")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"phonesim-.*-debug")>NUL && goto build_target
(echo %TARGET_PARAM%| findstr /R /C:"phonesim-.*-release")>NUL && goto build_target
echo Unknown target: %TARGET_PARAM%
goto failed

:build_target
(echo %TARGET_PARAM%| %BUSYBOX% cut -d- -f1) > tmp.txt && set /P BUILD_TYPE= < tmp.txt
(echo %TARGET_PARAM%| %BUSYBOX% cut -d- -f2) > tmp.txt && set /P CT_TARGET= < tmp.txt
(echo %TARGET_PARAM%| %BUSYBOX% cut -d- -f3) > tmp.txt && set /P CT_RELEASE= < tmp.txt
(%CYGPATH% -am .) > tmp.txt && set /p SOFT_WORKDIR= < tmp.txt
(%CYGPATH% -aw .) > tmp.txt && set /p SOFT_WINWORKDIR= < tmp.txt

call %BB_CSDTK4_DIR%CSDTKvars.bat || goto failed
set PATH=%SOFT_WINWORKDIR%env\utils;%SOFT_WINWORKDIR%env\win32;%PATH%
set CT_PRODUCT=%bb_product%
set CT_USER=ADMIN
set CT_OPT=dbg_size
set WITH_SVN=0
set WITH_GIT=1
set WITHOUT_WERROR=y
set HEX=%CT_TARGET%_%CT_PRODUCT%_%CT_RELEASE%
call:cleanup
call:build_%BUILD_TYPE%

echo "DONE"
subst %SDISK% /d
exit /B %ERRORLEVEL%

rem function clean
:cleanup
%GIT% clean -fdx || goto cleanup_return
%GIT% reset --hard HEAD || goto cleanup_return
:cleanup_return
exit /B %ERRORLEVEL%

rem function build_phone
:build_phone
make -r -j%NUMBER_OF_PROCESSORS% CT_RESGEN=yes CT_USERGEN=yes || goto build_phone_return
:build_phone_return
exit /B %ERRORLEVEL%

rem function build_phonesim
:build_phonesim
make -r -j%NUMBER_OF_PROCESSORS% CT_RESGEN=yes resgen || goto build_phonesim_return
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86 || goto build_phonesim_return
MSBuild.exe %SDISK%\application\CoolSimulator\CoolSimulator.sln /m /p:Configuration=Debug || goto build_phonesim_return
:build_phonesim_return
exit /B %ERRORLEVEL%

rem function build_modem
:build_modem
make -r -j%NUMBER_OF_PROCESSORS% CT_RESGEN=no CT_USERGEN=no || goto build_modem_return
:build_modem_return
exit /B %ERRORLEVEL%

rem function build_modemdb
:build_modemdb
make -r -j%NUMBER_OF_PROCESSORS% CT_RESGEN=no CT_USERGEN=no || goto build_modemdb_return
make -C toolpool/blfota -r -j%NUMBER_OF_PROCESSORS% CT_RESGEN=no CT_USERGEN=no || goto build_modemdb_return
%PYTHON% env/utils/dual_boot_merge.py --bl hex/%HEX%/blfota_%HEX%_flash.lod --lod hex/%HEX%/%HEX%_flash.lod  --output hex/%HEX%/merge_%HEX%_flash.lod || goto build_modemdb_return
:build_modemdb_return
exit /B %ERRORLEVEL%

rem function trim
:trim
set %2=%1
goto :eof

:failed
echo "Failed"
subst %SDISK% /d
exit /B %ERRORLEVEL%
