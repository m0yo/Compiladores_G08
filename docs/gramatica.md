# Guia da Gramática Formal (BNF)

Este documento fornece uma referência clara da sintaxe da linguagem, alinhada com o analisador sintático (`src/parser.y`). Ele tem duas partes:

* a **gramática implementada hoje** no `parser.y` (seção 3): declarações, atribuições, expressões e comandos de controle de fluxo. O detalhamento de tokens, precedência e erros está em [Parser](parser.md);
* a **gramática alvo** (seção 4), a referência de projeto que relaciona cada construção a um nó da AST (veja [AST e Avaliador](ast.md)).

---

## 1. Notação

A tabela abaixo descreve os símbolos utilizados na especificação da gramática formal:

| Símbolo | Significado |
|---|---|
| `::=` | "é definido como" |
| `\|` | alternativa |
| `[ X ]` | opcional |
| `{ X }` | repete 0 ou mais vezes |
| `TOKEN` | terminal, com o nome exato usado em `parser.y` |
| `<nome>` | não-terminal |

---

## 2. Tokens

Os tokens são declarados em `parser.y` e produzidos pelo analisador léxico (veja [Analisador Léxico](lexer.md)).

### Terminais com valor semântico (`%union`)

| Token | Campo da `%union` | Tipo |
|---|---|---|
| `L_INT` | `ival` | `int` |
| `L_FLOAT` | `dval` | `double` |
| `L_CHAR` | `cval` | `char` |
| `IDENT` | `sval` | `char *` |

### Terminais sem valor semântico

* **Tipos:** `T_INT`, `T_FLOAT`, `T_CHAR`, `T_DOUBLE`, `T_LONG`
* **Controle de fluxo:** `F_IF`, `F_ELSE`, `F_WHILE`, `F_FOR`, `F_DO`, `F_SWITCH`, `F_CASE`, `F_DEFAULT`, `F_BREAK`, `F_CONTINUE`, `F_RETURN`
* **Operadores:** `O_PLUS`, `O_MINUS`, `O_MULTI`, `O_DIV`, `O_MOD`, `O_ASSIGN`, `R_EQ`, `R_NE`, `R_LT`, `R_GT`, `R_LE`, `R_GE`, `DM_AND`, `DM_OR`, `DM_NOT`
* **Delimitadores:** `LBRACE`, `RBRACE`, `LBRACKET`, `RBRACKET`, `LPAREN`, `RPAREN`, `COLON`, `SEMICOLON`

---

## 3. Gramática implementada (`parser.y`)

O símbolo inicial é `programa`: uma sequência, possivelmente vazia, de comandos.

```bnf
<programa>       ::= /* vazio */
                   | <programa> <comando>

<comando>        ::= <declaracao>
                   | <atribuicao> SEMICOLON
                   | <expressao> SEMICOLON
                   | <bloco>
                   | <comando-if>
                   | <comando-while>
                   | <comando-do-while>
                   | <comando-for>
                   | <comando-switch>
                   | F_BREAK SEMICOLON
                   | F_CONTINUE SEMICOLON
                   | F_RETURN SEMICOLON
                   | F_RETURN <expressao> SEMICOLON

<declaracao>     ::= <tipo> IDENT SEMICOLON
                   | <tipo> IDENT O_ASSIGN <expressao> SEMICOLON

<atribuicao>     ::= IDENT O_ASSIGN <expressao>

<tipo>           ::= T_INT | T_FLOAT | T_CHAR | T_DOUBLE | T_LONG

<bloco>          ::= LBRACE <lista-comandos> RBRACE
<lista-comandos> ::= /* vazio */
                   | <lista-comandos> <comando>

<comando-if>       ::= F_IF LPAREN <expressao> RPAREN <comando>          (* %prec SEM_ELSE *)
                     | F_IF LPAREN <expressao> RPAREN <comando> F_ELSE <comando>
<comando-while>    ::= F_WHILE LPAREN <expressao> RPAREN <comando>
<comando-do-while> ::= F_DO <comando> F_WHILE LPAREN <expressao> RPAREN SEMICOLON
<comando-for>      ::= F_FOR LPAREN <for-init> SEMICOLON <for-cond> SEMICOLON <for-passo> RPAREN <comando>

<for-init>  ::= /* vazio */ | <tipo> IDENT O_ASSIGN <expressao> | <atribuicao>
<for-cond>  ::= /* vazio */ | <expressao>
<for-passo> ::= /* vazio */ | <atribuicao>

<comando-switch> ::= F_SWITCH LPAREN <expressao> RPAREN LBRACE <lista-casos> RBRACE
<lista-casos>    ::= /* vazio */
                   | <lista-casos> <caso>
<caso>           ::= F_CASE <rotulo> COLON <lista-comandos>
                   | F_DEFAULT COLON <lista-comandos>
<rotulo>         ::= L_INT | L_CHAR

<expressao> ::= <expressao> O_PLUS <expressao>
              | <expressao> O_MINUS <expressao>
              | <expressao> O_MULTI <expressao>
              | <expressao> O_DIV <expressao>
              | <expressao> O_MOD <expressao>
              | O_MINUS <expressao>                  (* %prec UMINUS *)
              | <expressao> DM_AND <expressao>
              | <expressao> DM_OR <expressao>
              | DM_NOT <expressao>
              | <expressao> R_EQ <expressao>
              | <expressao> R_NE <expressao>
              | <expressao> R_LT <expressao>
              | <expressao> R_GT <expressao>
              | <expressao> R_LE <expressao>
              | <expressao> R_GE <expressao>
              | LPAREN <expressao> RPAREN
              | L_INT
              | L_FLOAT
              | L_CHAR
              | IDENT
```

