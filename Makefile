# tcajson Makefile

# set requirements
NEED_SOCKET = 1

ifdef TCAMAKE_DEBUG
OPT_FLAGS= 	-g
endif

OPT_FLAGS+= -fPIC -O2
CCSHARED+=	-Wl,-soname,$@
CXXFLAGS=	-std=c++23

INCLUDES=   -Iinclude
LIBS=
BIN=
OBJS=		src/JsonObject.o src/JsonArray.o src/JSON.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

# ---------------------------------------------

ifeq ($(TCAMAKE_HOME),)
	export TCAMAKE_HOME := $(shell realpath ../tcamake)
endif
ifeq ($(TCAMAKE_PROJECT),)
	export TCAMAKE_PROJECT := $(shell realpath ..)
endif

include ${TCAMAKE_HOME}/tcamake_include

# ---------------------------------------------

all: lib

lib: arlib
arlib: lib/libtcajson.a
solib: libtcajson.so.2.5.3

lib/libtcajson.a: $(OBJS)
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

libtcajson.so.2.5.3: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) lib/$@ lib/libtcajson.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libtcajson.so )
	@echo

.PHONY: test
test:
	( cd test; make all )
	@echo

documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

doc-clean:
	( cd docs; $(MAKE) clean )
	@echo

libclean:
	( $(RM) lib )
	@echo

testclean:
	( cd test; $(MAKE) distclean )
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o lib/*.bd
	@echo

distclean: clean libclean testclean doc-clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TCAMAKE_PREFIX
	@echo "Installing tcajson to $(TCAMAKE_PREFIX)/{include,lib}"
	$(MKDIR) $(TCAMAKE_PREFIX)/include/tcajson
	$(MKDIR) $(TCAMAKE_PREFIX)/lib
	$(RSYNC) --delete include/ $(TCAMAKE_PREFIX)/include/tcajson/
	$(RSYNC) lib/ $(TCAMAKE_PREFIX)/lib/
	@echo
endif
