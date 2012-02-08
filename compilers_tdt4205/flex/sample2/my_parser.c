#include <stdio.h>
#include <stdlib.h>

int yyparse();

int main(int argc, char** argv)
{
    //call the parser
    yyparse();
    return 0;
}
