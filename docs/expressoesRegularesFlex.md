# Guia - Expressões Regulares no Flex

Este documento serve como referência oficial para a especificação, escrita e revisão das expressões regulares (ERs) utilizadas no analisador léxico (`lexer.l`) do projeto do interpretador da linguagem C.

O objetivo deste guia é padronizar a sintaxe utilizada por todos os membros do grupo, evitando divergências na captura de tokens.

---

## 1. Identificadores

Identificadores são utilizados para nomear variáveis, funções e outros identificadores definidos pelo usuário na linguagem C.

* **Expressão Regular:** `[a-zA-Z_][a-zA-Z0-9_]*`
* **Token associado:** `IDENT`
* **Descrição:** Deve iniciar obrigatoriamente com uma letra maiúscula, minúscula ou um sublinhado (`_`), seguido por zero ou mais caracteres alfanuméricos ou sublinhados.

### Exemplos:
| Lexema de Entrada | Token Esperado |
| :--- | :--- |
| `main` | `IDENT` |
| `_contador` | `IDENT` |
| `soma2` | `IDENT` |

---

## 2. Literais Numéricos (Inteiros e Ponto Flutuante)

Abrange as constantes numéricas inteiras e reais (floats), contemplando múltiplos dígitos antes e depois do ponto decimal.

### 2.1. Números Inteiros
* **Expressão Regular:** `[0-9]+`
* **Token associado:** `L_INT`
* **Descrição:** Sequência de um ou mais dígitos decimais.

### 2.2. Números Reais (Float)
* **Expressão Regular:** `[0-9]+\.[0-9]+` (ou variações tratadas no scanner)
* **Token associado:** `L_FLOAT`
* **Descrição:** Sequência de dígitos antes do ponto, o caractere ponto (`\.`) e um ou mais dígitos após o ponto, permitindo múltiplos dígitos decimais na parte fracionária.

> Pendência: o lexer.l ainda não implementa essa regra corretamente (usa `[0-9]+\.[0-9]`, aceitando apenas um dígito após o ponto). Ajustar o scanner para bater com as especificações

### Exemplos:
| Lexema de Entrada | Token Esperado |
| :--- | :--- |
| `42` | `L_INT` |
| `0` | `L_INT` |
| `3.14159` | `L_FLOAT` |
| `0.007` | `L_FLOAT` |

---

## 3. Literais de Caractere e Strings

Tratam os valores textuais e caracteres individuais, incluindo o suporte a sequências de escape padrão da linguagem C (como `\n` e `\t`).

### 3.1. Literais de Caractere
* **Expressão Regular:** `'(\\.|[^'\\])'`
* **Token associado:** `L_CHAR`
* **Descrição:** Delimitado por aspas simples, aceita um caractere comum (que não seja aspa simples ou barra invertida) ou uma sequência de escape precedida por barra invertida (`\`).

### 3.2. Literais de String
* **Expressão Regular:** `\"(\\.|[^"\\])*\"`
* **Token associado:** `L_STRING`
* **Descrição:** Delimitado por aspas duplas, aceita zero ou mais caracteres internos normais ou sequências de escape.

### Exemplos:
| Lexema de Entrada | Token Esperado |
| :--- | :--- |
| `'a'` | `L_CHAR` |
| `'\n'` | `L_CHAR` |
| `"Olá, Mundo!"` | `L_STRING` |
| `"Erro: %d\n"` | `L_STRING` |

---

## 4. Tipos e palavras-chave

Palavras reservadas da linguagem, reconhecidas literalmente pelo scanner.

* **Tipos:** `int` (`T_INT`), `float` (`T_FLOAT`), `char` (`T_CHAR`), `long` (`T_LONG`), `double` (`T_DOUBLE`)
* **Controle de fluxo:** `if` (`F_IF`), `else` (`F_ELSE`), `while` (`F_WHILE`), `for` (`F_FOR`), `do` (`F_DO`), `switch` (`F_SWITCH`), `case` (`F_CASE`), `break` (`F_BREAK`), `continue` (`F_CONTINUE`), `return` (`F_RETURN`)

### Exemplos:
| Lexema de Entrada | Token Esperado |
| :--- | :--- |
| `int` | `T_INT` |
| `if` | `F_IF` |
| `while` | `F_WHILE` |

---

## 5. Operadores e Delimitadores

Os operadores e delimitadores cobrem os símbolos aritméticos, relacionais, lógicos, de atribuição e pontuações estruturais da linguagem C.

* **Aritméticos:** `+` (`O_PLUS`), `-` (`O_MINUS`), `*` (`O_MULTI`), `/` (`O_DIV`)
* **Atribuição:** `=` (`O_ASSIGN`)
* **Relacionais:** `==` (`R_EQ`), `!=` (`R_NE`), `<` (`R_LT`), `>` (`R_GT`), `<=` (`R_LE`), `>=` (`R_GE`)
* **Lógicos:** `&&` (`DM_AND`), `||` (`DM_OR`), `!` (`DM_NOT`)
* **Delimitadores:** `{` (`LBRACE`), `}` (`RBRACE`), `[` (`LBRACKET`), `]` (`RBRACKET`), `(` (`LPAREN`), `)` (`RPAREN`), `:` (`COLON`), `;` (`SEMICOLON`), `,` (`COMMA`)

---

## 6. Espaços em Branco, Quebras de Linha e Tratamento de Erros

Para evitar que o parser receba caracteres indesejados, os espaços e quebras de linha são consumidos silenciosamente pelo analisador léxico.

* **Expressões Regulares:**
  * `[ \t\r]+`: Ignora espaços horizontais, tabulações e retornos de carro.
  * `\n`: Incrementa a variável de controle de linha (`linha_erro++`), essencial para a emissão correta de mensagens de erro detalhadas.
  * `.`: Captura qualquer caractere inválido não reconhecido pelas regras anteriores, disparando uma mensagem de erro na saída padrão de erro (`stderr`) formatada com a linha correspondente.

---

## 7. Comentários (Linha Única e Bloco)

> Pendente de implementação no `lexer.l` atual. O scanner hoje só tem um comentário para lembrete no código, a regra em si ainda não foi escrita.

> **Nota de Implementação:** O suporte a comentários no analisador léxico deve ignorar trechos de documentação e anotações do programador sem retornar tokens para o parser.

### 7.1. Comentário de Linha Única
* **Expressão Regular:** `\/\/.*`
* **Descrição:** Identifica comentários iniciados por `//` e é válido para toda a linha restante.
* **Exemplo:** `// Inicialização do contador` (Ignorado pelo scanner).

### 7.2. Comentário de Bloco
* **Expressão Regular:** `\/\*[^*]*\*+([^/*][^*]*\*+)*\/`
* **Descrição:** Identifica blocos de comentários iniciados por `/*` e terminados por `*/`, permitindo múltiplas linhas.
* **Exemplo:**
  ```c
  /* Bloco de código 
    com múltiplas linhas 
  */
  ```