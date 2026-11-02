#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ast.h"
#include "tabela_simbolos.h"

static int falhas = 0;
#define CHECK(desc, cond) do { \
    printf("[%s] %s\n", (cond) ? "OK " : "FALHOU", desc); \
    if (!(cond)) falhas++; } while (0)

static ASTNode *seq(ASTNode *a, ASTNode *b) { a->next = b; return a; }

int main(void) {
    resultado r;

    /* 1. Tabela de simbolos */
    SymValue v; v.intValue = 7;
    inserir_simbolo("x", TYPE_INT, v);
    SymEntry *e = buscar_simbolo("x");
    CHECK("inserir/buscar x=7", e && e->value.intValue == 7);
    CHECK("buscar inexistente retorna NULL", buscar_simbolo("zzz") == NULL);
    v.intValue = 9; inserir_simbolo("x", TYPE_INT, v);
    CHECK("reinserir atualiza valor", buscar_simbolo("x")->value.intValue == 9);
    limpar_tabela();
    CHECK("limpar_tabela esvazia", buscar_simbolo("x") == NULL);

    /* 2. Expressoes: (2 + 3) * 4 = 20 */
    ASTNode *t = new_binop_node('*', new_binop_node('+', new_num_node(2), new_num_node(3)), new_num_node(4));
    r = eval_ast(t);
    CHECK("(2+3)*4 == 20 (int)", r.val == 20 && r.Type == TYPE_INT);
    print_ast(t, 0); free_ast(t);

    /* 3. Promocao: 1 + 2.5 = 3.5 float */
    t = new_binop_node('+', new_num_node(1), new_float_node(2.5));
    r = eval_ast(t);
    CHECK("1 + 2.5 == 3.5 (float)", fabs(r.val - 3.5) < 1e-9 && r.Type == TYPE_FLOAT);
    free_ast(t);

    /* 4. Divisao inteira e relacionais */
    t = new_binop_node('/', new_num_node(7), new_num_node(2));
    r = eval_ast(t); CHECK("7/2 == 3 (int)", r.val == 3 && r.Type == TYPE_INT); free_ast(t);
    t = new_binop_node(OP_EQ, new_num_node(3), new_num_node(3));
    r = eval_ast(t); CHECK("3 == 3 -> 1", r.val == 1); free_ast(t);
    t = new_binop_node(OP_LEQ, new_num_node(4), new_num_node(3));
    r = eval_ast(t); CHECK("4 <= 3 -> 0", r.val == 0); free_ast(t);
    t = new_unaryop_node('-', new_num_node(5));
    r = eval_ast(t); CHECK("-5", r.val == -5); free_ast(t);
    t = new_unaryop_node('!', new_num_node(0));
    r = eval_ast(t); CHECK("!0 -> 1", r.val == 1); free_ast(t);

    /* 5. Declaracao + atribuicao: int a = 10; a = a + 1; */
    t = seq(new_decl_node(TYPE_INT, "a", new_num_node(10)),
            new_assign_node("a", new_binop_node('+', new_id_node("a"), new_num_node(1))));
    eval_ast(t); eval_ast(t->next);
    CHECK("a == 11", buscar_simbolo("a")->value.intValue == 11);
    free_ast(t);

    /* 6. Conversao ao atribuir float em int: int b = 3.9; -> 3 */
    t = new_decl_node(TYPE_INT, "b", new_float_node(3.9));
    eval_ast(t);
    CHECK("int b = 3.9 -> 3", buscar_simbolo("b")->value.intValue == 3);
    free_ast(t);

    /* 7. if/else */
    t = new_if_stmt_node(new_num_node(0),
            new_assign_node("a", new_num_node(100)),
            new_assign_node("a", new_num_node(200)));
    eval_ast(t);
    CHECK("if(0) ... else a=200", buscar_simbolo("a")->value.intValue == 200);
    free_ast(t);

    /* 8. while: int i = 0; while (i < 5) i = i + 1; */
    t = new_decl_node(TYPE_INT, "i", new_num_node(0)); eval_ast(t); free_ast(t);
    t = new_while_stmt_node(new_binop_node('<', new_id_node("i"), new_num_node(5)),
            new_assign_node("i", new_binop_node('+', new_id_node("i"), new_num_node(1))));
    eval_ast(t);
    CHECK("while termina com i == 5", buscar_simbolo("i")->value.intValue == 5);
    free_ast(t);

    /* 9. for: soma = 0; for (k = 0; k < 4; k = k+1) soma = soma + k;  -> 6 */
    t = new_decl_node(TYPE_INT, "soma", new_num_node(0)); eval_ast(t); free_ast(t);
    t = new_decl_node(TYPE_INT, "k", NULL); eval_ast(t); free_ast(t);
    t = new_for_stmt_node(
            new_assign_node("k", new_num_node(0)),
            new_binop_node('<', new_id_node("k"), new_num_node(4)),
            new_assign_node("k", new_binop_node('+', new_id_node("k"), new_num_node(1))),
            new_block_node(new_assign_node("soma", new_binop_node('+', new_id_node("soma"), new_id_node("k")))));
    eval_ast(t);
    CHECK("for: soma == 6", buscar_simbolo("soma")->value.intValue == 6);
    print_ast(t, 0); free_ast(t);

    /* 10. char */
    t = new_decl_node(TYPE_CHAR, "c", new_char_node('A')); eval_ast(t); free_ast(t);
    CHECK("char c = 'A'", buscar_simbolo("c")->value.charValue == 'A');

    sym_print();
    limpar_tabela();
    printf("\n%s (%d falha(s))\n", falhas ? "HOUVE FALHAS" : "TODOS OS TESTES PASSARAM", falhas);
    return falhas != 0;
}
