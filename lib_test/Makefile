#-------------------------------------------------------------
# Makefile
#-------------------------------------------------------------
SHELL= /bin/sh
#--------------------------------------------------------
ARCH= $(shell uname -s)
MACH= $(shell uname -m)
ifeq ($(ARCH), Linux)
	PLATFORM= linux
else ifeq ($(findstring CYGWIN,$(ARCH)), CYGWIN)
	PLATFORM= cygwin
else ifeq ($(findstring MINGW,$(ARCH)), MINGW)
	PLATFORM= mingw
else
       $(error $(ARCH) not supported)
endif
#-------------------------------------------------------------
CC   = gcc
#-------------------------------------------------------------
PROG = test
SRCS = main.c test_checksum.c eth_ip_udp_tcp_pkt.c
OBJS = $(SRCS:.c=.o)
#-------------------------------------------------------------
INCS = -Isrc -I../vpi/src
LIBS =
#-------------------------------------------------------------
ifeq ($(PLATFORM), linux)
INCS +=
LIBS +=
else ifeq ($(PLATFORM), cygwin)
INCS +=
LIBS +=
else ifeq ($(PLATFORM), mingw)
INCS +=
LIBS +=
endif
#-------------------------------------------------------------
CFLAGS = -g -O ${INCS}
LDFLAGS= ${LIBS}
#-------------------------------------------------------------
vpath %.h	src:../vpi/src
vpath %.c	src:../vpi/src
#-------------------------------------------------------------
ifndef OBJECTDIR
  OBJECTDIR = obj
endif
ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

$(OBJECTDIR)/%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $< 2>&1 | tee -a compile.log

#-------------------------------------------------------------
all: pre $(PROG)

pre:
	if [ -f compile.log ]; then /bin/rm -f compile.log; fi

$(PROG): $(addprefix $(OBJECTDIR)/, $(OBJS))
	${CC} -o ${PROG} $^ ${LDFLAGS} 2>&1 | tee -a compile.log

run: $(PROG)
	if [ -f run.log ]; then /bin/rm -f run.log; fi
	./$(PROG) 2>&1 | tee run.log
#-------------------------------------------------------------
clean:
	-rm -f  ${OBJS}
	-rm -fr ${OBJECTDIR}
	-rm -f  *stackdump
	-rm -f  compile.log run.log
	-rm -f ${PROG}.exe ${PROG}

cleanup clobber: clean

cleanupall: cleanup
#-------------------------------------------------------------
