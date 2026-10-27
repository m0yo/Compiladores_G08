/*INTERFACE DA ARVORE*/

#ifndef AST_H
#define AST_H

//#include "tabela_simbolos.h"

/*tipos de nos da arvore*/
typedef enum{
    //literais
    AST_INT,
    AST_FLOAT,
    AST_CHAR,

    AST_ID,

    //operadores
    AST_BINARYOP, //(+, -, %)
    AST_UNARYOP, //(=, !)

    //comandos
    AST_DECL, //declaracao variavel
    AST_ASSIGN , //atribuicao de valor a variavel
    AST_EXPRESS_STMT,// ;

    //controle de fluxo
    AST_IF,
    AST_WHILE,
    AST_FOR,

    AST_BLOCK //lista de comandos
} TipoNo;


//resultado tipado da avaliacao de uma expressao
typedef struct{
    double val; //valor numerico

    SymType Type; // tipo_semantico
    //vai ser definido na tabela de simbolos
}resultado; 

//!!!!NO DA AST

typedef struct ASTNode{
    TipoNo tipo;
    struct ASTNode *next; //proximo comando da lista

    union{
    struct { int value; } num;

    struct { double value; } flt;

    struct { char value; } chr;

    struct { char *name; } id;

    //AST_BINARYOP: left OP right
    struct{
        int op;

        struct ASTNode *left;
        struct ASTNode *right;
    } binaryop;

    //AST_UNARYOP: OP operand;
    struct{
        int op;

        struct ASTNode *operand;
    }unaryop;

    //AST_ASSIGN: id = expr
    struct {
        char *name;
        struct ASTNode *expr;
    }assign;

     //AST_DECL: type_spec id [= expr]
        struct {
            SymType type;
            char *name;
            struct ASTNode *init;       /* NULL se sem inicialização */
        } decl;

        //AST_EXPRESS_STMT: expr; 
        struct {
            struct ASTNode *expr;
        } expr_stmt;

        //AST_IF: if (cond) then_branch [else else_branch] 
        struct {
            struct ASTNode *cond;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch; //NULL se sem else 
        } if_stmt;

        //AST_WHILE: while (cond) body 
        struct {
            struct ASTNode *cond;
            struct ASTNode *body;
        } while_stmt;

        //AST_FOR: for (init; cond; step) body 
        struct {
            struct ASTNode *init;
            struct ASTNode *cond;
            struct ASTNode *step;
            struct ASTNode *body;
        } for_stmt;

        //AST_BLOCK: { stmt_list } 
        struct {
            struct ASTNode *stmts;      //cabeça da lista encadeada 
        } block;
}data;
}ASTNode;

#endif 