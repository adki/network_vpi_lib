DIRS	= $(subst /,, $(dir $(wildcard */Makefile)))

all install:
	make -C vpi $@

clean cleanup clobber:
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done

cleanupall:
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done
	/bin/rm -rf vpi_lib

.PHONY: all clean cleanup clobber
