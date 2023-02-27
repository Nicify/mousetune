CC = clang
CFLAGS = -fmodules -O3 -framework IOKit -framework CoreFoundation

DESTDIR =
prefix = /usr/local
bindir = $(prefix)/bin
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -m 755

all: build

build: mset

format:
	clang-format -i *.c

clean:
	$(RM) mset

install: build
	$(INSTALL_PROGRAM) mset $(DESTDIR)$(bindir)/mset

uninstall:
	$(RM) $(DESTDIR)$(bindir)/mset
