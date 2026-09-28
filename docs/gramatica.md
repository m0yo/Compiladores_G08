# Guia da Gramática Formal (BNF)

Este documento tem como objetivo fornecer uma referência clara e detalhada em prosa da sintaxe suportada pelo interpretador/compilador, alinhando a documentação com as regras implementadas no analisador sintático (`parser.y`).

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

### Tipos de terminais
* **Terminais com valor semântico (`%union`):** `NUM` (int), `FLOAT_LIT` (double), `CHAR_LIT` (char), `ID` (string), `TRUE_LIT`/`FALSE_LIT` (int 1/0).

* **Terminais sem valor semântico (palavras-chave e operadores):** `PLUS`, `MINUS`, `TIMES`, `DIVIDE`, `LPAREN`, `RPAREN`, `ASSIGN`, `SEMICOLON`, `T_INT`, `T_FLOAT`, `T_CHAR`, `T_BOOL`, `KW_IF`, `KW_ELSE`, `KW_WHILE`, `KW_FOR`, `LBRACE`, `RBRACE`, `AND`, `OR`, `NOT`, `EQ`, `NE`, `LT`, `GT`, `LE`, `GE`.

---

## 2. Programa

Um programa na linguagem é composto estruturalmente por uma lista de linhas ou instruções sequenciais.

```bnf
<programa>      ::= <lista-linhas>

<lista-linhas>  ::= /* vazio */
                  | <lista-linhas> <linha>

<linha>          ::= <comando>
```

---

## 3. Tipos

Os tipos de dados primitivos suportados pela linguagem para a declaração de variáveis são especificados pelo conjunto de especificadores de tipo:

```bnf
<tipo-especificador>  ::= T_INT | T_FLOAT | T_CHAR | T_BOOL
```

---

## 4. Comandos

Esta seção define os comandos suportados pela gramática, que estruturam as ações executadas pelo programa. Cada comando é formalmente especificado abaixo:

```bnf
<comando> ::= <expressao> SEMICOLON

           | ID ASSIGN <expressao> SEMICOLON

           | <tipo-especificador> ID SEMICOLON

           | <tipo-especificador> ID ASSIGN <expressao> SEMICOLON

           | KW_IF LPAREN <expressao> RPAREN <comando>

           | KW_IF LPAREN <expressao> RPAREN <comando> KW_ELSE <comando>

           | KW_WHILE LPAREN <expressao> RPAREN <comando>

           | KW_FOR LPAREN <for-init> SEMICOLON <for-cond> SEMICOLON <for-step> RPAREN <comando>

           | LBRACE <lista-comandos> RBRACE
```

### Explicação das regras de `<comando>`:

* **Expressão avaliada:** `<expressao> SEMICOLON` permite que uma expressão isolada seja executada como um comando, encerrada por ponto e vírgula.
* **Atribuição:** `ID ASSIGN <expressao> SEMICOLON` atribui o resultado de uma expressão a um identificador pré-existente.
* **Declaração simples:** `<tipo-especificador> ID SEMICOLON` declara uma nova variável informando seu tipo e identificador.
* **Declaração com inicialização:** `<tipo-especificador> ID ASSIGN <expressao> SEMICOLON` declara a variável e já atribui um valor inicial a ela.
* **Condicional simples (`if`):** `KW_IF LPAREN <expressao> RPAREN <comando>` executa um comando condicionalmente se a expressão entre parênteses for verdadeira.
* **Condicional com alternativa (`if-else`):** Permite ramificar a execução escolhendo entre dois comandos dependendo da veracidade da expressão avaliada.
* **Laço de repetição (`while`):** `KW_WHILE LPAREN <expressao> RPAREN <comando>` repete a execução de um comando enquanto a condição for verdadeira.
* **Laço de repetição (`for`):** Executa um loop controlado estruturado por inicialização, condição e passo.
* **Bloco de comandos:** `LBRACE <lista-comandos> RBRACE` agrupa múltiplos comandos entre chaves, delimitando um escopo.

### 4.1 Componentes do `for`

```bnf
<for-init> ::= /* vazio */
             | <tipo-especificador> ID ASSIGN <expressao>
             | ID ASSIGN <expressao>

<for-cond> ::= /* vazio */
             | <expressao>

<for-step> ::= /* vazio */
             | ID ASSIGN <expressao>
```

### Explicação das regras do `for`:

* **`<for-init>`:** Define a inicialização do loop, que pode ser vazia, uma declaração de variável com atribuição ou uma simples atribuição a uma variável existente.
* **`<for-cond>`:** Define a condição de continuidade do loop, podendo ser omitida (criando um laço infinito sintático) ou baseada em uma expressão booleana.
* **`<for-step`:** Define o passo ou incremento executado a cada iteração, podendo ser vazio ou uma nova atribuição a oidentificador de controle.

### 4.2 Bloco

```bnf
<lista-comandos> ::= /* vazio */
                   | <lista-comandos> <comando>
```

### Explicação da regra de bloco:

* **`<lista-comandos>`:** Representa uma sequência de comandos que podem aparecer de forma vazia (sem instruções) ou de maneira recursiva (`<lista-comandos> <comando>`), permitindo encadear múltiplos comandos em sequência dentro de um bloco.

---

## 5. Expressões

As expressões englobam operações aritméticas, lógicas, relacionais, chamadas de literais e uso de identificadores, respeitando a precedência de operadores:
* **Aritméticas:** Adição (`PLUS`), Subtração (`MINUS`), Multiplicação (`TIMES`), Divisão (`DIVIDE`) e inversão unária (`MINUS` com `%prec UMINUS`).
* **Lógicas:** Conexão lógica E (`AND`), OU (`OR`) e Negação (`NOT`).
* **Relacionais:** Igualdade (`EQ`), Diferença (`NE`), Menor que (`LT`), Maior que (`GT`), Menor ou igual (`LE`), Maior ou igual (`GE`).
* **Agrupamento e Terminais:** Expressões entre parênteses (`LPAREN`/`RPAREN`), além dos literais de número, ponto flutuante, caractere, booleanos e identificadores.

```bnf
<expressao> ::= <expressao> PLUS <expressao>
              | <expressao> MINUS <expressao>
              | <expressao> TIMES <expressao>
              | <expressao> DIVIDE <expressao>
              | MINUS <expressao>                    (* %prec UMINUS *)
              | <expressao> AND <expressao>
              | <expressao> OR <expressao>
              | NOT <expressao>
              | <expressao> EQ <expressao>
              | <expressao> NE <expressao>
              | <expressao> LT <expressao>
              | <expressao> GT <expressao>
              | <expressao> LE <expressao>
              | <expressao> GE <expressao>
              | LPAREN <expressao> RPAREN
              | NUM
              | FLOAT_LIT
              | CHAR_LIT
              | TRUE_LIT
              | FALSE_LIT
              | ID
```
---

## 6. Restrições de Escopo do Projeto

Para manter o interpretador focado no aprendizado dos conceitos fundamentais de análise léxica, sintática e semântica, o escopo da linguagem restringe estritamente:

* Sem suporte a ponteiros (*pointers*).
* Sem suporte a estruturas (*structs*).
* Sem importação de bibliotecas externas ou uso de funções de cabeçalhos complexos do C padrão.
