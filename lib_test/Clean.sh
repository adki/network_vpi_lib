#!/bin/sh

if [ -d obj         ]; then \rm -rf obj       ; fi
if [ -f *.stackdump ]; then \rm -f *.stackdump; fi
if [ -f *.exe       ]; then \rm -f *.exe      ; fi
