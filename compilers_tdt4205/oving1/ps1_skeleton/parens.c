#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef INIT_STACKSIZE
#define INIT_STACKSIZE 2
#endif


/*
 * Define a structure for tracking positions in text in
 * terms of line number and position on line
 */
typedef struct {
    int32_t line, count;
} position_t;

/* Pointer to the array we will use as a stack */
position_t *parens;

/* Controls the return value at program termination */
bool input_ok = true;

int32_t size = INIT_STACKSIZE;
int32_t top = -1;


void push ( position_t p )
{
    /* TODO:
     * Put 'p' at the top of the 'parens' stack.
     * Grow the size of the stack if it is too small.
     */
	if(top == size-1) { // need to reallocate to make stack bigger
		parens = (position_t*)realloc(parens, sizeof(position_t)*2*size);
		if( parens == NULL ) {
			fprintf(stderr, "No mem left. Exiting...");
			exit(1);
		} 
		size = size + size;
	}

	top++;
	parens[top] = p;
}


bool pop ( position_t *p )
{
    /* TODO:
     * Take the top element off of the stack, and put it in the
     * location at 'p'. If the stack shrinks below zero, there's been
     * more ')'-s than '('-s, so return false to let caller know the stack
     * was empty.
     */
	if(top == -1) {
		return false;
	}

	p = &parens[top];
	top--;
	return true;
}


void check ( void )
{
    /* TODO:
     * This is called after all input has been read.
     * If there are any elements left on stack, there have been more '('-s
     * than ')'-s. Emit suitable error message(s) on std. error, and cause
     * the program to return a failure code
     */
	
	if(top < -1) {
		fprintf(stderr, "\nError, unmatching parens!!!\n");
		input_ok = false;
	} else if (top > -1) {
		for(int i = top; i > -1; i--) {
			printf("Unmatched open paren at line: %d char: %d\n", parens[i].line, parens[i].count);
		}
		input_ok = false;
	} else {
		fprintf(stdout, "\nNo, unmatching parens! D:\n");
	}


}



int main ( int argc, char **argv )
{
    int32_t c = getchar();

    position_t now = { .line = 1, .count = 1 };   /* Track where we are in the input */
    position_t balance;                           /* Space for the matching position
                                               when parentheses are closed */
    position_t* temp; 
    bool couldpop;
    parens = (position_t *) malloc ( size * sizeof(position_t) );
    while ( ! feof(stdin) )
    {
        /* TODO:
         * - Manipulate the stack according to parentheses
         * - Update 'now' position according to read chars and line breaks
         */
	if(c == '\n') {
		now.count = 0;
		now.line++;
	} else if(c == '(') {
		balance=now;
		push(balance);
	} else if(c == ')') {
		if(!pop(temp)) {
			printf("Unmatched close paren at line: %d char: %d\n", now.line, now.count);
			input_ok = false;
		}
		/*
		if( ((*temp).line == balance.line) && ((*temp).count == balance.count) ) {
			fprintf(stdout, "herpderp\n");
		} else {
			fprintf(stdout, "Matching paren was not correct at line: ");
			fprintf(stdout, " char: ");
			fprintf(stdout, "\n");
		}
		*/
	}
        now.count += 1;
        c = getchar();

    }
    check();
    printf ( "Total of %d lines\n", now.line-1 );
    free ( parens );
    exit ( (input_ok) ? EXIT_SUCCESS : EXIT_FAILURE );
}


