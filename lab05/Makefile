############################################################
# File Name     : Makefile
# Purpose       :   
# Creation Date : 11-07-2017
# Last Modified : Tue 07 Nov 2017 06:21:52 PM PST
# Created By    : Mark Shanklin 
############################################################

C = gcc -g
CFLAGS = -Wall -std=c11
PROG = robConn 
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
