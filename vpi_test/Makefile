# Makefile for ModelSim VPI
SHELL   = /bin/sh
ARCH    = $(shell uname)
MACH    = $(shell uname -m)
ifeq ($(ARCH),Linux)
      PLATFORM=linux
else ifeq ($(findstring CYGWIN,$(ARCH)),CYGWIN)
      PLATFORM=cygwin
else ifeq ($(findstring MINGW,$(ARCH)),MINGW)
      PLATFORM=mingw
else
  $(error un-supported platform $(ARCH))
endif

#------------------------------------------------------------------------
VSIM    = $(shell which vsim)
STR     = $(shell $(VSIM) -version)
VVER    = $(shell for S in $(STR); do\
                if [ "$${NN}" = "vsim" ]; then\
                        echo $$S;\
                fi;\
                NN=$$S;\
        done)

#-------------------------------------------------------------------------------
DIR_VPI_SRC=../vpi
DIR_VPI_LIB=../vpi_lib/modelsim/$(VVER)/$(PLATFORM)_$(MACH)
ifeq ($(PLATFORM),linux)
VPI_LIB=libnetwork_vpi.so
else ifeq ($(PLATFORM),cygwin)
VPI_LIB=network_vpi.dll
else ifeq ($(PLATFORM),mingw)
VPI_LIB=network_vpi.dll
else
VPI_LIB=libnetwork_vpi.so
endif

WORK=work
TOP=top

#-------------------------------------------------------------------------------
unexport PLIOBJS

#-------------------------------------------------------------------------------
all: vpi vlib vlog vsim

vpi:
	if [ -f $(DIR_VPI_LIB)/$(VPI_LIB) ]; then\
		make -C $(DIR_VPI_SRC) install;\
	fi

vlib:
	if [ -f compile.log ]; then /bin/rm -f compile.log; fi
	if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi
	vlib $(WORK) 2>&1 | tee -a compile.log

vlog:
	vlog -work $(WORK) top.v 2>&1 | tee -a compile.log

vsim:
	vsim -pli $(DIR_VPI_LIB)/$(VPI_LIB) -novopt -c -do "run -all; quit"\
			-lib $(WORK) $(WORK).$(TOP) 2>&1 | tee -a compile.log

#-------------------------------------------------------------------------------
clean:
	/bin/rm -rf $(WORK)
	/bin/rm -f  transcript compile.log
	/bin/rm -f  wave.vcd
	/bin/rm -f  wave.wlf
	/bin/rm -f  modelsim.ini
	/bin/rm -f  vish_stacktrace.vstf
	/bin/rm -f  vsim_stacktrace.vstf

cleanup clobber: clean

cleanupall: cleanup
