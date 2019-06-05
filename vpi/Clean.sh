#!/bin/sh

if [ -d obj                  ]; then \rm -rf obj;                 fi
if [ -d work                 ]; then \rm -rf work;                fi
if [ -f transcript           ]; then \rm -f transcript;           fi
if [ -f wave.vcd             ]; then \rm -f wave.vcd;             fi
if [ -f compile.log          ]; then \rm -f compile.log;          fi
if [ -f vsim.wlf             ]; then \rm -f vsim.wlf;             fi
if [ -f vish_stacktrace.vstf ]; then \rm -f vish_stacktrace.vstf; fi
if [ -f ethernet_log.txt     ]; then \rm -f ethernet_log.txt;     fi
if [ -f fds.v                ]; then \rm -f fds.v;                fi
if [ -f mm.v                 ]; then \rm -f mm.v;                 fi
if [ -f m.v                  ]; then \rm -f m.v;                  fi
if [ -f xx.v                 ]; then \rm -f xx.v;                 fi
if [ -f x.v                  ]; then \rm -f x.v;                  fi
if [ -f libnetwork_vpi.so ]; then \rm -f libnetwork_vpi.so; fi
if [ -f network_vpi.dll   ]; then \rm -f network_vpi.dll  ; fi
if [ -f network_vpi.exp   ]; then \rm -f network_vpi.exp  ; fi
if [ -f network_vpi.lib   ]; then \rm -f network_vpi.lib  ; fi

/bin/rm -f wlft*
