@ECHO OFF
REM CMD -x64|x86 -help

@SET MACH=x64

:LOOP
IF NOT "%1"=="" (
   IF "%1"=="-x86" (
      SET MACH=x86
   ) ELSE IF "%1"=="-x64" (
      SET MACH=x64
   ) ELSE IF "%1"=="-help" (
      ECHO "%0 -x86|x64 -help"
      EXIT
   ) ELSE (
      SET MACH=x64
   )
   SHIFT
   GOTO :LOOP
)

SET VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"

IF "%MACH%"=="x86" (
   %VCVARSALL% x86
) ELSE (
   %VCVARSALL% x64
)

REM ===============================
REM "below does not work"
echo "nmake -f NMAKEFILE MACH=%MACH%"
nmake -f NMAKEFILE MACH=%MACH%

REM pause
