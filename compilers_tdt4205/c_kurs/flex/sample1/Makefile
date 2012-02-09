CFLAGS=--std=c99 -O3
LDFLAGS=-lm -lfl
OBJ=$(subst .c,.o,$(shell ls *.c))
LEXOBJ=$(subst .l,.o,$(shell ls *.l))
PROGRAM=my_scanner

.PHONY: clean all

all: $(PROGRAM)

$(PROGRAM): $(OBJ) $(LEXOBJ)
	gcc -o $@ $(OBJ) $(LDFLAGS)

%.o:%.c
	gcc $(CFLAGS) -c $<

%.c:%.l
	flex -t $< > $@

clean:
	rm -f $(OBJ) $(PROGRAM) $(LEXOBJ)

test: all
	./$(PROGRAM) < testprogram.calc
