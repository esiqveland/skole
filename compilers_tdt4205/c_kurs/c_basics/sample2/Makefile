CFLAGS=--std=c99 -O3
LDFLAGS=-lm
OBJ=$(subst .c,.o,$(shell ls *.c))
PROGRAM=varargs

.PHONY: clean all

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	gcc -o $@ $(OBJ) $(LDFLAGS)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(PROGRAM)
