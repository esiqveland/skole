%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodetypes.h"
#include "tree.h"

/* This defines the type for every $$ value in the productions. */
#define YYSTYPE node_t *

/*
 * Variables connecting the parser to the state of the scanner - defs. will be
 * generated as part of the scanner (lexical analyzer).
 */
extern char yytext[];
extern int yylineno;


/*
 * Since the return value of yyparse is an integer (as defined by yacc/bison),
 * we need the top level production to finalize parsing by setting the root
 * node of the entire syntax tree inside its semantic rule instead. This global
 * variable will let us get a hold of the tree root after it has been
 * generated.
 */
node_t *root;


/*
 * These functions are referenced by the generated parser before their
 * definition. Prototyping them saves us a couple of warnings during build.
 */
int yyerror ( const char *error );  /* Defined below */
int yylex ( void );                 /* Defined in the generated scanner */
%}


/* Tokens for all the key words in VSL */
%token NUMBER STRING IDENTIFIER FUNC PRINT RETURN CONTINUE EPSILON
%token PLUS MINUS MUL DIV POWER ASSIGNMENT
%token IF THEN ELSE FI WHILE DO DONE VAR COMMENT


/*
 * Operator precedences: 
 * + and - bind to the left { a+b+c = (a+b)+c }
 * * and / bind left like + and -, but have higher precedence
 * Unary minus has only one operand (and thus no direction), but highest
 * precedence. Since we've already used '-' for the binary minus, unary minus
 * needs a ref. name and explicit setting of precedence in its grammar
 * production.
 */

// %left '+' '-'
%left PLUS MINUS
//%left '*' '/'
%left MULT DIV
%nonassoc UMINUS


/*
 * The grammar productions follow below. These are mostly a straightforward
 * statement of the language grammar, with semantic rules building a tree data
 * structure which we can traverse in subsequent phases in order to understand
 * the parsed program. (The leaf nodes at the bottom need somewhat more
 * specific rules, but these should be manageable.)
 * A lot of the work to be done later could be handled here instead (reducing
 * the number of passes over the syntax tree), but sticking to a parser which
 * only generates a tree makes it easier to rule it out as an error source in
 * later debugging.
 */ 


/*
program: '+' {
    node_init ( root = malloc(sizeof(node_t)), program_n, NULL, 1, $1);
    
program: expr { printf("Answer is %d\n", $1); };
};*/

%%
program: function_list

function_list: function 
	     | function_list function

statement_list: statement 
	      | statement_list statement

print_list: print_item 
	  | print_list ", " print_item

expression_list: expression 
	       | expression_list ", " expression

variable_list: variable
	     | indexed_variable 
	     | variable_list ", " variable
  	     | variable_list ", " indexed_variable

argument_list: expression_list
	     | EPSILON

parameter_list: variable_list
	      | EPSILON

declaration_list: declaration_list declaration 
		| EPSILON

function: FUNC variable "(" parameter_list ")" statement

statement: assignment_statement 
	 | return_statement 
	 | print_statement 
	 | null_statement 
	 | if_statement 
	 | while_statement 
	 | block

block: "{" declaration_list statement_list "}"

assignment_statement: variable ASSIGNMENT expression
		    | variable "[" expression "]" ASSIGNMENT expression

return_statement: RETURN expression

print_statement: PRINT print_list

null_statement: CONTINUE

if_statement: IF expression THEN statement FI
	    | IF expression THEN statement ELSE statement FI

while_statement: WHILE expression DO statement DONE


expression: expression PLUS expression { $$ = $1 + $3; }
    | expression MINUS expression { $$ = $1 - $3; }
    | expression MULT expression { $$ = $1 * $3; }  
    | expression DIV expression { $$ = $1 / $3; }
    | MINUS expression
    | expression POWER expression
    | "(" expression ")"
    | integer
    | variable
    | variable "(" argument_list ")" 
    | variable "[" expression "]"

declaration: VAR variable_list

variable: IDENTIFIER

indexed_variable: variable "[" integer "]"

integer: NUMBER

print_item: expression
	  | text

text: STRING

%%


/*
 * This function is called with an error description when parsing fails.
 * Serious error diagnosis requires a lot of code (and imagination), so in the
 * interest of keeping this project on a manageable scale, we just chuck the
 * message/line number on the error stream and stop dead.
 */
int
yyerror ( const char *error )
{
    fprintf ( stderr, "\tError: %s detected at line %d\n", error, yylineno );
    exit ( EXIT_FAILURE );
}


