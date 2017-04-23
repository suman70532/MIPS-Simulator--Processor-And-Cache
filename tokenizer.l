/* recognize tokens for the calculator and print them out */
%{
    #include "processor.h"
    #include "parser.tab.h"
    char* temp;

%}
%%
[0-9a-zA-Z]+         	{ yylval.temp = yytext; return CODE;}
\n     					{ }
<<EOF>>					{ return End_of_file; }
[ \t]                   { }
%%
int yywrap()
{
    return 1;
}
