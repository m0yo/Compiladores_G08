/*INTERFACE DA ARVORE*/

#ifndef AST_H
#define AST_H

/*tipos de nos da arvore*/
typedef enum{
    //literais
    AST_INT,
    AST_FLOAT,
    AST_CHAR,

    AST_ID,

    //operadores
    AST_BINARYOP, //(+, -, %)
    AST_UNARYOP, //(==. !)

    //comandos
    AST_DECL, //declaracao variavel
    AST_ASSIGN , //atribuicao de valor a variavel
    AST_EXPRESS_STMT// ;

    //controle de fluxo
    AST_IF,
    AST_WHILE,
    AST_FOR,

    AST_BLOCK //comandos entre {}
} TipoNo;


//resultado tipado da avaliacao de uma expressao
typedef struct{
    double val; //valor numerico

    SymType Type; // tipo_semantico
    //vai ser definido na tabela de simbolos
}resultado; 