CC = clang
CFLAGS = -fmodules -O3

DESTDIR =
prefix = /usr/local
bindir = $(prefix)/bin
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -m 755

all: build

build_x86_64:
	$(CC) $(CFLAGS) -arch x86_64 -o mousetune.x86_64 main.c

build_arm64:
	$(CC) $(CFLAGS) -arch arm64 -o mousetune.arm64 main.c

build: clean build_x86_64 build_arm64
	lipo -create -output mousetune mousetune.x86_64 mousetune.arm64
	rm mousetune.x86_64 mousetune.arm64

format:
	clang-format -i *.c

clean:
	$(RM) mousetune-$(shell ./mousetune -v)-universal.zip
	$(RM) mousetune

install: build
	$(INSTALL_PROGRAM) mousetune $(DESTDIR)$(bindir)/mousetune

uninstall:
	$(RM) $(DESTDIR)$(bindir)/mousetune

# add launch service
launchd-add:
	cp launch-agent.plist ~/Library/LaunchAgents/com.relicx-me.mousetune.plist
	launchctl load ~/Library/LaunchAgents/com.relicx-me.mousetune.plist

# remove launch service
launchd-remove:
	launchctl unload ~/Library/LaunchAgents/com.relicx-me.mousetune.plist
	$(RM) ~/Library/LaunchAgents/com.relicx-me.mousetune.plist

release: build test
	zip -r mousetune-$(shell ./mousetune -v)-universal.zip mousetune launch-agent.plist LICENSE README.md

test:
	./mousetune
	./mousetune -v
	./mousetune -h
	./mousetune -s 100
	./mousetune -s 120 -a 500000
	./mousetune -a 0
	./mousetune -s 190 -a 0
