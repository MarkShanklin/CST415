#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 1
#
# Author: Philip Howard
# Email:  phil.howard@oit.edu
# Date:   Sept 22, 2016
# Modified By: Mark Shanklin
#
COPTS = -g -O0 -Wall

OBJS = addr2str.o encode.o 

DEPS = nameserver.h encode.h addr2str.h

all: oit_ns nsclient 

clean:
	rm -f nsclient
	rm -f oit_ns
	rm -f *.o

.c.o:
	gcc $(COPTS) -c $? -o $@

oit_ns: oit_ns.c $(DEPS) $(OBJS)
	gcc $(COPTS) oit_ns.c -o oit_ns $(OBJS)

nsclient: nsclient.c $(DEPS) $(OBJS)
	gcc $(COPTS) nsclient.c -o nsclient $(OBJS)

