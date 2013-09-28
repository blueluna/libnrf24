#############################################################################
#
# Makefile for nrf24 library
#
# License: MIT
# Author:  Erik Svensson <erik.public@gmail.com>
# Date:    2013-08
#

prefix := /usr/local

SOURCES=src/sleep.c src/gpio.c src/spi.c src/nrf24.c src/decode.c
OBJECTS=$(SOURCES:.c=.o)
LIBNAME=libnrf24
LIBRARY=$(LIBNAME).so.1.0

CC=gcc
CCFLAGS=-c -Wall -pedantic -O3 -fpic
LDFLAGS=-shared

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -Wl,-soname,$(LIBNAME).so.1 $(OBJECTS) -o $@

.c.o:
	$(CC) $(CCFLAGS) -Iinclude $< -o $@

clean:
	rm -rf $(LIBRARY) $(OBJECTS)

install:
	cp $(LIBRARY) $(prefix)/lib/$(LIBRARY)
	ln -sf $(prefix)/lib/$(LIBRARY) $(prefix)/lib/$(LIBNAME).so.1
	ln -sf $(prefix)/lib/$(LIBRARY) $(prefix)/lib/$(LIBNAME).so
	ldconfig
