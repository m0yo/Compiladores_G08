# Guia da Gramática Formal (BNF)

Este documento fornece uma referência clara da sintaxe da linguagem, alinhada com o analisador sintático (`src/parser.y`). Ele tem duas partes:

* a **gramática implementada hoje** no `parser.y` (seção 3), que reconhece apenas uma expressão;
* a **gramática alvo** (seção 4), com declarações, atribuições e comandos, que será implementada nas próximas etapas. Ela já corresponde aos nós da AST (veja [AST e Avaliador](ast.md)).

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
| `L_STRING` | `sval` | `char *` |
| `IDENT` | `sval` | `char *` |

### Terminais sem valor semântico

* **Tipos:** `T_INT`, `T_FLOAT`, `T_CHAR`, `T_DOUBLE`, `T_LONG`
* **Controle de fluxo:** `F_IF`, `F_ELSE`, `F_WHILE`, `F_FOR`, `F_DO`, `F_SWITCH`, `F_CASE`, `F_BREAK`, `F_CONTINUE`, `F_RETURN`
* **Operadores:** `O_PLUS`, `O_MINUS`, `O_MULTI`, `O_DIV`, `O_ASSIGN`, `R_EQ`, `R_NE`, `R_LT`, `R_GT`, `R_LE`, `R_GE`, `DM_AND`, `DM_OR`, `DM_NOT`
* **Delimitadores:** `LBRACE`, `RBRACE`, `LBRACKET`, `RBRACKET`, `LPAREN`, `RPAREN`, `COLON`, `SEMICOLON`, `COMMA`

---

## 3. Gramática implementada (`parser.y`)

O símbolo inicial é `programa` e, por enquanto, um programa é **uma única expressão**:

```bnf
<programa>  ::= <expressao>

<expressao> ::= <expressao> O_PLUS <expressao>
              | <expressao> O_MINUS <expressao>
              | <expressao> O_MULTI <expressao>
              | <expressao> O_DIV <expressao>
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
| 6 | esquerda | `O_MULTI` `O_DIV` |
| 7 | — (`%precedence`) | `DM_NOT` |
| 8 | — (`%precedence`) | `UMINUS` (menos unário) |

Exemplos: `1 + 2 * 3` agrupa como `1 + (2 * 3)`; `-2 * 3` agrupa como `(-2) * 3`; `!a && b` agrupa como `(!a) && b`.

### Comportamento atual

* O parser só **valida a sintaxe**: não há ações semânticas, então nada é calculado nem a AST é construída.
* `main` chama `yyparse()` e devolve o resultado como código de saída: `0` se a entrada é aceita e `1` se há erro sintático.
* Erros sintáticos são impressos em `stderr` como `Erro sintático: <mensagem>`, sem número de linha.
* Como `<programa>` é só uma expressão, entradas com `;`, atribuição (`=`), declaração de variável, `if`, `while`, `for` ou chaves resultam em erro sintático.
* Literais string (`L_STRING`) são emitidos pelo lexer mas não são aceitos pela gramática.
* Todos os tokens que não aparecem nas regras acima (tipos, palavras-chave de controle, `O_ASSIGN`, chaves, colchetes, `COLON`, `SEMICOLON`, `COMMA` e `L_STRING`) são declarados e produzidos pelo lexer, mas nenhuma regra os consome ainda.

---

## 4. Gramática alvo

> Esta seção descreve o que o parser deverá reconhecer e **ainda não está implementado em `parser.y`**. Em relação à versão anterior desta página, os nomes dos terminais foram alinhados aos que o lexer realmente produz (`L_INT`, `IDENT`, `O_PLUS`, `F_IF`, `DM_AND`, `R_EQ` etc.) e foram removidos `T_BOOL`, `TRUE_LIT` e `FALSE_LIT`, que não existem no lexer.

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
* **`<for-cond>`:** condição de continuidade; se omitida o laço não tem condição de parada (o avaliador atual ainda não possui `break`).
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

Cada regra da gramática alvo tem um construtor de nó pronto em `src/ast.c`:

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
| `LBRACE ... RBRACE` | `new_block_node` |

A tradução de token para código de operador (por exemplo `R_LE` para `OP_LEQ`) está em [AST e Avaliador](ast.md#operadores).

---

## 5. Restrições de escopo do projeto

Para manter o interpretador focado no aprendizado dos conceitos fundamentais de análise léxica, sintática e semântica, o escopo da linguagem restringe estritamente:

* Sem suporte a ponteiros (*pointers*).
* Sem suporte a estruturas (*structs*).
* Sem importação de bibliotecas externas ou uso de funções de cabeçalhos complexos do C padrão.

Também ficam fora da gramática alvo, por enquanto, `do-while`, `switch/case`, `break`, `continue`, `return`, funções, arrays e literais string, embora seus tokens já existam no lexer.
