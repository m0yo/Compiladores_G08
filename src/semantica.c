/*
 Analise semantica: percorre a AST antes da execucao e detecta
 - variavel usada sem declaracao
 - variavel declarada duas vezes
 - break/continue fora de laco (ou switch, no caso do break)
 - valor repetido em "case" do mesmo switch
 - operador % com operando float
 e avisa (sem impedir a execucao) quando
 - um float e atribuido a int/char (perde a parte decimal)
 - uma variavel pode ser usada sem valor (nenhuma atribuicao antes no texto)
 - ha divisao por zero com divisor literal

 O escopo e unico (global), como no avaliador: uma variavel declarada
 dentro de um bloco continua visivel depois dele.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantica.h"

typedef struct {
    char *nome;
    SymType tipo;
    int com_valor;  //1 se houve inicializacao ou atribuicao antes, no texto
} Declarada;

static Declarada *vars = NULL;
static int n_vars = 0;
static int cap_vars = 0;

static int n_erros = 0;
static int prof_laco = 0;    //quantos lacos (while/do/for) envolvem o ponto atual
static int prof_switch = 0;  //quantos switch envolvem o ponto atual

static void mensagem(const char *rotulo, const ASTNode *no, const char *fmt, va_list ap) {
    if (no && no->linha > 0)
        fprintf(stderr, "%s na linha %d: ", rotulo, no->linha);
    else
        fprintf(stderr, "%s: ", rotulo);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

static void erro(const ASTNode *no, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    mensagem("Erro semântico", no, fmt, ap);
    va_end(ap);
    n_erros++;
}

static void aviso(const ASTNode *no, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    mensagem("Aviso semântico", no, fmt, ap);
    va_end(ap);
}

static int procurar(const char *nome) {
    for (int i = 0; i < n_vars; i++)
        if (strcmp(vars[i].nome, nome) == 0) return i;
    return -1;
}

static void declarar(const char *nome, SymType tipo, int com_valor) {
    if (n_vars == cap_vars) {
        cap_vars = cap_vars ? cap_vars * 2 : 16;
        vars = (Declarada *)realloc(vars, (size_t)cap_vars * sizeof(Declarada));
        if (!vars) {
            fprintf(stderr, "Erro: Falha na alocação de memória na análise semântica!\n");
            exit(EXIT_FAILURE);
        }
    }
    vars[n_vars].nome = strdup(nome);
    vars[n_vars].tipo = tipo;
    vars[n_vars].com_valor = com_valor;
    n_vars++;
}

static void limpar(void) {
    for (int i = 0; i < n_vars; i++) free(vars[i].nome);
    free(vars);
    vars = NULL;
    n_vars = cap_vars = 0;
    n_erros = prof_laco = prof_switch = 0;
}

//float para int/char perde a parte decimal
static void checar_conversao(const ASTNode *no, SymType destino, SymType origem, const char *nome) {
    if (origem == TYPE_FLOAT && destino != TYPE_FLOAT)
        aviso(no, "'%s' recebe um float e será convertido para %s (perde a parte decimal)",
              nome, tipo_para_string(destino));
}

static int literal_zero(const ASTNode *e) {
    if (!e) return 0;
    if (e->tipo == AST_INT) return e->data.num.value == 0;
    if (e->tipo == AST_FLOAT) return e->data.flt.value == 0.0;
    return 0;
}

static int operador_booleano(int op) {
    return op == '<' || op == '>' || op == '&' || op == '|' ||
           op == OP_EQ || op == OP_NEQ || op == OP_LEQ || op == OP_GEQ;
}

//verifica a expressao e devolve o tipo dela (int, float ou char)
static SymType tipo_expr(const ASTNode *e) {
    if (!e) return TYPE_INT;

    switch (e->tipo) {
    case AST_INT:   return TYPE_INT;
    case AST_FLOAT: return TYPE_FLOAT;
    case AST_CHAR:  return TYPE_CHAR;

    case AST_ID: {
        int i = procurar(e->data.id.name);
        if (i < 0) {
            erro(e, "variável '%s' não declarada", e->data.id.name);
            return TYPE_INT;
        }
        if (!vars[i].com_valor) {
            aviso(e, "variável '%s' pode ser usada sem valor", e->data.id.name);
            vars[i].com_valor = 1;  //avisa uma vez so
        }
        return vars[i].tipo;
    }

    case AST_BINARYOP: {
        SymType l = tipo_expr(e->data.binaryop.left);
        SymType r = tipo_expr(e->data.binaryop.right);
        int op = e->data.binaryop.op;
        if ((op == '/' || op == '%') && literal_zero(e->data.binaryop.right))
            aviso(e, "divisão por zero");
        if (op == '%') {
            if (l == TYPE_FLOAT || r == TYPE_FLOAT)
                erro(e, "operador '%%' exige operandos inteiros (int ou char)");
            return TYPE_INT;
        }
        if (operador_booleano(op)) return TYPE_INT;
        return (l == TYPE_FLOAT || r == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
    }

    case AST_UNARYOP: {
        SymType t = tipo_expr(e->data.unaryop.operand);
        return e->data.unaryop.op == '!' ? TYPE_INT : t;
    }

    default:
        return TYPE_INT;
    }
}

static void visitar(const ASTNode *no);

static void visitar_switch(const ASTNode *no) {
    tipo_expr(no->data.switch_stmt.expr);

    int n = 0;
    for (const ASTNode *c = no->data.switch_stmt.cases; c; c = c->next) n++;
    int *vistos = (int *)malloc((size_t)(n ? n : 1) * sizeof(int));
    if (!vistos) {
        fprintf(stderr, "Erro: Falha na alocação de memória na análise semântica!\n");
        exit(EXIT_FAILURE);
    }
    int n_vistos = 0;

    prof_switch++;
    for (const ASTNode *c = no->data.switch_stmt.cases; c; c = c->next) {
        int repetido = 0;
        for (int i = 0; i < n_vistos; i++)
            if (vistos[i] == c->data.case_stmt.value) repetido = 1;
        if (repetido)
            erro(c, "valor 'case %d' repetido no switch", c->data.case_stmt.value);
        else
            vistos[n_vistos++] = c->data.case_stmt.value;
        visitar(c->data.case_stmt.stmts);
    }
    prof_switch--;
    free(vistos);
}

//percorre uma lista de comandos (encadeada por ->next)
static void visitar(const ASTNode *no) {
    for (; no; no = no->next) {
        switch (no->tipo) {
        case AST_DECL: {
            SymType t = tipo_expr(no->data.decl.init);
            if (procurar(no->data.decl.name) >= 0) {
                erro(no, "variável '%s' já declarada", no->data.decl.name);
            } else {
                declarar(no->data.decl.name, no->data.decl.type, no->data.decl.init != NULL);
                if (no->data.decl.init)
                    checar_conversao(no, no->data.decl.type, t, no->data.decl.name);
            }
            break;
        }

        case AST_ASSIGN: {
            int i = procurar(no->data.assign.name);
            SymType t = tipo_expr(no->data.assign.expr);
            if (i < 0)
                erro(no, "variável '%s' não declarada", no->data.assign.name);
            else {
                checar_conversao(no, vars[i].tipo, t, no->data.assign.name);
                vars[i].com_valor = 1;
            }
            break;
        }

        case AST_EXPR_STMT:
            tipo_expr(no->data.expr_stmt.expr);
            break;

        case AST_IF:
            tipo_expr(no->data.if_stmt.cond);
            visitar(no->data.if_stmt.then_branch);
            visitar(no->data.if_stmt.else_branch);
            break;

        case AST_WHILE:
            tipo_expr(no->data.while_stmt.cond);
            prof_laco++;
            visitar(no->data.while_stmt.body);
            prof_laco--;
            break;

        case AST_DO_WHILE:
            prof_laco++;
            visitar(no->data.while_stmt.body);
            prof_laco--;
            tipo_expr(no->data.while_stmt.cond);
            break;

        case AST_FOR:
            visitar(no->data.for_stmt.init);
            tipo_expr(no->data.for_stmt.cond);
            prof_laco++;
            visitar(no->data.for_stmt.body);
            visitar(no->data.for_stmt.step);
            prof_laco--;
            break;

        case AST_SWITCH:
            visitar_switch(no);
            break;

        case AST_BREAK:
            if (prof_laco == 0 && prof_switch == 0)
                erro(no, "'break' fora de laço ou switch");
            break;

        case AST_CONTINUE:
            if (prof_laco == 0)
                erro(no, "'continue' fora de laço");
            break;

        case AST_RETURN:
            tipo_expr(no->data.return_stmt.expr);
            break;

        case AST_BLOCK:
            visitar(no->data.block.stmts);
            break;

        default:
            break;
        }
    }
}

int analisar_semantica(const ASTNode *programa) {
    limpar();
    visitar(programa);
    int total = n_erros;
    limpar();
    return total;
}
