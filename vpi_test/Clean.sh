#!/bin/sh

if [ -d work                 ]; then \rm -rf work;                 fi
if [ -f transcript           ]; then \rm -f  transcript;           fi
if [ -f wave.vcd             ]; then \rm -f  wave.vcd;             fi
if [ -f wave.wlf             ]; then \rm -f  wave.wlf;             fi
if [ -f vish_stacktrace.vstf ]; then \rm -f  vish_stacktrace.vstf; fi
if [ -f vsim_stacktrace.vstf ]; then \rm -f  vsim_stacktrace.vstf; fi
if [ -f compile.log          ]; then \rm -f  compile.log         ; fi
