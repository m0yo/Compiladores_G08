/******************************************************
 * FGA0003 - Compiladores 1 - Grupo 8 - T01
 * Curso de Engenharia de Software
 * Universidade de Brasília (UnB)
 *
 * Parser inicial do projeto: declarações, expressões e comandos.
 ******************************************************/

%{
#include <stdio.h>
#include <stdlib.h>
#include "tabela_simbolos.h"

int yylex(void);
void yyerror(const char *s);
%}

/* Valores semânticos compartilhados com o lexer (yylval) */
%union {
    int ival;
    double dval;
    char cval;
    char *sval;
}

/* Símbolo inicial da gramática */
%start programa

/* Tipos */
%token T_INT
%token T_FLOAT
%token T_CHAR
%token T_DOUBLE
%token T_LONG

/* Controle de fluxo e laços de repetição */
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

/* Literais (valores semânticos) */
%token <ival> L_INT
%token <dval> L_FLOAT
%token <cval> L_CHAR

/* Identificador */
%token <sval> IDENT

/* Operadores aritméticos e atribuição */
%token O_PLUS
%token O_MINUS
%token O_MULTI
%token O_DIV
%token O_ASSIGN

/* Operadores relacionais */
%token R_EQ
%token R_NE
%token R_LT
%token R_GT
%token R_LE
%token R_GE

/* Operadores lógicos */
%token DM_AND
%token DM_OR
%token DM_NOT

/* Delimitadores */
%token LBRACE
%token RBRACE
%token LBRACKET
%token RBRACKET
%token LPAREN
%token RPAREN
%token COLON
%token SEMICOLON

/* Precedência (da menor para a maior) */

/* Resolve o "else pendente": o else casa com o if mais próximo */
%precedence SEM_ELSE
%precedence F_ELSE

%left DM_OR
%left DM_AND
%left R_EQ R_NE
%left R_LT R_GT R_LE R_GE
%left O_PLUS O_MINUS
%left O_MULTI O_DIV
%precedence DM_NOT
%precedence UMINUS

%%

programa:
      /* vazio */
    | programa comando
    ;

comando:
      declaracao
    | atribuicao SEMICOLON
    | expressao SEMICOLON
    | bloco
    | comando_if
    | comando_while
    | comando_do_while
    | comando_for
    | comando_switch
    | F_BREAK SEMICOLON
    | F_CONTINUE SEMICOLON
    | F_RETURN SEMICOLON
    | F_RETURN expressao SEMICOLON
    ;

declaracao:
      tipo IDENT SEMICOLON
    | tipo IDENT O_ASSIGN expressao SEMICOLON
    ;

atribuicao:
      IDENT O_ASSIGN expressao
    ;

tipo:
      T_INT
    | T_FLOAT
    | T_CHAR
    | T_DOUBLE
    | T_LONG
    ;

bloco:
      LBRACE lista_comandos RBRACE
    ;

lista_comandos:
      /* vazio */
    | lista_comandos comando
    ;

comando_if:
      F_IF LPAREN expressao RPAREN comando %prec SEM_ELSE
    | F_IF LPAREN expressao RPAREN comando F_ELSE comando
    ;

comando_while:
      F_WHILE LPAREN expressao RPAREN comando
    ;

comando_do_while:
      F_DO comando F_WHILE LPAREN expressao RPAREN SEMICOLON
    ;

comando_for:
      F_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_passo RPAREN comando
    ;

for_init:
      /* vazio */
    | tipo IDENT O_ASSIGN expressao
    | atribuicao
    ;

for_cond:
      /* vazio */
    | expressao
    ;

for_passo:
      /* vazio */
    | atribuicao
    ;

comando_switch:
      F_SWITCH LPAREN expressao RPAREN LBRACE lista_casos RBRACE
    ;

lista_casos:
      /* vazio */
    | lista_casos caso
    ;

caso:
      F_CASE rotulo COLON lista_comandos
    ;

rotulo:
      L_INT
    | L_CHAR
    ;

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

int main(void) {
    return yyparse();
}
