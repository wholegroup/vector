echo off
echo Copy the SDK examples to the Photoshop Plug-Ins folder
echo Use the Windows Registry to find the location of installed
echo versions of Photshop.
echo Version 1.0
set WinVer=
set WinMajor=
set WinMinor=
set WinBuild=
set RegTokens=2
:; Get Value from 'VER' command output 
FOR /F "tokens=*" %%i in ('VER') do SET WinVer=%%i 
FOR /F "tokens=1-3 delims=]-" %%A IN ("%WinVer%" ) DO ( 
 set Var1=%%A 
) 
:; Get version number only so drop off Microsoft Windows Version 
FOR /F "tokens=1-9 delims=n" %%A IN ("%Var1%" ) DO ( 
 set WinVer=%%C 
 REM echo %WinVer% 
) 
:; Separate version numbers 
FOR /F "tokens=1-8 delims=.-" %%A IN ("%WinVer%" ) DO ( 
set WinMajor=%%A 
set WinMinor=%%B 
set WinBuild=%%C 
) 
:; Fix the extra space left over in the Major 
FOR /F "tokens=1 delims= " %%A IN ("%WinMajor%" ) DO ( 
 set WinMajor=%%A 
) 
:; Display Results	
REM ECHO WinVer = %WinVer% 
REM ECHO WinMajor = %WinMajor% 
REM ECHO WinMinor = %WinMinor% 
REM ECHO WinBuild = %WinBuild% 
IF %WinMajor%==6 (
 REM ECHO Windows 7
) ELSE (
 REM ECHO Windows XP
 set RegTokens=3
)
set ps_cs4_11="NOT INSTALLED"
set ps_cs5_12="NOT INSTALLED"
set ps_cs55_12p1="NOT INSTALLED"
set ps_cs6_13="NOT INSTALLED"
set ps_cs4_11_32="NOT INSTALLED"
set ps_cs5_12_32="NOT INSTALLED"
set ps_cs55_12p1_32="NOT INSTALLED"
set ps_cs6_13_32="NOT INSTALLED"
set ps_install_path="NOT INSTALLED"
set sdk_source_path="NOT INSTALLED"
FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\11.0\PluginPath" ') DO SET ps_cs4_11="%%B"
REM ECHO ps_cs4_11=%ps_cs4_11%
FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\12.0\PluginPath" ') DO SET ps_cs5_12="%%B"
REM ECHO ps_cs5_12=%ps_cs5_12%
FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\55.0\PluginPath" ') DO SET ps_cs55_12p1="%%B"
REM ECHO ps_cs55_12p1=%ps_cs55_12p1%
FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\60.0\PluginPath" ') DO SET ps_cs6_13="%%B"
REM ECHO ps_cs6_13=%ps_cs6_13%
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Wow6432Node\Adobe\Photoshop\11.0\PluginPath" ') DO SET ps_cs4_11_32="%%B"
 REM ECHO ps_cs4_11_32=%ps_cs4_11_32%
 FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Wow6432Node\Adobe\Photoshop\12.0\PluginPath" ') DO SET ps_cs5_12_32="%%B"
 REM ECHO ps_cs5_12_32=%ps_cs5_12_32%
 FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Wow6432Node\Adobe\Photoshop\55.0\PluginPath" ') DO SET ps_cs55_12p1_32="%%B"
 REM ECHO ps_cs55_12p1_32=%ps_cs55_12p1_32%
 FOR /F "tokens=%RegTokens%* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Wow6432Node\Adobe\Photoshop\60.0\PluginPath" ') DO SET ps_cs6_13_32="%%B"
 REM ECHO ps_cs6_13_32=%ps_cs6_13_32%
) ELSE (
 set ps_cs4_11_32=%ps_cs4_11%
 set ps_cs5_12_32=%ps_cs5_12%
 set ps_cs55_12p1_32=%ps_cs55_12p1%
 set ps_cs6_13_32=%ps_cs6_13%
)
echo.
echo I found these installed versions:
echo.
echo 1. Photoshop CS4 32 bit     %ps_cs4_11_32%
echo 2. Photoshop CS5 32 bit     %ps_cs5_12_32%
echo 3. Photoshop CS5.1 32 bit   %ps_cs55_12p1_32%
echo 4. Photoshop CS6 32 bit     %ps_cs6_13_32%
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo 5. Photoshop CS4 64 bit     %ps_cs4_11%
 echo 6. Photoshop CS5 64 bit     %ps_cs5_12%
 echo 7. Photoshop CS5.1 64 bit   %ps_cs55_12p1%
 echo 8. Photoshop CS6 64 bit     %ps_cs6_13%
)
set /p userinp=Pick the Photoshop to install into:
set userinp=%userinp:~0,1%
if "%userinp%"=="1" goto CS432
if "%userinp%"=="2" goto CS532
if "%userinp%"=="3" goto CS5132
if "%userinp%"=="4" goto CS632
if "%userinp%"=="5" goto CS464
if "%userinp%"=="6" goto CS564
if "%userinp%"=="7" goto CS5164
if "%userinp%"=="8" goto CS664
echo Invalid Choice, quitting.
goto END
:CS432
echo Photoshop CS4 32
set ps_install_path=%ps_cs4_11_32%
goto SDK
:CS532
echo Photoshop CS5 32
set ps_install_path=%ps_cs5_12_32%
goto SDK
:CS5132
echo Photoshop CS5.1 32
set ps_install_path=%ps_cs55_12p1_32%
goto SDK
:CS632
echo Photoshop CS6 32
set ps_install_path=%ps_cs6_13_32%
goto SDK
:CS464
echo Photoshop CS4 64
set ps_install_path=%ps_cs4_11%
goto SDK
:CS564
echo Photoshop CS5 64
set ps_install_path=%ps_cs5_12%
goto SDK
:CS5164
echo Photoshop CS5.1 64
set ps_install_path=%ps_cs55_12p1%
goto SDK
:CS664
echo Photoshop CS6 64
set ps_install_path=%ps_cs6_13%
goto SDK
:SDK
echo.
echo Which SDK build to install:
echo.
echo 1. Debug 32 bit
echo 2. Debug 64 bit
echo 3. Release 32 bit
echo 4. Release 64 bit
set /p userinp=Choose a number ( 1 - 4 ):
set userinp=%userinp:~0,1%
if "%userinp%"=="1" goto D32
if "%userinp%"=="2" goto D64
if "%userinp%"=="3" goto R32
if "%userinp%"=="4" goto R64
echo Invalid Choice, quitting.
goto END
:D32
echo Debug 32
set sdk_source_path=..\Output\Win\Debug\*.*
goto COPY
:D64
echo Debug 64
set sdk_source_path=..\Output\Win\Debug64\*.*
goto COPY
:R32
echo Release 32
set sdk_source_path=..\Output\Win\Release\*.*
goto COPY
:R64
echo Release 64
set sdk_source_path=..\Output\Win\Release64\*.*
goto COPY

:COPY
echo Copying from here %sdk_source_path% to here %ps_install_path%.
xcopy %sdk_source_path% %ps_install_path%. /E /F /H /R /D /C /Y

:END
set ps_cs4_11=
set ps_cs5_12=
set ps_cs55_12p1=
set ps_cs6_13=
set ps_cs4_11_32=
set ps_cs5_12_32=
set ps_cs55_12p1_32=
set ps_cs6_13_32=
set ps_install_path=
set sdk_source_path=
set userinp=
set WinVer=
set WinMajor=
set WinMinor=
set WinBuild=
set RegTokens=
set Var1=