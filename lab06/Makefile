#**************************************************
# Makefile for OIT lab
# 
# Created By: Mark Shanklin
# Email: mark.shanklin@oit.edu
# Date: Sept 27, 2016
# 
CC = gcc
COPTS = -g -O0 -Wall -std=gnu11
PROG = server
PROG2 = client

SOURCES = $(PROG).c
SOURCES2 = $(PROG2).c
DEPS = nameserver.h getport.o tscounter.o

LAB := $(shell basename ${PWD})
TAR_FILE := $(LAB)_${LOGNAME}.tar

all: $(PROG) $(PROG2)

.c.o:
	$(CC) $(COPTS) -c $? -o $@

$(PROG): $(SOURCES) $(DEPS)
	$(CC) $(COPTS) -o $@ $(SOURCES)

$(PROG2): $(SOURCES2) $(DEPS)
	$(CC) $(COPTS) -o $@ $(SOURCES2)

clean:
	rm -f $(PROG)
	rm -f $(PROG2)
	rm -f *.o

tar:
	rm -f $(TAR_FILE)
	tar cvf $(TAR_FILE) *.[ch] [Mm]akefile