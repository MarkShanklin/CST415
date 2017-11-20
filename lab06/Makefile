############################################################
# File Name     : Makefile
# Purpose       : DNS Resolver  
# Creation Date : 11-15-2017
# Created By    : Mark Shanklin 
############################################################

CC = gcc -g -std=gnu11
CFLAGS = -Wall 
PROG = labSix
SOURCES := $(shell echo *.c)
DEPS := $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
DEPENDS = depends.mk
$(shell touch ${DEPENDS})

LAB := $(shell basename ${PWD})
TAR_FILE := $(LAB)_${LOGNAME}.tar

all: depends $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(PROG) *.o $(DEPENDS)

include $(DEPENDS)

depends:
	gcc -MM $(SOURCES) > $(DEPENDS)

tar:
	rm -f $(TAR_FILE)
	tar cvf $(TAR_FILE) *.[ch] [Mm]akefile