TARGETS = \
	sample0 sample1 sample2 sample3 sample4 \
	sample5 sample6 sample7 sample8 sample9 \
	sample10 sample11

OBJS := $(TARGETS:=.o)

CC = gcc

GFLAGS = -O0 -Wall -g
CFLAGS = 
LFLAGS = 

.SUFFIXES:
.SUFFIXES: .o .c

all :	$(TARGETS)

.c.o :
	$(CC) $(GFLAGS) $(CFLAGS) $< -c -o $@

.o :
	$(CC) $< $(GFLAGS) $(LFLAGS) -o $@

$(TARGETS) :	$(OBJS)

clean :
	rm -f $(OBJS) $(TARGETS)
