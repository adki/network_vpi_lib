@ECHO OFF
REM Copyright (c) 2019 Ando Ki.
REM All right reserved.
REM

SETLOCAL
SET WORK=work
SET VLIB=vlib
SET VSIM=vsim
SET VCOM=vcom
SET VLOG=vlog
SEt PLIOBJS=
SEt VPI=../vpi_lib/modelsim/10.3/MS64/network_vpi.dll

SET TOP=top

IF EXIST %WORK% RMDIR /S/Q %WORK%

%VLIB% %WORK%
IF %errorlevel% NEQ 0 GOTO :END
@REM %VLOG% -work %WORK% top.v > compile.log 2>&1
%VLOG% -work %WORK% top.v
IF %errorlevel% NEQ 0 GOTO :END
@REM %VSIM% -pli %VPI% -novopt -c -do "run -all; quit" %WORK%.top >> compile.log 2>&1
%VSIM% -pli %VPI% -novopt -c -do "run -all; quit" %WORK%.top
IF %errorlevel% NEQ 0 GOTO :END

:END
ENDLOCAL
