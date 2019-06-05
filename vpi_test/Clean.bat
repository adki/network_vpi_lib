@ECHO OFF

SET MODELSIMWORK=work

IF EXIST %MODELSIMWORK%       RMDIR /S/Q %MODELSIMWORK%
IF EXIST transcript           DEL   /Q   transcript
IF EXIST wave.vcd             DEL   /Q   wave.vcd
IF EXIST vish_stacktrace.vstf DEL   /Q   vish_stacktrace.vstf
IF EXIST J3_*out.dat          DEL   /Q   J3_*out.dat
IF EXIST Flash*out.dat        DEL   /Q   Flash*out.dat
IF EXIST vsim_stacktrace.vstf DEL   /Q   vsim_stacktrace.vstf
IF EXIST vsim.wlf             DEL   /Q   vsim.wlf            
IF EXIST wave.wlf             DEL   /Q   wave.wlf            
IF EXIST compile.log          DEL   /Q   compile.log

DEL /Q wlf*
