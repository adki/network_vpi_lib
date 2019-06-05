@ECHO OFF

IF EXIST obj         RMDIR /S/Q obj
IF EXIST *.stackdump DEL   /Q   *.stackdump
IF EXIST *.exe       DEL   /Q   *.exe
