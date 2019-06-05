#!/bin/csh -f

if ( -e work                 ) \rm -rf work
if ( -e transcript           ) \rm -f  transcript
if ( -e wave.vcd             ) \rm -f  wave.vcd
if ( -e wave.wlf             ) \rm -f  wave.wlf
if ( -e vish_stacktrace.vstf ) \rm -f  vish_stacktrace.vstf
if ( -e vsim_stacktrace.vstf ) \rm -f  vsim_stacktrace.vstf
if ( -e compile.log          ) \rm -f  compile.log         
