//implementacao da arvore sintatica abstrata

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela_simbolos.h"

//linha atual do lexer (definida em lexer.l; test_main.c define a sua)
extern int linha_erro;

//aloca um no com tipo definido
static ASTNode *alloc_node(TipoNo tipo) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Erro: Falha na alocação de memória para o nó da AST!\n");
        exit(EXIT_FAILURE);
    }
    node->tipo = tipo;
    node->next = NULL;
    node->linha = linha_erro;
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

ASTNode *new_do_while_node(ASTNode *body, ASTNode *cond) {
    ASTNode *node = alloc_node(AST_DO_WHILE);
    node->data.while_stmt.cond = cond;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode *new_switch_node(ASTNode *expr, ASTNode *cases) {
    ASTNode *node = alloc_node(AST_SWITCH);
    node->data.switch_stmt.expr = expr;
    node->data.switch_stmt.cases = cases;
    return node;
}

ASTNode *new_case_node(int value, ASTNode *stmts) {
    ASTNode *node = alloc_node(AST_CASE);
    node->data.case_stmt.value = value;
    node->data.case_stmt.stmts = stmts;
    return node;
}

ASTNode *new_break_node(void) {
    return alloc_node(AST_BREAK);
}

ASTNode *new_continue_node(void) {
    return alloc_node(AST_CONTINUE);
}

ASTNode *new_return_node(ASTNode *expr) {
    ASTNode *node = alloc_node(AST_RETURN);
    node->data.return_stmt.expr = expr;
    return node;
}

ASTNode *new_block_node(ASTNode *statements) {
    ASTNode *node = alloc_node(AST_BLOCK);
    node->data.block.stmts = statements;
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
    SymValue sym_value;
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
                printf("%d", (int)c); //nao imprimivel: mostra o codigo
            break;
        }
        default:        printf("%d", (int)value); break;
    }
}

/*
execucao de uma lista encadeada de comandos
percorre todos os nos via next
*/

ControleFluxo controle_fluxo = CTRL_NORMAL;
resultado valor_retorno = {0, TYPE_INT};

static resultado exec_list(ASTNode *node){
    resultado last = {0, TYPE_INT}; //valor default
    //para no primeiro break/continue/return
    while(node && controle_fluxo == CTRL_NORMAL){
        last = eval_ast(node);
        node = node->next;
    }
    return last;
}    

/*
chamada pelos lacos depois de executar o corpo:
retorna 1 se o laco deve terminar (break ou return)
break e continue sao consumidos aqui; return continua propagando
*/
static int laco_deve_parar(void){
    switch(controle_fluxo){
        case CTRL_BREAK:
            controle_fluxo = CTRL_NORMAL;
            return 1;
        case CTRL_CONTINUE:
            controle_fluxo = CTRL_NORMAL;
            return 0;
        case CTRL_RETURN:
            return 1;
        default:
            return 0;
    }
}

/*
avalia recursivamente a arvore
caso == expressao, retorna `resultado` com o valor e tipo da expressao
*/

