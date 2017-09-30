############################################################
# File Name     : Makefile
# Purpose       : Used to Compile Program  
# Creation Date : 09-30-2017
# Last Modified : Sat 30 Sep 2017 11:11:08 AM PDT
# Created By    : Mark Shanklin 
############################################################

CC = gcc -g -std=c99
CFLAGS = -Wall 
PROG = labOne
SOURCES := $(shell echo *.c)
DEPS := $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
DEPENDS = depends.mk
$(shell touch ${DEPENDS})

all: depends $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(PROG) *.o $(DEPENDS)

include $(DEPENDS)

depends:
	gcc -MM $(SOURCES) > $(DEPENDS)