### Precedência e associatividade

Da menor para a maior precedência (as linhas de baixo prendem mais forte):

| Nível | Associatividade | Tokens |
|---|---|---|
| 1 | esquerda | `DM_OR` |
| 2 | esquerda | `DM_AND` |
| 3 | esquerda | `R_EQ` `R_NE` |
| 4 | esquerda | `R_LT` `R_GT` `R_LE` `R_GE` |
| 5 | esquerda | `O_PLUS` `O_MINUS` |
| 6 | esquerda | `O_MULTI` `O_DIV` `O_MOD` |
| 7 | — (`%precedence`) | `DM_NOT` |
| 8 | — (`%precedence`) | `UMINUS` (menos unário) |

`SEM_ELSE` e `F_ELSE` ficam abaixo de todos os operadores e resolvem o *else pendente* (o `else` casa com o `if` mais próximo). Exemplos: `1 + 2 * 3` agrupa como `1 + (2 * 3)`; `-2 * 3` agrupa como `(-2) * 3`; `!a && b` agrupa como `(!a) && b`.

### Comportamento atual

* Cada regra tem uma ação semântica que constrói o nó correspondente da AST; o `main` executa a árvore com `eval_ast`. Os detalhes estão em [Parser](parser.md).
* `long` e `double` são aceitos, mas tratados como `int` e `float`, porque a tabela de símbolos só tem esses tipos.
* Erros sintáticos são impressos em `stderr` como `Erro sintático na linha N: <mensagem>`, e o código de saída é `1`.
* `break`, `continue` e `return` são aceitos em qualquer lugar pela gramática. Um `break`/`continue` fora de laço só dá erro na execução; `return` no nível do programa encerra a execução.
* `LBRACKET` e `RBRACKET` são produzidos pelo lexer, mas nenhuma regra os consome ainda.

---

## 4. Gramática alvo

> Esta seção é a referência de projeto da linguagem. O `parser.y` reconhece tudo que está aqui (veja a seção 3) e já constrói os nós da AST. Em relação à versão anterior desta página, os nomes dos terminais foram alinhados aos que o lexer realmente produz (`L_INT`, `IDENT`, `O_PLUS`, `F_IF`, `DM_AND`, `R_EQ` etc.) e foram removidos `T_BOOL`, `TRUE_LIT` e `FALSE_LIT`, que não existem no lexer.

### 4.1 Programa

Um programa é composto por uma lista de linhas ou instruções sequenciais.

```bnf
<programa>      ::= <lista-linhas>

<lista-linhas>  ::= /* vazio */
                  | <lista-linhas> <linha>

<linha>         ::= <comando>
```

### 4.2 Tipos

Os tipos de dados primitivos para a declaração de variáveis são os que a tabela de símbolos e a AST já suportam:

```bnf
<tipo-especificador> ::= T_INT | T_FLOAT | T_CHAR
```

> `T_LONG` e `T_DOUBLE` são produzidos pelo lexer, mas `SymType` (veja [Tabela de Símbolos](tabela_simbolos.md)) só tem `int`, `float` e `char`. Para aceitá-los é preciso ampliar a tabela de símbolos e o avaliador.

### 4.3 Comandos

```bnf
<comando> ::= <expressao> SEMICOLON

           | IDENT O_ASSIGN <expressao> SEMICOLON

           | <tipo-especificador> IDENT SEMICOLON

           | <tipo-especificador> IDENT O_ASSIGN <expressao> SEMICOLON

           | F_IF LPAREN <expressao> RPAREN <comando>

           | F_IF LPAREN <expressao> RPAREN <comando> F_ELSE <comando>

           | F_WHILE LPAREN <expressao> RPAREN <comando>

           | F_FOR LPAREN <for-init> SEMICOLON <for-cond> SEMICOLON <for-step> RPAREN <comando>

           | LBRACE <lista-comandos> RBRACE
```

