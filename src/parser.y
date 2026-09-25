/******************************************************
 * FGA0003 - Compiladores 1 - Grupo 8 - T01
 * Curso de Engenharia de Software
 * Universidade de Brasília (UnB)
 *
 * Parser inicial do projeto.
 ******************************************************/

%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);

void yyerror(const char *s);
%}

/*Tipos*/
%token T_INT
%token T_FLOAT
%token T_CHAR
%token T_DOUBLE
%token T_LONG

/*Literais (valores semânticos)*/
%token L_INT
%token L_FLOAT
%token L_CHAR
%token L_STRING

/*Identificador*/
%token IDENT

/*Operadores aritméticos*/
%token O_PLUS
%token O_MINUS
%token O_MULTI
%token O_DIV

/*Operadores relacionais*/
%token R_EQ
%token R_NE
%token R_LT
%token R_GT
%token R_LE
%token R_GE

/*Operadores lógicos*/
%token DM_AND
%token DM_OR
%token DM_NOT


/*Não coloquei todos os delimitadores que estão no lexer.l*/
/*Delimitadores*/
%token LPAREN
%token RPAREN
%token LBRACE
%token RBRACE
%token SEMICOLON


/*Precedência*/

%left DM_OR
%left DM_AND
%left R_EQ R_NE
%left R_LT R_GT R_LE R_GE
%left O_PLUS O_MINUS
%left O_MULTI O_DIV
%right DM_NOT
%right UMINUS

%%

expressao:
      expressao O_PLUS expressao
    | expressao O_MINUS expressao
    | expressao O_MULTI expressao
    | expressao O_DIV expressao

    | O_MINUS expressao %prec UMINUS

    | expressao DM_AND expressao
    | expressao DM_OR expressao
    | DM_NOT expressao

    | expressao R_EQ expressao
    | expressao R_NE expressao
    | expressao R_LT expressao
    | expressao R_GT expressao
    | expressao R_LE expressao
    | expressao R_GE expressao

    | LPAREN expressao RPAREN

    | L_INT
    | L_FLOAT
    | L_CHAR
    | IDENT
    ;


%%

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}