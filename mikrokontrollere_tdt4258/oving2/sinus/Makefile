CC=gcc
CFLAGS=-g -Wall --std=c99
LDFLAGS=-lm


####### RULES ##########################################################
.PHONY : all
all: 
	$(CC) $(CFLAGS) sinus.c -o sinus $(LDFLAGS)

# upload to STK1000 (program flash)
clean :
	rm -rf *.o sinus
