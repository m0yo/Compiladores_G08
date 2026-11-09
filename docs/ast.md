# AST e Avaliador

Esta página documenta a Árvore Sintática Abstrata (AST) e o avaliador que a executa. O código está em `src/ast.h` (interface) e `src/ast.c` (implementação).

> **Estado atual:** a AST e o avaliador funcionam e são testados por `src/test_main.c`, mas **o parser ainda não constrói a árvore**: as regras de `src/parser.y` não têm ações semânticas. Hoje a árvore só é montada manualmente, nos testes.

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

> Os códigos `OP_EQ`, `OP_NEQ`, `OP_LEQ` e `OP_GEQ` (256 a 259) **não são** os códigos de token do Bison, e os dois conjuntos se sobrepõem numericamente (por exemplo, `OP_LEQ` vale 258, o mesmo número de `T_INT`). Nas ações semânticas do parser é preciso traduzir explicitamente cada token para o seu operador. O módulo `%` não é implementado: o avaliador responde `Erro: Operador binário desconhecido`.

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
| Bloco | Executa a lista de comandos; **não cria um novo escopo** |

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

## Integração com o parser (a fazer)

Para ligar o parser à AST, cada regra de `parser.y` deve chamar o construtor correspondente (veja a tabela em [Gramática](gramatica.md#47-correspondencia-entre-a-gramatica-e-a-ast)). Um cuidado de memória: o lexer entrega `IDENT` e `L_STRING` em `sval` alocado com `malloc`, e `new_id_node`, `new_assign_node` e `new_decl_node` **duplicam** o nome. Portanto o parser deve chamar `free` no `sval` depois de criar o nó, senão há vazamento.

## Testes

`src/test_main.c` cobre a tabela de símbolos, expressões (precedência, promoção `int`/`float`, divisão inteira, relacionais e unários), declaração e atribuição, conversão `float` → `int` na atribuição, `if/else`, `while`, `for` e `char`. Veja [Como executar](como_executar.md) para rodá-lo.
