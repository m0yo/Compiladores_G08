//implementacao da arvore sintatica abstrata

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tabela_simbolos.h"

//aloca um no com tipo definido
static ASTNode *alloc_node(TipoNo tipo) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Erro: Falha na alocação de memória para o nó da AST!\n");
        exit(EXIT_FAILURE);
    }
    node->tipo = tipo;
    node->next = NULL;
    return node;
}

//construtores de nos

ASTNode *new_num_node(int value) {
    ASTNode *node = alloc_node(AST_INT);
    node->data.num.value = value;
    return node;
}

ASTNode *new_float_node(double value) {
    ASTNode *node = alloc_node(AST_FLOAT);
    node->data.flt.value = value;
    return node;
}   

ASTNode *new_char_node(char value) {
    ASTNode *node = alloc_node(AST_CHAR);
    node->data.chr.value = value;
    return node;
}

ASTNode *new_id_node(const char *name) {
    ASTNode *node = alloc_node(AST_ID);
    node->data.id.name = strdup(name);
    return node;
}

ASTNode *new_binop_node(int op, ASTNode *left, ASTNode *right) {
    ASTNode *node = alloc_node(AST_BINARYOP);
    node->data.binaryop.op = op;
    node->data.binaryop.left = left;
    node->data.binaryop.right = right;
    return node;
}

ASTNode *new_unaryop_node(int op, ASTNode *operand) {
    ASTNode *node = alloc_node(AST_UNARYOP);
    node->data.unaryop.op = op;
    node->data.unaryop.operand = operand;
    return node;
}

ASTNode *new_assign_node(const char *name, ASTNode *expr) {
    ASTNode *node = alloc_node(AST_ASSIGN);
    node->data.assign.name = strdup(name);
    node->data.assign.expr = expr;
    return node;
}

ASTNode *new_decl_node(SymType type, const char *name, ASTNode *init) {
    ASTNode *node = alloc_node(AST_DECL);
    node->data.decl.type = type;
    node->data.decl.name = strdup(name);
    node->data.decl.init = init;
    return node;
}

ASTNode *new_expr_stmt_node(ASTNode *expr) {
    ASTNode *node = alloc_node(AST_EXPR_STMT);
    node->data.expr_stmt.expr = expr;
    return node;
}

ASTNode *new_if_stmt_node(ASTNode *cond, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = alloc_node(AST_IF);
    node->data.if_stmt.cond = cond;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode *new_while_stmt_node(ASTNode *cond, ASTNode *body) {
    ASTNode *node = alloc_node(AST_WHILE);
    node->data.while_stmt.cond = cond;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode *new_for_stmt_node(ASTNode *init, ASTNode *cond, ASTNode *step, ASTNode *body) {
    ASTNode *node = alloc_node(AST_FOR);
    node->data.for_stmt.init = init;
    node->data.for_stmt.cond = cond;
    node->data.for_stmt.step = step;
    node->data.for_stmt.body = body;
    return node;
}

ASTNode *new_block_node(ASTNode *statements) {
    ASTNode *node = alloc_node(AST_BLOCK);
    node->data.block.statements = statements;
    return node;
}

//funcoes auxiliares de execucao

//converte para symvalue 
static SymValue to_sym_value(SymType type, double value){
    Symvalue sym_value;
    switch(type){
        case TYPE_FLOAT: sym_value.floatValue = (float)value; break;
        case TYPE_INT: sym_value.intValue = (int)value; break;
        case TYPE_CHAR: sym_value.charValue = (char)value; break;
        default: sym_value.intValue = (int)value; break; //default para int
    }
    return sym_value;
}

//extrai o valor double de symentry
static double sym_val_as_double(const SymEntry *entry){
    switch(entry->type){
        case TYPE_FLOAT: return (double)entry->value.floatValue;
        case TYPE_CHAR: return (double)entry->value.charValue;
        default:        return (double)entry->value.intValue;
    }
}

//imprime um valor formtado de acordo com o tipo
static void print_sym_value(SymType type, double value){
    switch(type){
        case TYPE_FLOAT: printf("%f", value); break;
        case TYPE_CHAR: {
            char c = (char)(int)value;
            if(c >= 32 && c <= 126) //verifica se é um caractere imprimivel
                printf("'%c'", c);
            else
                printf("%d", (int)c); break; //imprime em hexadecimal
        }
        case TYPE_BOOL: printf("%s", ((int)value) ? "true" : "false"); break;
        default:        printf("%d", (int)value); break;
    }
}