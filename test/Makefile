# Makefile
#

TOPDIR = ../..

ifdef TCAMAKE_DEBUG
OPT_FLAGS= 	-g
endif

# custom lib/includes
INCLUDES=       -I../include
LFLAGS=		-L../lib
LIBS=		-ltcajson

BIN=		jsontest jsoncreate
OBJS=		jsontest.o jsoncreate.o

ALL_OBJS=	$(OBJS) 
ALL_BINS=	$(BIN)

all: jsontest jsoncreate

include $(TOPDIR)/tcamake/project_defs


jsontest: jsontest.o
	$(make-cxxbin-rule)
	@echo

jsoncreate: jsoncreate.o
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o
	@echo

distclean: clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TCAMAKE_PREFIX
	@echo
endif
