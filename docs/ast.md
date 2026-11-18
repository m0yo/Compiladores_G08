# AST e Avaliador

Esta página documenta a Árvore Sintática Abstrata (AST) e o avaliador que a executa. O código está em `src/ast.h` (interface) e `src/ast.c` (implementação).

> **Estado atual:** o parser (`src/parser.y`) constrói a AST a partir do código-fonte e o `main` a executa com `eval_ast`. A AST também é testada isoladamente por `src/test_main.c`.

## Estrutura de um nó

Todo nó é um `ASTNode` com três partes:

* `tipo` (`TipoNo`): qual é o tipo do nó;
* `next`: próximo comando da lista, ou `NULL`. É assim que uma sequência de comandos é representada (`a; b; c;` vira `a → b → c`);
* `data`: uma `union` com os campos específicos de cada tipo de nó.

Os campos das expressões e dos comandos são:

| Nó | Campos (`data.`) | Construtor |
|---|---|---|
| `AST_INT` | `num.value` (`int`) | `new_num_node(int)` |
| `AST_FLOAT` | `flt.value` (`double`) | `new_float_node(double)` |
| `AST_CHAR` | `chr.value` (`char`) | `new_char_node(char)` |
| `AST_ID` | `id.name` | `new_id_node(const char *)` |
| `AST_BINARYOP` | `binaryop.op`, `left`, `right` | `new_binop_node(op, left, right)` |
| `AST_UNARYOP` | `unaryop.op`, `operand` | `new_unaryop_node(op, operand)` |
| `AST_DECL` | `decl.type`, `name`, `init` (`NULL` se não há inicialização) | `new_decl_node(type, name, init)` |
| `AST_ASSIGN` | `assign.name`, `expr` | `new_assign_node(name, expr)` |
| `AST_EXPR_STMT` | `expr_stmt.expr` | `new_expr_stmt_node(expr)` |
| `AST_IF` | `if_stmt.cond`, `then_branch`, `else_branch` (`NULL` se não há `else`) | `new_if_stmt_node(cond, then, else)` |
| `AST_WHILE` | `while_stmt.cond`, `body` | `new_while_stmt_node(cond, body)` |
| `AST_FOR` | `for_stmt.init`, `cond`, `step`, `body` | `new_for_stmt_node(init, cond, step, body)` |
| `AST_DO_WHILE` | `while_stmt.body`, `cond` (mesmos campos do `while`) | `new_do_while_node(body, cond)` |
| `AST_SWITCH` | `switch_stmt.expr`, `cases` (lista de `AST_CASE` via `next`) | `new_switch_node(expr, cases)` |
| `AST_CASE` | `case_stmt.value` (`int`), `stmts` | `new_case_node(value, stmts)` |
| `AST_BREAK` / `AST_CONTINUE` | — | `new_break_node()` / `new_continue_node()` |
| `AST_RETURN` | `return_stmt.expr` (`NULL` em `return;`) | `new_return_node(expr)` |
| `AST_BLOCK` | `block.stmts` (cabeça da lista de comandos) | `new_block_node(stmts)` |

Os construtores alocam o nó com `malloc` e **copiam os nomes com `strdup`**. Se a alocação do nó falha, o programa imprime um erro e termina.

## Operadores

O campo `op` guarda o próprio caractere para `+ - * / < > ! & |` e uma constante `OP_*` para os que não cabem em um único caractere:

| Token (lexer) | Código em `op` | Impresso por `print_ast` |
|---|---|---|
| `O_PLUS` | `'+'` | `+` |
| `O_MINUS` | `'-'` (binário e unário) | `-` |
| `O_MULTI` | `'*'` | `*` |
| `O_DIV` | `'/'` | `/` |
| `DM_AND` | `'&'` | `&&` |
| `DM_OR` | `'\|'` | `\|\|` |
| `DM_NOT` | `'!'` | `!` |
| `R_EQ` | `OP_EQ` | `==` |
| `R_NE` | `OP_NEQ` | `!=` |
| `R_LT` | `'<'` | `<` |
| `R_GT` | `'>'` | `>` |
| `R_LE` | `OP_LEQ` | `<=` |
| `R_GE` | `OP_GEQ` | `>=` |

> Os códigos `OP_EQ`, `OP_NEQ`, `OP_LEQ` e `OP_GEQ` (256 a 259) **não são** os códigos de token do Bison, e os dois conjuntos se sobrepõem numericamente (por exemplo, `OP_LEQ` vale 258, o mesmo número de `T_INT`). Por isso as ações semânticas do parser traduzem explicitamente cada token para o seu operador. O módulo `%` não é implementado: o avaliador responde `Erro: Operador binário desconhecido`.

## Avaliador (`eval_ast`)

```c
resultado eval_ast(ASTNode *node);
```

O avaliador percorre a árvore recursivamente e executa o programa. Cada chamada devolve um `resultado`, com o valor numérico (`val`, um `double`) e o tipo (`Type`, um `SymType`).

Listas de comandos (`next`, corpo de `while`/`for`, ramos do `if` e `AST_BLOCK`) são executadas em ordem por uma função auxiliar. O avaliador consulta e altera a [tabela de símbolos](tabela_simbolos.md).

### Regras de avaliação

