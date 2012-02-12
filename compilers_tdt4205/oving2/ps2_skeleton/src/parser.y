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
%left MUL DIV
%right POWER
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
    
};
program: expr { printf("Answer is %d\n", $1); };
*/

%%
program: function_list { node_init ( root = malloc(sizeof(node_t)), program_n, NULL, 1, $1); }
       ;

function_list: function { node_init ( $$ = malloc(sizeof(node_t)), function_list_n, NULL, 1, $1); }
	     | function_list function { node_init ( $$ = malloc(sizeof(node_t)), function_list_n, NULL, 2, $1, $2); }
             ;

statement_list: statement { node_init ( $$ = malloc(sizeof(node_t)), statement_list_n, NULL, 1, $1); }
	      | statement_list statement { node_init ( $$ = malloc(sizeof(node_t)), statement_list_n, NULL, 2, $1, $2); }
              ;


print_list: print_item { node_init ( $$ = malloc(sizeof(node_t)), print_list_n, NULL, 1, $1); }
	  | print_list ',' print_item { node_init ( $$ = malloc(sizeof(node_t)), print_list_n, NULL, 2, $1, $3); }
	  ;

expression_list: expression { node_init ( $$ = malloc(sizeof(node_t)), expression_list_n, NULL, 1, $1); }
	       | expression_list ',' expression { node_init ( $$ = malloc(sizeof(node_t)), expression_list_n, NULL, 2, $1, $3); }
	       ;

variable_list: variable { node_init ( $$ = malloc(sizeof(node_t)), variable_list_n, NULL, 1, $1); }
	     | indexed_variable { node_init ( $$ = malloc(sizeof(node_t)), variable_list_n, NULL, 1, $1); }
	     | variable_list ',' variable { node_init ( $$ = malloc(sizeof(node_t)), variable_list_n, NULL, 2, $1, $3); }
  	     | variable_list ',' indexed_variable { node_init ( $$ = malloc(sizeof(node_t)), variable_list_n, NULL, 2, $1, $3); }
	     ;

argument_list: expression_list { node_init ( $$ = malloc(sizeof(node_t)), argument_list_n, NULL, 1, $1); }
	     | { $$ = NULL; }
	     ;

parameter_list: variable_list { node_init ( $$ = malloc(sizeof(node_t)), parameter_list_n, NULL, 1, $1); }
	      | { $$ = NULL; }
	      ;

declaration_list: declaration_list declaration { node_init ( $$ = malloc(sizeof(node_t)), declaration_list_n, NULL, 2, $1, $2); }
		| { $$ = NULL; }
		;

function: FUNC variable '(' parameter_list ')' statement { node_init ( $$ = malloc(sizeof(node_t)), function_n, NULL, 3, $2, $4, $6); }
	;

statement: assignment_statement { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); } 
	 | return_statement  { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 | print_statement  { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 | null_statement  { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 | if_statement  { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 | while_statement  { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 | block { node_init ( $$ = malloc(sizeof(node_t)), statement_n, NULL, 1, $1); }
	 ;

block: '{' declaration_list statement_list '}' { node_init ( $$ = malloc(sizeof(node_t)), block_n, NULL, 2, $2, $3); }
     ;

assignment_statement: variable ASSIGNMENT expression { node_init ( $$ = malloc(sizeof(node_t)), assignment_statement_n, NULL, 2, $1, $3); }
		    | variable '[' expression ']' ASSIGNMENT expression { node_init ( $$ = malloc(sizeof(node_t)), assignment_statement_n, NULL, 3, $1, $3, $5); }
		    ;

return_statement: RETURN expression { node_init ( $$ = malloc(sizeof(node_t)), return_statement_n, NULL, 1, $2); }
		;

print_statement: PRINT print_list { node_init ( $$ = malloc(sizeof(node_t)), print_statement_n, NULL, 1, $2); }
	       ;

null_statement: CONTINUE { node_init ( $$ = malloc(sizeof(node_t)), null_statement_n, NULL, 0); }
	      ;

if_statement: IF expression THEN statement FI { node_init ( $$ = malloc(sizeof(node_t)), if_statement_n, NULL, 2, $2, $4); }
	    | IF expression THEN statement ELSE statement FI { node_init ( $$ = malloc(sizeof(node_t)), if_statement_n, NULL, 3, $2, $4, $6); }
	    ;

while_statement: WHILE expression DO statement DONE { node_init ( $$ = malloc(sizeof(node_t)), while_statement_n, NULL, 2, $2, $4); }
	       ;

expression: expression PLUS expression { node_init( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("+"), 2, $1, $3); }
	  | expression MINUS expression { node_init( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("-"), 2, $1, $3); }
    	  | expression MUL expression { node_init( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("*"), 2, $1, $3); }
    	  | expression DIV expression { node_init( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("/"), 2, $1, $3); }
    	  | MINUS expression { node_init ( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("-"), 1, $2); }
    	  | expression POWER expression { node_init ( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("**"), 2, $1, $3); }
    	  | '(' expression ')' { node_init ( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("PAREN"), 1, $2); }
    	  | integer { node_init ( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("NUMBER"), 1, $1); }
    	  | variable { node_init ( $$ = malloc(sizeof(node_t)), expression_n, STRDUP("VARIABLE"), 1, $1); }
    	  | variable '(' argument_list ')' { node_init ( $$ = malloc(sizeof(node_t)), expression_n, NULL, 2, $1, $3); } 
    	  | variable '[' expression ']' { node_init ( $$ = malloc(sizeof(node_t)), expression_n, NULL, 2, $1, $3); }
    	  ;

declaration: VAR variable_list { node_init ( $$ = malloc(sizeof(node_t)), declaration_n, STRDUP("VARIABLE"), 1, $1); }
	   ;

variable: IDENTIFIER { node_init( $$ = malloc(sizeof(node_t)), variable_n, STRDUP(yytext), 0) }
	;

indexed_variable: variable '[' integer ']' { node_init ( $$ = malloc(sizeof(node_t)), variable_n, NULL, 2, $1, $3); }
		;

integer: NUMBER { node_init( $$ = malloc(sizeof(node_t)), integer_n, STRDUP(yytext), 0); }
       ;

print_item: expression { node_init( $$ = malloc(sizeof(node_t)), print_item_n, NULL, 1, $1); }
	  | text { node_init( $$ = malloc(sizeof(node_t)), print_item_n, NULL, 1, $1); }
	  ;

text: STRING { node_init( $$ = malloc(sizeof(node_t)), text_n, STRDUP(yytext), 0); }
    ;

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


