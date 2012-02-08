%%
[0-9]+  { printf("NUMBER(%s)", yytext); }
"+"     { printf("PLUS"); }
"-"     { printf("MINUS"); }
"*"     { printf("MULT"); }
\s+ {}
%%
