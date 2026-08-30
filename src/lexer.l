/*le o codigo fonte e gera os token pra o bison(parser)*/

%{ 
#include<stdio.h>
#include<stdlib.h>
#include "parser.tab.h" 
%}


/*expressao regular*/
%%

[0-9]+          { 
    yylval.intValue = atoi(yytext);/*guarda o valor semantico em yytext*/
    return NUM; 
}
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
[ \t\n]+        { /* Ignorar espaços em branco */ }
.               { printf("Caractere inválido: %s\n", yytext); }

%%

int yywrap(void) {
    return 1;
}