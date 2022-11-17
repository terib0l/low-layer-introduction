TARGETS = main

OBJS := $(TARGETS:=.o)

CC = gcc

GFLAGS = -O0 -Wall -g
CFLAGS =
LFLAGS =

.SUFFIXES:
.SUFFIXES: .o .c

all : $(TARGETS)

.c.o :
	$(CC) $(GFLAGS) $(CFLAGS) $< -c -o $@

.o :
	$(CC) $< $(GFLAGS) $(LFLAGS) -o $@

$(TARGETS) : $(OBJS)

clean :
	rm -f $(OBJS) $(TARGETS)
