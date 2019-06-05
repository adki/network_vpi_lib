@ECHO OFF

IF EXIST obj            RMDIR /S/Q obj
IF EXIST work           RMDIR /S/Q work
IF EXIST transcript     DEL   /Q   transcript
IF EXIST wave.vcd       DEL   /Q   wave.vcd
IF EXIST compile.log    DEL   /Q   compile.log
IF EXIST vsim.wlf       DEL   /Q   vsim.wlf
IF EXIST vish_stacktrace.vstf DEL /Q vish_stacktrace.vstf
IF EXIST ethernet_log.txt     DEL /Q ethernet_log.txt
IF EXIST fds.v                DEL /Q fds.v
IF EXIST mm.args              DEL /Q mm.args
IF EXIST m.args               DEL /Q m.args
IF EXIST xx.args              DEL /Q xx.args
IF EXIST x.args               DEL /Q x.args
IF EXIST wlft*                DEL /Q wlft*
IF EXIST libnetwork_vpi.so    DEL /Q libnetwork_vpi.so
IF EXIST network_vpi.dll      DEL /Q network_vpi.dll
IF EXIST network_vpi.exp      DEL /Q network_vpi.exp
IF EXIST network_vpi.lib      DEL /Q network_vpi.lib
