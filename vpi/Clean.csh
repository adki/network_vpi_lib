#!/bin/csh -f

if ( -e obj                  ) /bin/rm -rf obj
if ( -e work                 ) /bin/rm -rf work
if ( -e transcript           ) /bin/rm -f transcript
if ( -e wave.vcd             ) /bin/rm -f wave.vcd
if ( -e compile.log          ) /bin/rm -f compile.log
if ( -e vsim.wlf             ) /bin/rm -f vsim.wlf
if ( -e vish_stacktrace.vstf ) /bin/rm -f vish_stacktrace.vstf
if ( -e ethernet_log.txt     ) /bin/rm -f ethernet_log.txt
if ( -e fds.v                ) /bin/rm -f fds.v
if ( -e mm.v                 ) /bin/rm -f mm.v
if ( -e m.v                  ) /bin/rm -f m.v
if ( -e xx.v                 ) /bin/rm -f xx.v
if ( -e x.v                  ) /bin/rm -f x.v
if ( -e libnetwork_vpi.so ) \rm -f libnetwork_vpi.so
if ( -e network_vpi.dll   ) \rm -f network_vpi.dll
if ( -e network_vpi.exp   ) \rm -f network_vpi.exp
if ( -e network_vpi.lib   ) \rm -f network_vpi.lib
