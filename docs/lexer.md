# Analisador Léxico

Esta seção detalha a implementação do analisador léxico, localizado em `src/lexer.l`.

A ferramenta responsável pela geração do analisador léxico é o **Flex** (Fast Lexical Analyzer Generator). O objetivo do analisador léxico é ler o código-fonte de entrada caractere por caractere e agrupá-los em tokens (como palavras-chave, identificadores, números, operadores, etc.), que são consumidos pelo analisador sintático (Bison).

## Visão geral

- **Arquivo fonte:** `src/lexer.l`
- **Código gerado:** `build/lex.yy.c` (criado pelo `make`, não versionado)
- **Entrada:** código-fonte lido da entrada padrão (`stdin`)
- **Saída:** um código de token a cada chamada de `yylex()`, com o valor semântico (quando existe) em `yylval`
- **Definição dos tokens:** `src/parser.y` (`%token` e `%union`). O Bison os exporta em `build/parser.tab.h`, que o lexer inclui; por isso o `parser.y` é sempre processado antes do `lexer.l` (veja o `Makefile`)

## Funcionamento

- `%option noyywrap`: a análise termina no fim da entrada.
- `%x COMENTARIO`: estado exclusivo usado para ler comentários de bloco.
- `linha_erro`: variável global que conta as linhas lidas (começa em 1) e é usada nas mensagens de erro.
- **Maior lexema:** o Flex sempre escolhe o maior lexema possível; em caso de empate vale a regra que aparece primeiro no arquivo. Por isso `int` é `T_INT`, mas `integer` é `IDENT`, e `<=` é um único token (`R_LE`), não `<` seguido de `=`.
- **Modo isolado:** compilando com `-DLEXER_STANDALONE` (`make lexer_exe`), o lexer define o próprio `yylval` e um `main` que imprime cada token, o lexema e a linha.

## Tokens reconhecidos

| Categoria | Lexemas | Tokens |
|---|---|---|
| Tipos | `int` `float` `char` `long` `double` | `T_INT` `T_FLOAT` `T_CHAR` `T_LONG` `T_DOUBLE` |
| Controle de fluxo | `if` `else` `while` `for` `do` `switch` `case` `break` `continue` `return` | `F_IF` `F_ELSE` `F_WHILE` `F_FOR` `F_DO` `F_SWITCH` `F_CASE` `F_BREAK` `F_CONTINUE` `F_RETURN` |
| Aritméticos | `+` `-` `*` `/` | `O_PLUS` `O_MINUS` `O_MULTI` `O_DIV` |
| Atribuição | `=` | `O_ASSIGN` |
| Relacionais | `==` `!=` `<` `>` `<=` `>=` | `R_EQ` `R_NE` `R_LT` `R_GT` `R_LE` `R_GE` |
| Lógicos | `&&` `\|\|` `!` | `DM_AND` `DM_OR` `DM_NOT` |
| Delimitadores | `{` `}` `[` `]` `(` `)` `:` `;` | `LBRACE` `RBRACE` `LBRACKET` `RBRACKET` `LPAREN` `RPAREN` `COLON` `SEMICOLON` |
| Literais | inteiro, real, caractere | `L_INT` `L_FLOAT` `L_CHAR` |
| Identificadores | `[a-zA-Z_][a-zA-Z0-9_]*` | `IDENT` |

As expressões regulares de cada grupo estão em [Expressões Regulares](expressoesRegularesFlex.md).

### Valores semânticos (`yylval`)

| Token | Campo | Conteúdo |
|---|---|---|
| `L_INT` | `ival` (`int`) | convertido com `strtol` e cast para `int`, sem verificação de estouro |
| `L_FLOAT` | `dval` (`double`) | convertido com `strtod` |
| `L_CHAR` | `cval` (`char`) | caractere já com a sequência de escape traduzida |
| `IDENT` | `sval` (`char *`) | cópia alocada com `malloc` do lexema |

> Os nomes de `IDENT` em `sval` são alocados pelo lexer e **não são liberados** por ele: quem consome o token (o parser) é responsável por chamar `free`. Hoje o parser ainda não usa esses valores.

### Sequências de escape

Em literais de caractere, o lexer traduz `\n`, `\t`, `\r`, `\0`, `\\` e `\'` para o caractere correspondente. Qualquer outra sequência `\x` vira simplesmente `x`.

## Comentários

- `// ...` vai até o fim da linha e é descartado.
- `/* ... */` pode ocupar várias linhas e é lido no estado exclusivo `COMENTARIO`, que também conta as quebras de linha. Comentários de bloco **não** podem ser aninhados.
- Se o arquivo terminar com um comentário de bloco aberto, o lexer imprime `erro na linha N: comentario de bloco nao terminado` em `stderr` e encerra a análise.

Nenhum comentário gera token.

## Espaços e erros

- Espaços, tabulações e `\r` são ignorados; `\n` apenas incrementa `linha_erro`.
- Qualquer caractere que não case com nenhuma regra (por exemplo `@`, `#`, `%` ou um `&` ou `|` isolado) gera a mensagem `erro na linha N: 'c'` em `stderr`. O lexer devolve o próprio caractere como token, que não existe na gramática; por isso o parser rejeita a entrada com erro sintático logo em seguida (o programa não é executado).

## Limitações conhecidas

- Não existem tokens para `%`, `&` e `|` isolados, `++`, `--`, `+=` e demais operadores compostos, `.` e `->`.
- Literais numéricos: apenas decimal. Não há hexadecimal, octal, notação científica (`1e3`), sufixos (`10L`, `1.5f`) nem reais como `.5` ou `5.` (o `5.` vira `L_INT` seguido de erro no `.`).
- `short`, `signed`, `unsigned`, `void`, `bool`, `true` e `false` **não** são palavras reservadas: são reconhecidos como `IDENT`. O mesmo vale para `string`, `main` e `printf`. Strings, funções e structs não fazem parte da linguagem: aspas duplas, `,` e `.` geram erro léxico e o parser rejeita a entrada.
- `long` e `double` (`T_LONG`, `T_DOUBLE`) são aceitos pela gramática, mas ainda não têm tipo próprio: o parser os trata como `int` e `float`. `default` não é palavra-chave: chega como `IDENT`, então o `switch` só aceita `case`.

## Como testar

```bash
make lexer_exe
./lexer_exe < tests/teste_lexer.txt
```

O arquivo `tests/teste_lexer.txt` exercita literais, sequências de escape e os dois tipos de comentário. Ele serve apenas para o lexer isolado: o conteúdo não é aceito pelo parser atual.

Exemplo de saída para `int x = 10;`:

```
Token: 258 | Lexema: 'int' | Linha: 1
Token: 277 | Lexema: 'x' | Linha: 1
Token: 282 | Lexema: '=' | Linha: 1
Token: 273 | Lexema: '10' | Linha: 1
Token: 299 | Lexema: ';' | Linha: 1
```

Os códigos numéricos são definidos pelo Bison a partir da ordem dos `%token` em `src/parser.y`.