resultado eval_ast(ASTNode *node){
    resultado res = {0, TYPE_INT}; 

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
            if(!entry){
                fprintf(stderr, "Erro: variável '%s' não declarada!\n", node->data.id.name);
                exit(EXIT_FAILURE);
            }
            res.val = sym_val_as_double(entry);
            res.Type = entry->type;
            break;
        }

        //operacao binaria
        case AST_BINARYOP: {
            resultado left = eval_ast(node->data.binaryop.left);

            //curto-circuito (como em C): o lado direito de && so e avaliado
            //se o esquerdo e verdadeiro, e o de || so se o esquerdo e falso
            if(node->data.binaryop.op == '&' || node->data.binaryop.op == '|'){
                int l = left.val != 0;
                if(node->data.binaryop.op == '&' ? !l : l){
                    res.val = (double)l;
                }else{
                    resultado right = eval_ast(node->data.binaryop.right);
                    res.val = (double)(right.val != 0);
                }
                res.Type = TYPE_INT;
                break;
            }

            resultado right = eval_ast(node->data.binaryop.right);
            SymType rtype = promote_type(left.Type, right.Type);   
            
            
            switch(node->data.binaryop.op){
                case '+': 
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val + right.val;
                    else
                        res.val = (double)((int)left.val + (int)right.val);
                    res.Type = rtype;
                    break;
                case '-':
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val - right.val;
                    else
                        res.val = (double)((int)left.val - (int)right.val);
                    res.Type = rtype;
                    break;
                case '*':
                    if(rtype == TYPE_FLOAT)
                        res.val = left.val * right.val;
                    else
                        res.val = (double)((int)left.val * (int)right.val);
                    res.Type = rtype;
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
                case '%':
                    //resto da divisao inteira: so para int/char, como em C
                    if(rtype == TYPE_FLOAT){
                        fprintf(stderr, "Erro: operador %% exige operandos inteiros!\n");
                        exit(EXIT_FAILURE);
                    }
                    if((int)right.val == 0){
                        fprintf(stderr, "Erro: Divisão por zero!\n");
                        exit(EXIT_FAILURE);
                    }
                    res.val = (double)((int)left.val % (int)right.val);
                    res.Type = TYPE_INT;
                    break;

                //operadores logicos
                
                //operadores de igualdade
                case OP_EQ:
                    res.val = (double)(left.val == right.val);
                    res.Type = TYPE_INT;
                    break;
                case OP_NEQ:
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
                case OP_LEQ:
                    res.val = (double)(left.val <= right.val);
                    res.Type = TYPE_INT;
                    break;
                case OP_GEQ:
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
        if(!entry){
            fprintf(stderr, "Erro: variável '%s' não declarada!\n", node->data.assign.name);
            exit(EXIT_FAILURE);
        }
        entry->value = to_sym_value(entry->type, expr.val); //converte para o tipo da variavel
        printf("%s %s = ", tipo_para_string(entry->type), node->data.assign.name);
        print_sym_value(entry->type, sym_val_as_double(entry));
        printf("\n");
        break;
    }  
    
    //declaracao
    case AST_DECL: {
        double init_val = 0.0;
        if (node->data.decl.init) {
            resultado r = eval_ast(node->data.decl.init);
            init_val = r.val;
        }
        SymValue v = to_sym_value(node->data.decl.type, init_val);
        inserir_simbolo(node->data.decl.name, node->data.decl.type, v);
        if (node->data.decl.init) {
            printf("Declarado: %s : %s = ", node->data.decl.name,
                   tipo_para_string(node->data.decl.type));
            print_sym_value(node->data.decl.type,
                        sym_val_as_double(buscar_simbolo(node->data.decl.name)));
            printf("\n");
        } else {
            printf("Declarado: %s : %s\n", node->data.decl.name,
                   tipo_para_string(node->data.decl.type));
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
            if(laco_deve_parar()) break;
        }
        break;
    }

    //do-while: o corpo executa pelo menos uma vez
    case AST_DO_WHILE: {
        while(1){
            exec_list(node->data.while_stmt.body);
            if(laco_deve_parar()) break;
            resultado cond = eval_ast(node->data.while_stmt.cond);
            if(!cond.val) break;
        }
        break;
    }

    //for
    case AST_FOR: {
        if(node->data.for_stmt.init)
            eval_ast(node->data.for_stmt.init);
        //loop que testa condicao, executa corpo e passo
        while(1){
            if(node->data.for_stmt.cond){
                resultado cond = eval_ast(node->data.for_stmt.cond);
                if(!cond.val) break;
            }
            exec_list(node->data.for_stmt.body);
            if(laco_deve_parar()) break;
            if(node->data.for_stmt.step)
                eval_ast(node->data.for_stmt.step);
        }
        break;
    }

    //switch: executa a partir do case que casa ate um break (fall-through, como em C)
    case AST_SWITCH: {
        resultado v = eval_ast(node->data.switch_stmt.expr);
        ASTNode *c = node->data.switch_stmt.cases;
        while(c && c->data.case_stmt.value != (int)v.val)
            c = c->next;
        for(; c && controle_fluxo == CTRL_NORMAL; c = c->next)
            exec_list(c->data.case_stmt.stmts);
        if(controle_fluxo == CTRL_BREAK) //break sai do switch; continue/return seguem
            controle_fluxo = CTRL_NORMAL;
        break;
    }

    //AST_CASE so e executado pelo AST_SWITCH
    case AST_CASE:
        break;

    case AST_BREAK:
        controle_fluxo = CTRL_BREAK;
        break;

    case AST_CONTINUE:
        controle_fluxo = CTRL_CONTINUE;
        break;

    case AST_RETURN: {
        resultado r = {0, TYPE_INT};
        if(node->data.return_stmt.expr)
            r = eval_ast(node->data.return_stmt.expr);
        valor_retorno = r;
        controle_fluxo = CTRL_RETURN;
        break;
    }

    //bloco de comandos
    case AST_BLOCK: {
        exec_list(node->data.block.stmts);
        break;
    }

    }
    return res;
}

