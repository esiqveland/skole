CFLAGS=--std=c99 -O3
LDFLAGS=-lm -lfl
OBJ=my_parser.o parser.o scanner.o
PROGRAM=my_parser

.PHONY: clean all

all: $(PROGRAM)

$(PROGRAM): $(OBJ) 
	gcc -o $@ $(OBJ) $(LDFLAGS)

%.o:%.c
	gcc $(CFLAGS) -c $<

scanner.c: scanner.l parser.h
	flex -t $< > $@

parser.c: parser.y
	bison --defines=parser.h $< -o $@

parser.h: parser.o

clean:
	rm -f $(OBJ) $(PROGRAM) $(LEXOBJ) $(BISONOBJ) parser.h scanner.c parser.c

test: all
	./$(PROGRAM) < testprogram.calc