Explicação das regras de `<comando>`:

* **Expressão avaliada:** `<expressao> SEMICOLON` executa uma expressão isolada como comando, encerrada por ponto e vírgula (nó `AST_EXPR_STMT`).
* **Atribuição:** `IDENT O_ASSIGN <expressao> SEMICOLON` atribui o resultado de uma expressão a uma variável já declarada (`AST_ASSIGN`).
* **Declaração simples:** `<tipo-especificador> IDENT SEMICOLON` declara uma variável informando tipo e identificador (`AST_DECL`, sem inicialização).
* **Declaração com inicialização:** `<tipo-especificador> IDENT O_ASSIGN <expressao> SEMICOLON` declara a variável e já atribui um valor inicial (`AST_DECL`, com `init`).
* **Condicional (`if` e `if-else`):** executa um comando se a expressão entre parênteses for verdadeira (diferente de zero) e, opcionalmente, outro comando se for falsa (`AST_IF`).
* **Laço `while`:** repete um comando enquanto a condição for verdadeira (`AST_WHILE`).
* **Laço `for`:** loop controlado por inicialização, condição e passo (`AST_FOR`).
* **Bloco de comandos:** `LBRACE <lista-comandos> RBRACE` agrupa vários comandos entre chaves (`AST_BLOCK`). Na implementação atual o bloco **não** cria um novo escopo: a tabela de símbolos é única.

### 4.4 Componentes do `for`

```bnf
<for-init> ::= /* vazio */
             | <tipo-especificador> IDENT O_ASSIGN <expressao>
             | IDENT O_ASSIGN <expressao>

<for-cond> ::= /* vazio */
             | <expressao>

<for-step> ::= /* vazio */
             | IDENT O_ASSIGN <expressao>
```

* **`<for-init>`:** inicialização do loop; pode ser vazia, uma declaração com atribuição ou uma atribuição a uma variável existente.
* **`<for-cond>`:** condição de continuidade; se omitida o laço só termina com `break` ou `return`.
* **`<for-step>`:** passo executado a cada iteração; pode ser vazio ou uma atribuição.

### 4.5 Bloco

```bnf
<lista-comandos> ::= /* vazio */
                   | <lista-comandos> <comando>
```

`<lista-comandos>` é uma sequência de comandos que pode ser vazia ou recursiva, permitindo encadear vários comandos dentro de um bloco.

### 4.6 Expressões

A gramática de `<expressao>` é a mesma da seção 3, com a mesma tabela de precedência. As expressões englobam operações aritméticas, lógicas e relacionais, parênteses, literais (`L_INT`, `L_FLOAT`, `L_CHAR`) e identificadores (`IDENT`).

### 4.7 Correspondência entre a gramática e a AST

Cada regra da gramática tem um construtor de nó em `src/ast.c`, chamado pela ação semântica correspondente em `parser.y`:

| Regra | Construtor |
|---|---|
| `L_INT` / `L_FLOAT` / `L_CHAR` | `new_num_node` / `new_float_node` / `new_char_node` |
| `IDENT` (em expressão) | `new_id_node` |
| `<expressao> op <expressao>` | `new_binop_node` |
| `O_MINUS <expressao>` / `DM_NOT <expressao>` | `new_unaryop_node` |
| `<expressao> SEMICOLON` | `new_expr_stmt_node` |
| `IDENT O_ASSIGN <expressao> SEMICOLON` | `new_assign_node` |
| declaração | `new_decl_node` |
| `if` / `if-else` | `new_if_stmt_node` |
| `while` | `new_while_stmt_node` |
| `for` | `new_for_stmt_node` |
| `do-while` | `new_do_while_node` |
| `switch` / `case` | `new_switch_node` / `new_case_node` |
| `break` / `continue` / `return` | `new_break_node` / `new_continue_node` / `new_return_node` |
| `LBRACE ... RBRACE` | `new_block_node` |

A tradução de token para código de operador (por exemplo `R_LE` para `OP_LEQ`) está em [AST e Avaliador](ast.md#operadores).

---

## 5. Restrições de escopo do projeto

Para manter o interpretador focado no aprendizado dos conceitos fundamentais de análise léxica, sintática e semântica, o escopo da linguagem restringe estritamente:

* Sem suporte a ponteiros (*pointers*).
* Sem suporte a estruturas (*structs*).
* Sem suporte a literais de string (*strings*).
* Sem suporte a definição nem chamada de funções.
* Sem importação de bibliotecas externas ou uso de funções de cabeçalhos complexos do C padrão.

Também ficam fora, por enquanto, arrays. Programas que usam strings, funções ou structs são rejeitados com erro léxico ou sintático (veja `tests/escopo/`). Já `do-while`, `switch/case/default`, `break`, `continue` e `return` são reconhecidos pelo parser e executados pelo avaliador.