//ferramentas de depuracao criadas pela ia
/* ====================================================
 * Impressão visual da AST (para demonstração)
 *
 * Percorre a árvore recursivamente, imprimindo cada nó
 * com indentação proporcional ao nível de profundidade.
 * Ao final de cada nó, percorre a lista encadeada (->next).
 * ==================================================== */

/* Converte o código interno do operador para string legível */
static const char *op_to_str(int op) {
    switch (op) {
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case '/': return "/";
        case '%': return "%";
        case '&': return "&&";
        case '|': return "||";
        case '!': return "!";
        case OP_EQ:  return "==";
        case OP_NEQ: return "!=";
        case '<': return "<";
        case '>': return ">";
        case OP_LEQ: return "<=";
        case OP_GEQ: return ">=";
        default:  return "?";
    }
}

void print_ast(ASTNode *node, int level) {
    if (!node) return;

    /* Indentação: 2 espaços por nível */
    for (int i = 0; i < level; i++) printf("  ");

    switch (node->tipo) {

    case AST_INT:
        printf("NUM (%d)\n", node->data.num.value);
        break;

    case AST_FLOAT:
        printf("FLOAT (%g)\n", node->data.flt.value);
        break;

    case AST_CHAR:
        printf("CHAR ('%c')\n", node->data.chr.value);
        break;

    case AST_ID:
        printf("ID (%s)\n", node->data.id.name);
        break;

    case AST_BINARYOP:
        printf("BINOP (%s)\n", op_to_str(node->data.binaryop.op));
        print_ast(node->data.binaryop.left,  level + 1);
        print_ast(node->data.binaryop.right, level + 1);
        break;

    case AST_UNARYOP:
        printf("UNARYOP (%s)\n", op_to_str(node->data.unaryop.op));
        print_ast(node->data.unaryop.operand, level + 1);
        break;

    case AST_ASSIGN:
        printf("ASSIGN (%s)\n", node->data.assign.name);
        print_ast(node->data.assign.expr, level + 1);
        break;

    case AST_DECL:
        printf("DECL (%s : %s)\n",
               node->data.decl.name,
               tipo_para_string(node->data.decl.type));
        if (node->data.decl.init) {
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("init:\n");
            print_ast(node->data.decl.init, level + 2);
        }
        break;

    case AST_EXPR_STMT:
        printf("EXPR_STMT\n");
        print_ast(node->data.expr_stmt.expr, level + 1);
        break;

    case AST_IF:
        printf("IF\n");
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("cond:\n");
        print_ast(node->data.if_stmt.cond, level + 2);
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("then:\n");
        print_ast(node->data.if_stmt.then_branch, level + 2);
        if (node->data.if_stmt.else_branch) {
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("else:\n");
            print_ast(node->data.if_stmt.else_branch, level + 2);
        }
        break;

    case AST_WHILE:
        printf("WHILE\n");
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("cond:\n");
        print_ast(node->data.while_stmt.cond, level + 2);
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("body:\n");
        print_ast(node->data.while_stmt.body, level + 2);
        break;

    case AST_FOR:
        printf("FOR\n");
        if (node->data.for_stmt.init) {
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("init:\n");
            print_ast(node->data.for_stmt.init, level + 2);
        }
        if (node->data.for_stmt.cond) {
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("cond:\n");
            print_ast(node->data.for_stmt.cond, level + 2);
        }
        if (node->data.for_stmt.step) {
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("step:\n");
            print_ast(node->data.for_stmt.step, level + 2);
        }
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("body:\n");
        print_ast(node->data.for_stmt.body, level + 2);
        break;

    case AST_DO_WHILE:
        printf("DO_WHILE\n");
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("body:\n");
        print_ast(node->data.while_stmt.body, level + 2);
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("cond:\n");
        print_ast(node->data.while_stmt.cond, level + 2);
        break;

    case AST_SWITCH:
        printf("SWITCH\n");
        for (int i = 0; i < level + 1; i++) printf("  ");
        printf("expr:\n");
        print_ast(node->data.switch_stmt.expr, level + 2);
        print_ast(node->data.switch_stmt.cases, level + 1);
        break;

    case AST_CASE:
        printf("CASE (%d)\n", node->data.case_stmt.value);
        print_ast(node->data.case_stmt.stmts, level + 1);
        break;

    case AST_BREAK:
        printf("BREAK\n");
        break;

    case AST_CONTINUE:
        printf("CONTINUE\n");
        break;

    case AST_RETURN:
        printf("RETURN\n");
        print_ast(node->data.return_stmt.expr, level + 1);
        break;

    case AST_BLOCK:
        printf("BLOCK\n");
        print_ast(node->data.block.stmts, level + 1);
        break;
    }

    /* Percorre a lista encadeada (próximo comando no mesmo nível) */
    print_ast(node->next, level);
}

