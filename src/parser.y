//exemplo de gramática para expressão aritmética

%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);//funcao que le o proximo token
void yyerror(const char *s);//funcao para tratar erros
%}

%union{//define valor semantico
    int intValue;
}

//token com valor semantico(NUM é do tipo intValue e possui valor semantico como 10 por ex)
%token <intValue> NUM;

%token PLUS MINUS TIMES DIVIDE LPAREN RPAREN
%left PLUS MINUS
%left TIMES DIVIDE
//define times e divide com maior precedencia que plus e minus

%type <intValue> expressao//expressao resulta em um tipo intValue
%%

inicio:
    expressao { printf("Resultado: %d\n", $1); }
  ;

expressao:
    expressao PLUS expressao    { $$ = $1 + $3; }
  | expressao MINUS expressao   { $$ = $1 - $3; }  
  | expressao TIMES expressao   { $$ = $1 * $3; }
  | expressao DIVIDE expressao  { $$ = $1 / $3; }
  | LPAREN expressao RPAREN     { $$ = $2; }
  | NUM                         { $$ = $1; } 
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}