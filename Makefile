#############################################################################
#
# Makefile for nrf24 library
#
# License: GPL (General Public License)
# Author:  Erik Svensson <erik.public@gmail.com>
# Date:    2013-08
#

prefix := /usr/local

SOURCES=src/sleep.c src/gpio.c src/spi.c src/nrf24.c
OBJECTS=$(SOURCES:.c=.o)
LIBNAME=libnrf24
LIBRARY=$(LIBNAME).so.1.0

# The recommended compiler flags for the Raspberry Pi
CC=gcc
CCFLAGS=-c -Wall -pedantic -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
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
