/******************************************************
 * FGA0003 - Compiladores 1 - Grupo 8 - T01
 * Curso de Engenharia de Software
 * Universidade de Brasília (UnB)
 *
 * Parser: declarações, expressões e comandos. Constrói a AST
 * e o main a executa com eval_ast.
 ******************************************************/

%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tabela_simbolos.h"
#include "semantica.h"

int yylex(void);
void yyerror(const char *s);

extern FILE *yyin;
extern int linha_erro;
extern int erros_lexicos;

/* raiz da AST: lista encadeada (via ->next) com os comandos do programa */
static ASTNode *raiz = NULL;

/* anexa a lista b ao final da lista a (qualquer uma pode ser NULL) */
static ASTNode *anexar(ASTNode *a, ASTNode *b) {
    if (!a) return b;
    ASTNode *fim = a;
    while (fim->next) fim = fim->next;
    fim->next = b;
    return a;
}
%}

/* Valores semânticos compartilhados com o lexer (yylval) */
%union {
    int ival;
    double dval;
    char cval;
    char *sval;
    ASTNode *node;
    SymType tipo;
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
%token F_DEFAULT
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
%token O_MOD
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
%left O_MULTI O_DIV O_MOD
%precedence DM_NOT
%precedence UMINUS

/* Não-terminais que carregam nós da AST */
%type <node> comando declaracao atribuicao bloco lista_comandos
%type <node> comando_if comando_while comando_do_while comando_for comando_switch
%type <node> for_init for_cond for_passo lista_casos caso expressao
%type <tipo> tipo
%type <ival> rotulo

%%

programa:
      %empty                    { raiz = NULL; }
    | programa comando          { raiz = anexar(raiz, $2); }
    ;

comando:
      declaracao                { $$ = $1; }
    | atribuicao SEMICOLON      { $$ = $1; }
    | expressao SEMICOLON       { $$ = new_expr_stmt_node($1); }
    | bloco                     { $$ = $1; }
    | comando_if                { $$ = $1; }
    | comando_while             { $$ = $1; }
    | comando_do_while          { $$ = $1; }
    | comando_for               { $$ = $1; }
    | comando_switch            { $$ = $1; }
    | F_BREAK SEMICOLON         { $$ = new_break_node(); }
    | F_CONTINUE SEMICOLON      { $$ = new_continue_node(); }
    | F_RETURN SEMICOLON        { $$ = new_return_node(NULL); }
    | F_RETURN expressao SEMICOLON { $$ = new_return_node($2); }
    ;

declaracao:
      tipo IDENT SEMICOLON                      { $$ = new_decl_node($1, $2, NULL); free($2); }
    | tipo IDENT O_ASSIGN expressao SEMICOLON   { $$ = new_decl_node($1, $2, $4);   free($2); }
    ;

atribuicao:
      IDENT O_ASSIGN expressao  { $$ = new_assign_node($1, $3); free($1); }
    ;

/* long e double ainda não têm tipo próprio na tabela de símbolos:
   são tratados como int e float */
tipo:
      T_INT     { $$ = TYPE_INT; }
    | T_FLOAT   { $$ = TYPE_FLOAT; }
    | T_CHAR    { $$ = TYPE_CHAR; }
    | T_DOUBLE  { $$ = TYPE_FLOAT; }
    | T_LONG    { $$ = TYPE_INT; }
    ;

bloco:
      LBRACE lista_comandos RBRACE  { $$ = new_block_node($2); }
    ;

lista_comandos:
      %empty                    { $$ = NULL; }
    | lista_comandos comando    { $$ = anexar($1, $2); }
    ;

comando_if:
      F_IF LPAREN expressao RPAREN comando %prec SEM_ELSE
                                { $$ = new_if_stmt_node($3, $5, NULL); }
    | F_IF LPAREN expressao RPAREN comando F_ELSE comando
                                { $$ = new_if_stmt_node($3, $5, $7); }
    ;

comando_while:
      F_WHILE LPAREN expressao RPAREN comando
                                { $$ = new_while_stmt_node($3, $5); }
    ;

comando_do_while:
      F_DO comando F_WHILE LPAREN expressao RPAREN SEMICOLON
                                { $$ = new_do_while_node($2, $5); }
    ;

comando_for:
      F_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_passo RPAREN comando
                                { $$ = new_for_stmt_node($3, $5, $7, $9); }
    ;

for_init:
      %empty                            { $$ = NULL; }
    | tipo IDENT O_ASSIGN expressao     { $$ = new_decl_node($1, $2, $4); free($2); }
    | atribuicao                        { $$ = $1; }
    ;

for_cond:
      %empty        { $$ = NULL; }
    | expressao     { $$ = $1; }
    ;

for_passo:
      %empty        { $$ = NULL; }
    | atribuicao    { $$ = $1; }
    ;

comando_switch:
      F_SWITCH LPAREN expressao RPAREN LBRACE lista_casos RBRACE
                                { $$ = new_switch_node($3, $6); }
    ;

lista_casos:
      %empty                    { $$ = NULL; }
    | lista_casos caso          { $$ = anexar($1, $2); }
    ;

caso:
      F_CASE rotulo COLON { $<ival>$ = linha_erro; } lista_comandos
                                { $$ = new_case_node($2, $5); $$->linha = $<ival>4; }
    | F_DEFAULT COLON { $<ival>$ = linha_erro; } lista_comandos
                                { $$ = new_default_node($4); $$->linha = $<ival>3; }
    ;

rotulo:
      L_INT     { $$ = $1; }
    | L_CHAR    { $$ = $1; }
    ;

expressao:
      expressao O_PLUS expressao    { $$ = new_binop_node('+', $1, $3); }
    | expressao O_MINUS expressao   { $$ = new_binop_node('-', $1, $3); }
    | expressao O_MULTI expressao   { $$ = new_binop_node('*', $1, $3); }
    | expressao O_DIV expressao     { $$ = new_binop_node('/', $1, $3); }
    | expressao O_MOD expressao     { $$ = new_binop_node('%', $1, $3); }

    | O_MINUS expressao %prec UMINUS { $$ = new_unaryop_node('-', $2); }

    | expressao DM_AND expressao    { $$ = new_binop_node('&', $1, $3); }
    | expressao DM_OR expressao     { $$ = new_binop_node('|', $1, $3); }
    | DM_NOT expressao              { $$ = new_unaryop_node('!', $2); }

    | expressao R_EQ expressao      { $$ = new_binop_node(OP_EQ,  $1, $3); }
    | expressao R_NE expressao      { $$ = new_binop_node(OP_NEQ, $1, $3); }
    | expressao R_LT expressao      { $$ = new_binop_node('<',    $1, $3); }
    | expressao R_GT expressao      { $$ = new_binop_node('>',    $1, $3); }
    | expressao R_LE expressao      { $$ = new_binop_node(OP_LEQ, $1, $3); }
    | expressao R_GE expressao      { $$ = new_binop_node(OP_GEQ, $1, $3); }

    | LPAREN expressao RPAREN       { $$ = $2; }

    | L_INT                         { $$ = new_num_node($1); }
    | L_FLOAT                       { $$ = new_float_node($1); }
    | L_CHAR                        { $$ = new_char_node($1); }
    | IDENT                         { $$ = new_id_node($1); free($1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático na linha %d: %s\n", linha_erro, s);
}

/*
 * Uso: ./parser_exe [-a] [arquivo.c]
 *   -a       imprime a AST antes de executar
 *   arquivo  código-fonte (sem arquivo, lê da entrada padrão)
 *
 * Código de saída: 1 em erro sintático ou de execução; se o programa
 * executar "return expr;", o valor de expr (como o main de um programa C);
 * senão 0.
 */
int main(int argc, char **argv) {
    int mostrar_ast = 0;
    const char *arquivo = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'a' && argv[i][2] == '\0')
            mostrar_ast = 1;
        else
            arquivo = argv[i];
    }

    if (arquivo) {
        yyin = fopen(arquivo, "r");
        if (!yyin) {
            perror(arquivo);
            return 1;
        }
    }

    if (yyparse() != 0)
        return 1;

    ASTNode *programa = new_block_node(raiz);

    /* erro léxico (ex.: comentário de bloco não fechado): não executa */
    if (erros_lexicos > 0) {
        free_ast(programa);
        if (arquivo) fclose(yyin);
        return 1;
    }

    /* análise semântica: se houver erro, o programa não é executado */
    if (analisar_semantica(programa) > 0) {
        free_ast(programa);
        limpar_tabela();
        if (arquivo) fclose(yyin);
        return 1;
    }

    if (mostrar_ast) {
        printf("=== AST ===\n");
        print_ast(programa, 0);
        printf("=== Execução ===\n");
    }

    eval_ast(programa);

    int codigo = 0;
    switch (controle_fluxo) {
        case CTRL_RETURN:
            codigo = (int)valor_retorno.val;
            printf("Programa retornou: %d\n", codigo);
            break;
        case CTRL_BREAK:
            fprintf(stderr, "Erro: 'break' fora de laço ou switch\n");
            codigo = 1;
            break;
        case CTRL_CONTINUE:
            fprintf(stderr, "Erro: 'continue' fora de laço\n");
            codigo = 1;
            break;
        default:
            break;
    }

    free_ast(programa);
    limpar_tabela();
    if (arquivo) fclose(yyin);
    return codigo;
}
