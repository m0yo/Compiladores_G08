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

/* Símbolo inicial da gramática */
%start expressao

/*Tipos*/
%token T_INT
%token T_FLOAT
%token T_CHAR
%token T_DOUBLE
%token T_LONG

/*Controle de fluxo e laços de repetição*/
%token F_IF
%token F_ELSE
%token F_WHILE
%token F_FOR
%token F_DO
%token F_SWITCH
%token F_CASE
%token F_BREAK
%token F_CONTINUE
%token F_RETURN

/*Literais (valores semânticos)*/
%token L_INT
%token L_FLOAT
%token L_CHAR
%token L_STRING

/*Identificador*/
%token IDENT

/*Operadores aritméticos e atribuição*/
%token O_PLUS
%token O_MINUS
%token O_MULTI
%token O_DIV
%token O_ASSIGN

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


/*Delimitadores*/
%token LBRACE
%token RBRACE
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token COLON
%token SEMICOLON
%token COMMA


/*Precedência*/

%left DM_OR
%left DM_AND
%left R_EQ R_NE
%left R_LT R_GT R_LE R_GE
%left O_PLUS O_MINUS
%left O_MULTI O_DIV
%precedence DM_NOT
%precedence UMINUS

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



void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}
