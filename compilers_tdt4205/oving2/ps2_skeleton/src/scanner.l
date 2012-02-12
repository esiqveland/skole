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
\/\/+[^\n]*	 ;/*{ RETURN(COMMENT); }*/
[ \t\n]		/* Whitespace catch */ ;
{digit}+    	 { yylval=atoi(yytext);  RETURN(NUMBER); }
"+"	    	 { RETURN(PLUS); }
"-"	    	 { RETURN(MINUS); }
"/"	    	 { RETURN(DIV); }
\*\*	    	 { RETURN(POWER); }
"*"         	 { RETURN(MUL); }
":="		 { RETURN(ASSIGNMENT); }
"FUNC"		 { RETURN(FUNC); }
"PRINT"		 { RETURN(PRINT); }
"RETURN"	 { RETURN(RETURN); }
"CONTINUE"	 { RETURN(CONTINUE); }
"IF"		 { RETURN(IF); }
"THEN"		 { RETURN(THEN); }
"ELSE"		 { RETURN(ELSE); }
"FI"		 { RETURN(FI); }
"WHILE"		 { RETURN(WHILE); }
"DO"		 { RETURN(DO); }
"DONE"		 { RETURN(DONE); }
"VAR"		 { RETURN(VAR); }
({letter}|{underscore})({letter}|{underscore}|{digit})* {  RETURN(IDENTIFIER); }
\"(\\\"|[^\"\n])*\" { RETURN(STRING); }
.           	 { RETURN( yytext[0] ); } 
%%

//(\")[^\^]*(\") 	 { RETURN(STRING); }
//\".*\"		 { RETURN(STRING); }
//\"(\\\"|[^\"\n])*\" { RETURN(STRING); }
// "\"[a-zA-z]*\""  { return STRING; }
// [^\^] denotes every char except newline and carot 
// \\ --> \  \" --> " \\\" --> \" | 
