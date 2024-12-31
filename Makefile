CC := gcc
PREFIX := /usr/local
CCFLAGS := -pedantic -Wall -Wno-deprecated-declarations -Os
LDFLAGS := -lX11

all: options dwmblocks

options:
	@echo dwmblocks build options:
	@echo "CCFLAGS = $(CCFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"

dwmblocks: dwmblocks.c config.h
	$(CC) -o dwmblocks dwmblocks.c $(CCFLAGS) $(LDFLAGS)

clean:
	rm -f dwmblocks

install: dwmblocks
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dwmblocks $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dwmblocks

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dwmblocks

.PHONY: all options clean install uninstall
