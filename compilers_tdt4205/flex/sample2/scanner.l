%{
    #include "parser.h"
%}

%%
[0-9]+  { yylval=atoi(yytext); return NUMBER; }
"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MULT; }
\s+ {}
%%
