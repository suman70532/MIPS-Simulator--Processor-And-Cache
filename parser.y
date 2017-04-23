/* simplest version of calculator */
%{
#include <stdio.h>
#include "processor.h"
# include <stdlib.h>
#include <string.h>
#include<pthread.h>

int yyerror(char *);
int yylex();
%}
/* declare tokens */
%token End_of_file
%token EOL
%token CODE
%union{
    char* temp;
}
%type <temp> exprn CODE

%%
exprn : CODE                      {add_to_array($1); }
| End_of_file						{create_IM(); execute_program();}
| exprn CODE                      {add_to_array($2);}
| exprn End_of_file                           {create_IM(); execute_program();}    	

%%
int main(int argc, char **argv)
{
    char line[30];
    if(argc != 4)
    	exit(0);
    out = fopen(argv[2], "w");
    yyin = fopen(argv[1], "r");
    yyparse();
    fclose(yyin);
    fclose(out);
    return 0;
}
int yyerror(char *s)
{
    fprintf(out, "%s\n", "SynErr");
}
