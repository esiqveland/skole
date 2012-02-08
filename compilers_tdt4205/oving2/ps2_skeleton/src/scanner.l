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


digit	[0-9]
letter  [a-zA-Z]
underscore _
%%
{digit}+    	 { yylval=atoi(yytext); return NUMBER; }
"+"	    	 { return PLUS; }
"-"	    	 { return MINUS; }
"*"         	 { return MUL; }
"/"	    	 { return DIV; }
"**"	    	 { return POWER; }
":="		 { return ASSIGNMENT; }
"FUNC"		 { return FUNC; }
"PRINT"		 { return PRINT; }
"RETURN"	 { return RETURN; }
"CONTINUE"	 { return CONTINUE; }
"IF"		 { return IF; }
"THEN"		 { return THEN; }
"ELSE"		 { return ELSE; }
"FI"		 { return FI; }
"WHILE"		 { return WHILE; }
"DO"		 { return DO; }
"DONE"		 { return DONE; }
"VAR"		 { return VAR; }
{letter}({letter}|{underscore}|{digit})* { return IDENTIFIER; }
"(\")[^\^]*(\")"  { return STRING; }
\/\/+[^\n]*	 { return COMMENT; }
[ \t\n]		/* Whitespace catch */
.           	 { RETURN( yytext[0] ); } 
%%

// "\"[a-zA-z]*\""  { return STRING; }
// [^\^] denotes every char except newline and carot 

