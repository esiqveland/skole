%{
#include "parser.h" // defines the tokens, must include
#include "tree.h"

#ifdef DUMP_TOKENS
    #define RETURN(t) do {                                          \
        fprintf ( stderr, "TOKEN ( %d,\t'%s' )\n", t, yytext );     \
        return t;                                                   \
    } while ( 0 )
#else
    #define RETURN(t) return t
#endif
%}

%option array
%option noyywrap
%option yylineno

%%
[0-9]+	    { yylval=atoi(yytext); return NUMBER; }
"+"	    { return PLUS; }
"-"	    { return MINUS; }
"*"         { return MULT; }
.           { RETURN( yytext[0] ); }
%%
