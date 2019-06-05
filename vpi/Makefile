#----------------------------------------------------------------------------
# Copyright (c) 2019 by Ando Ki.
# All rights are reserved by Ando Ki.
#----------------------------------------------------------------------------
# Makefile
#------------------------------------------------------------------------
SHELL	= /bin/sh
ARCH	= $(shell uname)
MACH	= $(shell uname -m)
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
ifndef MODEL_TECH
   $(error MODEL_TECH should be defined)
else
ifeq ($(PLATFORM),linux)
      MODEL_TECH_INC = $(MODEL_TECH)/include
      MODEL_TECH_LIB = $(MODEL_TECH)/linux_$(MACH)
else ifeq ($(PLATFORM),cygwin)
      MODEL_TECH_INC = $(MODEL_TECH)/include
      ifeq ($(MACH),x86_64)
      MODEL_TECH_LIB = $(MODEL_TECH)/win64
      else
      MODEL_TECH_LIB = $(MODEL_TECH)/win32
      endif
else ifeq ($(PLATFORM),mingw)
      MODEL_TECH_INC = $(MODEL_TECH)/include
      ifeq ($(MACH),x86_64)
      MODEL_TECH_LIB = $(MODEL_TECH)/win64
      else
      MODEL_TECH_LIB = $(MODEL_TECH)/win32
      endif
else
      MODEL_TECH_INC = $(MODEL_TECH)/include
      MODEL_TECH_LIB = $(MODEL_TECH)/lib
endif
endif

#------------------------------------------------------------------------
VSIM	= $(shell which vsim)
STR	= $(shell $(VSIM) -version)
VVER    = $(shell for S in $(STR); do\
                if [ "$${NN}" = "vsim" ]; then\
                        echo $$S;\
                fi;\
                NN=$$S;\
        done)

#------------------------------------------------------------------------
ifndef OBJECTDIR
  OBJECTDIR = obj
endif
ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

#------------------------------------------------------------------------
TARGET_DIR   = ../vpi_lib/modelsim/$(VVER)/$(PLATFORM)_$(MACH)
ifeq ($(PLATFORM),linux)
   TARGET_VPI	= libnetwork_vpi.so
else ifeq ($(PLATFORM),cygwin)
   TARGET_VPI	= network_vpi.dll
else ifeq ($(PLATFORM),mingw)
   TARGET_VPI	= network_vpi.dll
else
   TARGET_VPI	= network_vpi.so
endif

#------------------------------------------------------------------------
SRCS	= network_vpi_lib.c\
		eth_ip_udp_tcp_pkt.c\
		ptpv2_message.c
OBJS	= $(SRCS:.c=.o)

#------------------------------------------------------------------------
vpath %.h	$(MODEL_TECH)/include:src
vpath %.c	src

#------------------------------------------------------------------------
GXX	= gcc
#GXX	= i686-pc-mingw32-gcc

#------------------------------------------------------------------------
C_CFLAGS = -g -O0 -Werror $(C_USER_DEFS)
C_LFLAGS = -O0 

ifeq ($(PLATFORM),linux)
   C_USER_DEFS = -DRIGOR
   C_CFLAGS   += -fPIC
else ifeq ($(PLATFORM),cygwin)
   #C_USER_DEFS = -DRIGOR -DWIN32 -DUNSUPPORT_MISALIGN -mno-cygwin
   C_USER_DEFS = -DRIGOR -DWIN32 -DUNSUPPORT_MISALIGN
   C_CFLAGS   += -fPIC
else ifeq ($(PLATFORM),mingw)
   C_USER_DEFS = -DRIGOR -DWIN32
   C_CFLAGS   += -fPIC
else
   C_USER_DEFS =
endif

CFLAGS      = $(C_CFLAGS) -I$(MODEL_TECH_INC) -Isrc
LDFLAGS     = $(C_LFLAGS)
LDLIBS      =

#------------------------------------------------------------------------
$(OBJECTDIR)/%.o: %.c
	($(GXX) -c $(CFLAGS) $< -o $@ || exit 1) 2>&1 | tee -a compile.log

#------------------------------------------------------------------------
.PHONY: all
all: pre $(TARGET_VPI)

pre:
	@if [ -f compile.log ] ; then /bin/rm -f compile.log; fi

#------------------------------------------------------------------------
$(TARGET_VPI): $(addprefix $(OBJECTDIR)/,$(OBJS))
    ifeq ($(PLATFORM),linux)
		($(GXX) -shared -Bsymbolic -o $(TARGET_VPI) $(addprefix $(OBJECTDIR)/,$(OBJS))\
				-L$(MODEL_TECH_LIB) -lmtipli) 2>&1 | tee -a compile.log
    else ifeq ($(PLATFORM),cygwin)
		($(GXX) -shared -o $(TARGET_VPI) $(addprefix $(OBJECTDIR)/,$(OBJS))\
				$(MODEL_TECH_LIB)/mtipli.dll) 2>&1 | tee -a compile.log
    else ifeq ($(PLATFORM),mingw)
		($(GXX) -shared -o $(TARGET_VPI) $(addprefix $(OBJECTDIR)/,$(OBJS))\
				$(MODEL_TECH_LIB)/mtipli.dll) 2>&1 | tee -a compile.log
    else
		($(GXX) -shared -Bsymbolic -o $(TARGET_VPI) $(addprefix $(OBJECTDIR)/,$(OBJS))\
				-L$(MODEL_TECH_LIB) -lmtipli) 2>&1 | tee -a compile.log
    endif

#------------------------------------------------------------------------
install: $(TARGET_VPI)
	@if [ -f $(TARGET_VPI) ]; then\
		if [ ! -d $(TARGET_DIR) ]; then\
			mkdir -p $(TARGET_DIR);\
		fi;\
		cp $(TARGET_VPI) $(TARGET_DIR);\
	else\
		echo $(TARGET_VPI) not found;\
	fi

#------------------------------------------------------------------------
.PHONY: clean
clean:
	\rm -rf $(OBJECTDIR)
	\rm -f  *.log
	\rm -f  *.o
	\rm -f *stackdump
	\rm -f *.exe.core
	\rm -f compile.log
	\rm -f network_vpi.dll network_vpi.lib network_vpi.exp
	\rm -f libnetwork_vpi.so

.PHONY: cleanup clobber
cleanup clobber: clean
	\rm -f $(TARGET_VPI)

.PHONY: cleanupall
cleanupall: cleanup

#----------------------------------------------------------------------------
# Revision history
#
# 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
#----------------------------------------------------------------------------
