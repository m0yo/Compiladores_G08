/*INTERFACE DA ARVORE*/

#ifndef AST_H
#define AST_H

/*tipos de nos da arvore*/
typedef enum{
    AST_INT,
    AST_FLOAT,
    AST_CHAR,

    AST_ID,

    AST_BINARYOP,
    AST_UNARYOP,

    AST_DECL, //declaracao variavel
    AST_ASSIGN , //atribuicao de valor a variavel
    AST_EXPRESS // ;


}