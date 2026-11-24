# Analisador Sintático (Parser)

O parser está em `src/parser.y` e é gerado pelo [Bison](https://www.gnu.org/software/bison/). Ele recebe os tokens do [analisador léxico](lexer.md), **valida a sintaxe** do programa e **constrói a AST**, que o `main` executa. Esta página é a referência do que ele reconhece hoje; a gramática em BNF está em [Gramática](gramatica.md).

> **Estado atual:** cada regra tem uma ação semântica que cria o nó correspondente da AST (seção 5). Depois do `yyparse()`, o `main` executa a árvore com `eval_ast` (veja [AST e Avaliador](ast.md)).

---

## 1. Tokens

Os nomes abaixo são os mesmos usados pelo lexer (`src/lexer.l`). O Bison gera os códigos numéricos em `build/parser.tab.h`, e o lexer inclui esse arquivo.

| Categoria | Tokens |
|---|---|
| Tipos | `T_INT` `T_FLOAT` `T_CHAR` `T_DOUBLE` `T_LONG` |
| Controle de fluxo | `F_IF` `F_ELSE` `F_WHILE` `F_FOR` `F_DO` `F_SWITCH` `F_CASE` `F_BREAK` `F_CONTINUE` `F_RETURN` |
| Literais | `L_INT` `L_FLOAT` `L_CHAR` |
| Identificador | `IDENT` |
| Operadores aritméticos e atribuição | `O_PLUS` `O_MINUS` `O_MULTI` `O_DIV` `O_ASSIGN` |
| Operadores relacionais | `R_EQ` `R_NE` `R_LT` `R_GT` `R_LE` `R_GE` |
| Operadores lógicos | `DM_AND` `DM_OR` `DM_NOT` |
| Delimitadores | `LBRACE` `RBRACE` `LBRACKET` `RBRACKET` `LPAREN` `RPAREN` `COLON` `SEMICOLON` |

Dois nomes não vêm do lexer: `UMINUS` (menos unário) e `SEM_ELSE`. São marcadores só de precedência, usados com `%prec`.

---

## 2. Precedência e associatividade

Da menor para a maior precedência (as linhas de baixo prendem mais forte):

| Nível | Associatividade | Tokens | Para que serve |
|---|---|---|---|
| 1 | — (`%precedence`) | `SEM_ELSE` | `if` sem `else` |
| 2 | — (`%precedence`) | `F_ELSE` | `else` do `if` mais próximo |
| 3 | esquerda | `DM_OR` | `\|\|` |
| 4 | esquerda | `DM_AND` | `&&` |
| 5 | esquerda | `R_EQ` `R_NE` | `==` `!=` |
| 6 | esquerda | `R_LT` `R_GT` `R_LE` `R_GE` | `<` `>` `<=` `>=` |
| 7 | esquerda | `O_PLUS` `O_MINUS` | `+` `-` |
| 8 | esquerda | `O_MULTI` `O_DIV` | `*` `/` |
| 9 | — (`%precedence`) | `DM_NOT` | `!` |
| 10 | — (`%precedence`) | `UMINUS` | `-` unário |

Como a tabela desambigua:

* `1 + 2 * 3` agrupa como `1 + (2 * 3)`, porque `*` está num nível acima de `+`.
* `10 - 4 - 3` agrupa como `(10 - 4) - 3`, porque os operadores são associativos à esquerda.
* `-2 * 3` agrupa como `(-2) * 3`, porque `UMINUS` prende mais forte que `*`.
* `!a && b` agrupa como `(!a) && b`.

### O `else` pendente

Em `if (a) if (b) x = 1; else x = 2;` o `else` poderia pertencer a qualquer um dos dois `if`. As linhas `SEM_ELSE` e `F_ELSE` resolvem isso do jeito do C: o `else` casa com o `if` **mais próximo**, ou seja, o `if (b)`. O Bison compila a gramática com **0 conflitos**.

---

## 3. Regras de produção

O símbolo inicial é `programa`, que é uma sequência (possivelmente vazia) de comandos. Cada linha mostra a regra e uma entrada válida.

| Regra | Exemplo válido |
|---|---|
| `programa` → vazio \| `programa comando` | *(entrada vazia)* |
| `declaracao` → `tipo IDENT ;` | `int x;` |
| `declaracao` → `tipo IDENT = expressao ;` | `float y = 2 * 3;` |
| `comando` → `atribuicao ;` | `x = x + 1;` |
| `comando` → `expressao ;` | `x + 2;` |
| `bloco` → `{ lista_comandos }` | `{ x = 1; y = 2; }` |
| `comando_if` → `if ( expressao ) comando` | `if (x > 0) y = 1;` |
| `comando_if` → `if ( expressao ) comando else comando` | `if (x) y = 1; else y = 2;` |
| `comando_while` → `while ( expressao ) comando` | `while (x < 10) x = x + 1;` |
| `comando_do_while` → `do comando while ( expressao ) ;` | `do { x = x - 1; } while (x > 0);` |
| `comando_for` → `for ( for_init ; for_cond ; for_passo ) comando` | `for (int i = 0; i < 3; i = i + 1) s = s + i;` |
| `comando_switch` → `switch ( expressao ) { lista_casos }` | `switch (x) { case 1: y = 1; break; }` |
| `caso` → `case rotulo : lista_comandos` | `case 'a': y = 2;` |
| `comando` → `break ;` \| `continue ;` | `break;` |
| `comando` → `return ;` \| `return expressao ;` | `return x + 1;` |

Detalhes:

* **`tipo`** é `T_INT`, `T_FLOAT`, `T_CHAR`, `T_DOUBLE` ou `T_LONG`.
* **`for`:** `for_init` pode ser vazio, `tipo IDENT = expressao` ou `IDENT = expressao`. `for_cond` pode ser vazio ou uma expressão. `for_passo` pode ser vazio ou `IDENT = expressao`. Por isso `for (;;) break;` é válido.
* **`rotulo`** do `case` é só `L_INT` ou `L_CHAR`.
* **Expressões:** as mesmas de antes (`+ - * /`, `&& || !`, `== != < > <= >=`, menos unário, parênteses), com operandos `L_INT`, `L_FLOAT`, `L_CHAR` e `IDENT`.

Um arquivo de entrada completo, com todos esses comandos, está em `tests/teste_parser.txt`.

---

## 4. O que ainda não está implementado

O parser **aceita** a sintaxe abaixo; o comportamento é decidido na execução, não na análise sintática:

* `break` ou `continue` fora de um laço (ou de um `switch`, no caso do `break`) **não** dão erro sintático; o erro aparece ao executar (`Erro: 'break' fora de laço ou switch`). Uma análise semântica separada ainda não existe.
* `return` no nível do programa encerra a execução, como o `return` do `main` em C (seção 6).
* `switch` sem `case` é aceito (`switch (x) { }`) e não faz nada.

O parser **não reconhece**:

* Strings, funções e structs: fora do escopo da linguagem. O lexer não tem token para string, e um programa que os use é rejeitado com erro léxico ou sintático (veja `tests/escopo/`).
* `default` dentro do `switch`: o lexer não tem token para ele, então `default` chega como `IDENT`.
* Arrays (`LBRACKET`/`RBRACKET`), várias declarações na mesma linha (`int a, b;`), operadores `++`, `--`, `+=` e afins.
* Declaração e inicialização de mais de uma variável por linha, e `case` com expressão no rótulo (só literal).
* `int main() { ... }`: como não há funções, o programa é a própria lista de comandos.

Além disso, a tabela de símbolos hoje só conhece `int`, `float` e `char`; por isso o parser trata `long` como `int` e `double` como `float`.

---

## 5. Integração com o lexer (`%union` e `yylval`)

O lexer e o parser trocam valores por uma variável global, `yylval`, cujo tipo é a `%union` declarada no `parser.y`:

```c
%union {
    int ival;
    double dval;
    char cval;
    char *sval;
    ASTNode *node;   /* nós da AST montados pelas regras */
    SymType tipo;    /* resultado da regra "tipo" */
}
```

Como a `%union` usa `ASTNode` e `SymType`, o `parser.y` inclui `ast.h` num bloco `%code requires`, que o Bison copia para o `parser.tab.h`. Assim o lexer, que inclui esse cabeçalho, também enxerga os tipos.

Cada token com valor declara qual campo da `%union` ele usa:

| Token | Campo | Tipo |
|---|---|---|
| `L_INT` | `ival` | `int` |
| `L_FLOAT` | `dval` | `double` |
| `L_CHAR` | `cval` | `char` |
| `IDENT` | `sval` | `char *` |

Quando o lexer reconhece `42`, ele escreve `yylval.ival = 42;` e devolve `L_INT`. O parser lê o valor de volta por `$1` na regra (por exemplo `L_INT { $$ = new_num_node($1); }`). O lexer **não declara** a `%union`: ele inclui `build/parser.tab.h`, onde o Bison a gera, e assim os dois sempre concordam sobre os tipos. Só no modo isolado (`make lexer_exe`) o lexer declara a própria `yylval`.

---

### Ações semânticas (construção da AST)

Cada regra devolve em `$$` um nó da AST (`%type <node>`). As principais:

| Regra | Nó criado |
|---|---|
| `L_INT` / `L_FLOAT` / `L_CHAR` / `IDENT` | `new_num_node` / `new_float_node` / `new_char_node` / `new_id_node` |
| `expressao op expressao` | `new_binop_node(op, $1, $3)`; o token vira o código do operador (`R_LE` → `OP_LEQ`, `DM_AND` → `'&'` etc.) |
| `- expressao` / `! expressao` | `new_unaryop_node` |
| `expressao ;` | `new_expr_stmt_node` |
| `atribuicao` | `new_assign_node` |
| `declaracao` | `new_decl_node` (`long` → `TYPE_INT`, `double` → `TYPE_FLOAT`) |
| `bloco` | `new_block_node` |
| `if` / `if-else` | `new_if_stmt_node` |
| `while` / `do-while` / `for` | `new_while_stmt_node` / `new_do_while_node` / `new_for_stmt_node` |
| `switch` / `case` | `new_switch_node` / `new_case_node` (o rótulo é um `int`; um `char` vira o seu código) |
| `break` / `continue` / `return` | `new_break_node` / `new_continue_node` / `new_return_node` |

Listas (`programa`, `lista_comandos`, `lista_casos`) são encadeadas pelo campo `next` com a função auxiliar `anexar`. Os nomes vindos do lexer (`IDENT`, em `sval` alocado com `malloc`) são liberados com `free` logo depois de criar o nó, porque os construtores fazem `strdup`.

---

## 6. Como gerar e compilar

O `Makefile` faz tudo. A ordem importa: o Bison roda primeiro, porque o Flex precisa do `parser.tab.h`.

```bash
make                              # gera build/parser.tab.{c,h} (Bison), build/lex.yy.c (Flex) e o parser_exe
./parser_exe tests/teste_parser.txt
./parser_exe -a tests/teste_parser.txt   # imprime a AST antes de executar
```

O `parser_exe` aceita `[-a] [arquivo]`; sem arquivo, lê a entrada padrão. Depois do `yyparse()`, o `main` envolve a lista de comandos num `AST_BLOCK`, executa com `eval_ast` e libera a árvore com `free_ast`.

Código de saída:

* `1` em erro sintático ou de execução (variável não declarada, divisão por zero, `break`/`continue` fora de laço);
* o valor de `expr` se o programa executar `return expr;` (como o `main` de um programa C), também impresso como `Programa retornou: N`;
* `0` nos demais casos.

O comando que o `Makefile` usa para o Bison é:

```bash
bison -d --defines=build/parser.tab.h -o build/parser.tab.c src/parser.y
```

Pré-requisito: Bison 3.0 ou superior, por causa do `%precedence`. Veja também [Como executar](como_executar.md).

---

## 7. Erros sintáticos

Quando a entrada não casa com a gramática, o Bison chama `yyerror`, que imprime em `stderr`:

```
Erro sintático na linha 3: syntax error
```

e o programa termina com código de saída **1**, sem executar nada. A linha vem do contador `linha_erro` do lexer. Mensagens mais detalhadas e recuperação de erros continuam como objetivo da issue de tratamento de erros sintáticos (#15).

Exemplos:

```bash
echo 'x = 1'     | ./parser_exe   # falta o ';'        -> Erro sintático, saída 1
echo 'int x = ;' | ./parser_exe   # falta a expressão  -> Erro sintático, saída 1
echo 'else x = 1;' | ./parser_exe # else sem if        -> Erro sintático, saída 1
echo 'int x = 1;' | ./parser_exe  # válido              -> "Declarado: x : int = 1", código 0
```

---

## Veja também

* [Gramática](gramatica.md): a BNF completa e os pontos que ainda são só alvo.
* [Analisador Léxico](lexer.md): como cada token é reconhecido.
* [Como executar](como_executar.md): compilar e rodar.