/* ====================================================
 * free_ast: libera recursivamente toda a memória da AST
 * ==================================================== */
void free_ast(ASTNode *node) {
    if (!node) return;

    /* Libera o próximo na lista encadeada */
    free_ast(node->next);

    switch (node->tipo) {
    case AST_INT:
    case AST_FLOAT:
    case AST_CHAR:
        /* Nenhuma alocação interna */
        break;

    case AST_ID:
        free(node->data.id.name);
        break;

    case AST_BINARYOP:
        free_ast(node->data.binaryop.left);
        free_ast(node->data.binaryop.right);
        break;

    case AST_UNARYOP:
        free_ast(node->data.unaryop.operand);
        break;

    case AST_ASSIGN:
        free(node->data.assign.name);
        free_ast(node->data.assign.expr);
        break;

    case AST_DECL:
        free(node->data.decl.name);
        free_ast(node->data.decl.init);
        break;

    case AST_EXPR_STMT:
        free_ast(node->data.expr_stmt.expr);
        break;

    case AST_IF:
        free_ast(node->data.if_stmt.cond);
        free_ast(node->data.if_stmt.then_branch);
        free_ast(node->data.if_stmt.else_branch);
        break;

    case AST_WHILE:
        free_ast(node->data.while_stmt.cond);
        free_ast(node->data.while_stmt.body);
        break;

    case AST_FOR:
        free_ast(node->data.for_stmt.init);
        free_ast(node->data.for_stmt.cond);
        free_ast(node->data.for_stmt.step);
        free_ast(node->data.for_stmt.body);
        break;

    case AST_DO_WHILE:
        free_ast(node->data.while_stmt.cond);
        free_ast(node->data.while_stmt.body);
        break;

    case AST_SWITCH:
        free_ast(node->data.switch_stmt.expr);
        free_ast(node->data.switch_stmt.cases);
        break;

    case AST_CASE:
        free_ast(node->data.case_stmt.stmts);
        break;

    case AST_BREAK:
    case AST_CONTINUE:
        break;

    case AST_RETURN:
        free_ast(node->data.return_stmt.expr);
        break;

    case AST_BLOCK:
        free_ast(node->data.block.stmts);
        break;
    }

    free(node);
}