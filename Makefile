CC = clang
CFLAGS = -fmodules -O3

VERSION = 0.1.1
DESTDIR =
prefix = /usr/local
bindir = $(prefix)/bin
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -m 755

all: build

build_x86_64:
	$(CC) $(CFLAGS) -arch x86_64 -o mset.x86_64 mset.c

build_arm64:
	$(CC) $(CFLAGS) -arch arm64 -o mset.arm64 mset.c

build: build_x86_64 build_arm64
	lipo -create -output mset mset.x86_64 mset.arm64
	rm mset.x86_64 mset.arm64

format:
	clang-format -i *.c

clean:
	$(RM) mset
	$(RM) mset-$(VERSION)-universal.zip

install: build
	$(INSTALL_PROGRAM) mset $(DESTDIR)$(bindir)/mset

uninstall:
	$(RM) $(DESTDIR)$(bindir)/mset

release: build
	zip -r mset-$(VERSION)-universal.zip mset LICENSE README.md

test: build
	./mset
	./mset -v
	./mset -h
	./mset -s 100
	./mset -s 120 -a 500000
	./mset -a 0
	./mset -s 190 -a 0
