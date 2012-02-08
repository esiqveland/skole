%{
#include <stdlib.h>
#include <stdio.h>
    int yylex();
    int yyerror(char*);
%}
%token NUMBER PLUS MINUS MULT

%left PLUS MINUS
%left MULT

%%
program: expr { printf("Answer is %d\n", $1); };
expr: expr PLUS expr { $$ = $1 + $3; }
    | expr MINUS expr { $$ = $1 - $3; }
    | expr MULT expr { $$ = $1 * $3; }
    | NUMBER { $$ = $1; };
%%

int yyerror(char* s)
{
    //these symbols are defined in the lexer but we want to use them here for error messages
    extern int yylineno;
    extern char* yytext;

    printf("Error: %s, line %d\n", yytext, yylineno);
    exit(1);
}

