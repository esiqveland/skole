#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    //call the lexer to scan stdin
    yylex();
    return 0;
}
