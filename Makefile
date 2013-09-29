#############################################################################
#
# Makefile for nrf24 library
#
# License: MIT
# Author:  Erik Svensson <erik.public@gmail.com>
# Date:    2013-08
#

prefix := /usr/local

VERSION_MAJOR := 0
VERSION_MINOR := 1
VERSION_FIX := 0
GIT_REVISION := $(shell git describe --always --dirty 2> /dev/null)
SOURCES=src/sleep.c src/gpio.c src/spi.c src/nrf24.c src/decode.c
OBJECTS=$(SOURCES:.c=.o)
LIBNAME=libnrf24
LIBRARY=$(LIBNAME).so.$(VERSION_MAJOR).$(VERSION_MINOR)

CC=gcc
CCFLAGS=-c -Wall -pedantic -O3 -fpic -DNRF24_VERSION_MAJOR=$(VERSION_MAJOR) -DNRF24_VERSION_MINOR=$(VERSION_MINOR) -DNRF24_VERSION_FIX=$(VERSION_FIX)
LDFLAGS=-shared

ifneq ($(GIT_REVISION),)
CCFLAGS += -DNRF24_COMMIT='"$(GIT_REVISION)"'
else
CCFLAGS += -DNRF24_COMMIT=""
endif

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -Wl,-soname,$(LIBRARY) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CCFLAGS) -Iinclude $< -o $@

clean:
	rm -rf $(LIBRARY) $(OBJECTS)

install:
	cp $(LIBRARY) $(prefix)/lib/$(LIBRARY)
	ln -sf $(prefix)/lib/$(LIBRARY) $(prefix)/lib/$(LIBNAME).so
	ldconfig
