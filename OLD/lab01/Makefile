#**************************************************
# Makefile for OIT lab
# 
# Created By: Mark Shanklin
# Email: mark.shanklin@oit.edu
# Date: Sept 27, 2016
# 
CC = gcc
COPTS = -g -O0 -Wall -std=c99
PROG = DNS_server
PROG2 = DNS_client
PROG3 = clearport

SOURCES = $(PROG).c
SOURCES2 = $(PROG2).c
SOURCES3 = $(PROG3).c
DEPS = *.h

LAB := $(shell basename ${PWD})
TAR_FILE := $(LAB)_${LOGNAME}.tar

all: $(PROG) $(PROG2) $(PROG3)

.c.o:
	$(CC) $(COPTS) -c $? -o $@

$(PROG): $(SOURCES) $(DEPS)
	$(CC) $(COPTS) -o $@ $(SOURCES)

$(PROG2): $(SOURCES2) $(DEPS)
	$(CC) $(COPTS) -o $@ $(SOURCES2)

$(PROG3): $(SOURCES3) $(DEPS)
	$(CC) $(COPTS) -o $@ $(SOURCES3)

clean:
	rm -f $(PROG)
	rm -f $(PROG2)
	rm -f $(PROG3)
	rm -f *.o

tar:
	rm -f $(TAR_FILE)
	tar cvf $(TAR_FILE) *.[ch] [Mm]akefile
