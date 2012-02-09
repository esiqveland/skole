CFLAGS=--std=c99 -O3
LDFLAGS=-lm
OBJ=$(subst .c,.o,$(shell ls *.c))
PROGRAM=my_program

.PHONY: clean all

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	gcc -o $@ $(OBJ) $(LDFLAGS)

my_program.o: my_header1.h my_header2.h mylibrary.h

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(PROGRAM)