| Construção | Comportamento |
|---|---|
| Literais | Devolvem o valor com tipo `int`, `float` ou `char` |
| Identificador | Busca a variável na tabela de símbolos |
| Aritmética | Se algum operando é `float`, o resultado é `float`; caso contrário é `int` (um `char` é promovido a `int`). Em `int` os operandos são convertidos com truncamento e `/` é divisão inteira |
| Comparações e `==`, `!=` | Resultado `int`: `1` ou `0` |
| `&&`, `\|\|` | Resultado `int` (`1` ou `0`). **Os dois lados são sempre avaliados** (não há curto-circuito) |
| `-x` | Mantém o tipo do operando |
| `!x` | Resultado `int` (`1` se `x` é zero) |
| Atribuição | Avalia a expressão e converte o valor para o **tipo declarado** da variável (por exemplo `int b = 3.9` guarda `3`) |
| Declaração | Insere a variável na tabela; sem inicialização o valor é `0` |
| `if` | Executa o ramo `then` se a condição é diferente de zero, senão o `else` (se houver) |
| `while` | Repete o corpo enquanto a condição é diferente de zero |
| `for` | Executa `init` uma vez; a cada volta testa `cond` (se existir), executa o corpo e depois `step` |
| `do-while` | Executa o corpo e depois testa a condição; o corpo roda pelo menos uma vez |
| `switch` | Avalia a expressão, procura o primeiro `case` com o mesmo valor e executa a partir dele, **seguindo para os `case` seguintes** (*fall-through*) até um `break`. Sem `case` correspondente, nada é executado (não há `default`) |
| `break` | Sai do laço ou `switch` mais interno |
| `continue` | Pula para a próxima volta do laço mais interno (no `for`, o `step` ainda é executado) |
| `return` | Interrompe a execução e guarda o valor em `valor_retorno` |
| Bloco | Executa a lista de comandos; **não cria um novo escopo** |

### Controle de fluxo (`break`, `continue`, `return`)

Os desvios usam uma variável global, `controle_fluxo` (`CTRL_NORMAL`, `CTRL_BREAK`, `CTRL_CONTINUE` ou `CTRL_RETURN`):

* `AST_BREAK`, `AST_CONTINUE` e `AST_RETURN` só mudam esse estado (o `return` também guarda o valor em `valor_retorno`);
* a função que executa listas de comandos para assim que o estado deixa de ser `CTRL_NORMAL`, então o resto do bloco é pulado e o sinal "sobe" pela árvore;
* os laços consomem `break` (saem) e `continue` (seguem para a próxima volta); o `switch` consome só o `break`, deixando o `continue` chegar ao laço de fora;
* `return` não é consumido por ninguém e encerra o programa. O `main` do parser imprime `Programa retornou: N` e usa `N` como código de saída. Se sobrar um `break` ou `continue` no fim, ele estava fora de laço, e o `main` imprime um erro.

### Saída impressa

O avaliador imprime no `stdout` um resumo do que executa. Não existe ainda um comando de impressão da linguagem:

| Situação | Saída |
|---|---|
| Declaração sem valor inicial | `Declarado: x : int` |
| Declaração com valor inicial | `Declarado: x : int = 10` |
| Atribuição | `int x = 11` (tipo, nome e valor) |
| Expressão como comando | `Resultado da expressão: 20` |

Cada atribuição executada imprime uma linha, inclusive a cada iteração de um laço. Valores `char` imprimíveis saem entre aspas simples (`'A'`); os demais saem como número.

### Erros

Os erros abaixo imprimem a mensagem em `stderr` e **encerram o programa** com `exit(EXIT_FAILURE)`; não há recuperação:

* variável não declarada: `Erro: variável 'x' não declarada!`;
* divisão por zero: `Erro: Divisão por zero!`;
* operador desconhecido.

### Pontos de atenção

* Declarar de novo uma variável existente **não é erro**: `inserir_simbolo` só atualiza o valor e mantém o tipo original.
* A tabela de símbolos é única e global: variáveis declaradas dentro de um bloco continuam visíveis depois dele.

## Visualização e memória

```c
void print_ast(ASTNode *node, int level);
void free_ast(ASTNode *node);
```

* `print_ast` imprime a árvore com indentação de dois espaços por nível e percorre também os nós ligados por `next`. Exemplo (laço `for` dos testes):

```
FOR
  init:
    ASSIGN (k)
      NUM (0)
  cond:
    BINOP (<)
      ID (k)
      NUM (4)
  step:
    ASSIGN (k)
      BINOP (+)
        ID (k)
        NUM (1)
  body:
    BLOCK
      ASSIGN (soma)
        BINOP (+)
          ID (soma)
          ID (k)
```

* `free_ast` libera recursivamente a árvore inteira, incluindo os nós ligados por `next` e os nomes copiados com `strdup`. Depois de liberar um nó, ele não deve mais ser usado.

## Integração com o parser

Cada regra de `parser.y` chama o construtor correspondente (veja a tabela em [Gramática](gramatica.md#47-correspondencia-entre-a-gramatica-e-a-ast) e em [Parser](parser.md)). A raiz é a lista de comandos do programa, que o `main` envolve num `AST_BLOCK` antes de chamar `eval_ast` e `free_ast`. Cuidado de memória: o lexer entrega `IDENT` em `sval` alocado com `malloc`, e `new_id_node`, `new_assign_node` e `new_decl_node` **duplicam** o nome; por isso o parser chama `free` no `sval` logo após criar o nó.

## Testes

`src/test_main.c` cobre a tabela de símbolos, expressões (precedência, promoção `int`/`float`, divisão inteira, relacionais e unários), declaração e atribuição, conversão `float` → `int` na atribuição, `if/else`, `while`, `for` e `char`. Os programas em `tests/programas/` testam o pipeline completo (lexer → parser → AST → `eval_ast`), inclusive `break`, `continue`, `do-while`, `switch` com *fall-through* e `return`. Tudo roda com `make test`; veja [Como executar](como_executar.md).
