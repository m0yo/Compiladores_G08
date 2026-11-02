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

//promocao de tipo C-standard, float domina, char promove para int
static SymType promote_type(SymType left, SymType right){
    if(left == TYPE_FLOAT || right == TYPE_FLOAT)
        return TYPE_FLOAT;
    return TYPE_INT; //default para int
}

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

/*
execucao de uma lista encadeada de comandos
percorre todos os nos via next
*/

static resultado exec_list(ASTNode *node){
    resultado last = {0,0, TYPE_INT}; //valor default
    while(node){
        last = eval_ast(node);
        node = node->next;
    }
    return last;
}    

/*
avalia recursivamente a arvore
caso == expressao, retorna `resultado` com o valor e tipo da expressao
*/

resultado eval_ast(ASTNode *node){
    resultado res = {0,0, TYPE_INT}; 

    if(!node) return res; 

    switch(node->tipo){

        //literais
        case AST_INT:
            res.val = (double)node->data.num.value;
            res.Type = TYPE_INT;
            break;
        
        case AST_FLOAT:
            res.val = node->data.flt.value;
            res.Type = TYPE_FLOAT;
            break;
            
        case AST_CHAR:
            res.val = (double)node->data.chr.value;
            res.Type = TYPE_CHAR;
            break;  
        
        //identificador
        case AST_ID: {
            SymEntry *entry = buscar_simbolo(node->data.id.name);
            res.val = sym_val_as_double(entry);
            res.Type = entry->type;
            break;
        }

        //operacao binaria
        case AST_BINARYOP: {
            resultado left = eval_ast(node->data.binaryop.left);
            resultado right = eval_ast(node->data.binaryop.right);
            SymType rtype = promote_type(left.Type, right.Type);   
            
            
            switch(node->data.binaryop.op){
                case '+': 
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val + right.val;
                    else
                        res.val = (double)((int)left.val + (int)right.val);
                    res.Type = left.Type; //assume o tipo do lado esquerdo
                    break;
                case '-':
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val - right.val;
                    else
                        res.val = (double)((int)left.val - (int)right.val);
                    res.Type = left.Type; //assume o tipo do lado esquerdo
                    break;
                case '*':
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val * right.val;
                    else
                        res.val = (double)((int)left.val * (int)right.val);
                    res.Type = left.Type; //assume o tipo do lado esquerdo
                    break;

                case '/':
                    if(right.val == 0.0){
                        fprintf(stderr, "Erro: Divisão por zero!\n");
                        exit(EXIT_FAILURE);
                    }
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val / right.val;
                    else
                        res.val = (double)((int)left.val / (int)right.val);
                    res.Type = rtype;
                    break;

                //operadores logicos
                case '&':
                    res.val = (double)(left.val && right.val);
                    res.Type = TYPE_INT;
                    break;
                case '|':
                    res.val = (double)(left.val || right.val);
                    res.Type = TYPE_INT;
                    break;
                
                //operadores de igualdade
                case EQ:
                    res.val = (double)(left.val == right.val);
                    res.Type = TYPE_INT;
                    break;
                case NEQ:
                    res.val = (double)(left.val != right.val);
                    res.Type = TYPE_INT;
                    break;

                //operadores relacionais
                case '<':
                    res.val = (double)(left.val < right.val);
                    res.Type = TYPE_INT;
                    break;
                case '>':
                    res.val = (double)(left.val > right.val);
                    res.Type = TYPE_INT;
                    break;
                case LEQ:
                    res.val = (double)(left.val <= right.val);
                    res.Type = TYPE_INT;
                    break;
                case GEQ:
                    res.val = (double)(left.val >= right.val);
                    res.Type = TYPE_INT;
                    break;
                default:
                    fprintf(stderr, "Erro: Operador binário desconhecido '%d'!\n", node->data.binaryop.op);
                    exit(EXIT_FAILURE);
        }
        break;       

    }
    //operacao unaria
    case AST_UNARYOP: {
        resultado operand = eval_ast(node->data.unaryop.operand);
        switch(node->data.unaryop.op){
            case '-':
                res.val = -operand.val;
                res.Type = operand.Type;
                break;
            case '!':
                res.val = (double)(!operand.val);
                res.Type = TYPE_INT;
                break;
            default:
                fprintf(stderr, "Erro: Operador unário desconhecido '%c'!\n", node->data.unaryop.op);
                exit(EXIT_FAILURE);
        }
        break;
    }

    //atribuicao
    case AST_ASSIGN: {
        resultado expr = eval_ast(node->data.assign.expr);
        SymEntry *entry = buscar_simbolo(node->data.assign.name);
        entry->value = to_sym_value(entry->type, expr.val); //converte para o tipo da variavel
        printf("%s %s = ", sym_type_name(entry->type), node->data.assign.name);
        print_value(entry->type, sym_val_as_double(entry));
        printf("\n");
        break;
    }  
    
    //declaracao
    case AST_DECL: {
        double init_val = 0.0;
        if (node->data.decl.init) {
            EvalResult r = eval_ast(node->data.decl.init);
            init_val = r.val;
        }
        SymValue v = to_sym_value(node->data.decl.type, init_val);
        sym_set(node->data.decl.name, node->data.decl.type, v);
        if (node->data.decl.init) {
            printf("Declarado: %s : %s = ", node->data.decl.name,
                   sym_type_name(node->data.decl.type));
            print_value(node->data.decl.type,
                        sym_val_as_double(sym_lookup(node->data.decl.name)));
            printf("\n");
        } else {
            printf("Declarado: %s : %s\n", node->data.decl.name,
                   sym_type_name(node->data.decl.type));
        }
        break;
    }

    //expressoes como comando (expr;)
    case AST_EXPR_STMT: {
        resultado expr = eval_ast(node->data.expr_stmt.expr);
        printf("Resultado da expressão: ");
        print_sym_value(expr.Type, expr.val);
        printf("\n");
        break;
    }

    //controle de fluxo
    //if/ if-else
    case AST_IF: {
        resultado cond = eval_ast(node->data.if_stmt.cond);
        if (cond.val) {
            exec_list(node->data.if_stmt.then_branch);
        } else if (node->data.if_stmt.else_branch) {
            exec_list(node->data.if_stmt.else_branch);
        }
        break;
    }   

    //while
    case AST_WHILE: {
        while(1){
            resultado cond = eval_ast(node->data.while_stmt.cond);
            if(!cond.val) break;
            exec_list(node->data.while_stmt.body);
        }
        break;
    }

    //for
    case AST_FOR: {
        if(node->data.for_stmt.init)
            eval_ast(node->data.for_stmt.init);
        }
        //loop que testa condicao, executa corpo e passo
        while(1){
            if(node->data.for_stmt.cond){
                resultado cond = eval_ast(node->data.for_stmt.cond);
                if(!cond.val) break;
            }
            exec_list(node->data.for_stmt.body);
            if(node->data.for_stmt.step)
                eval_ast(node->data.for_stmt.step);
        }
        break;
    }

    //bloco de comandos
    case AST_BLOCK: {
        exec_list(node->data.block.statements);
        break;
    }

    return res;
}
    