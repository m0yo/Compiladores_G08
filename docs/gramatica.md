## 1. Notação

| Símbolo | Significado |
|---|---|
| `::=` | "é definido como" |
| `\|` | alternativa |
| `[ X ]` | opcional |
| `{ X }` | repete 0 ou mais vezes |
| `TOKEN` | terminal, com o nome exato usado em `parser.y` |
| `<nome>` | não-terminal |

Terminais com valor semântico (`%union`): `NUM` (int), `FLOAT_LIT` (double),
`CHAR_LIT` (char), `ID` (string), `TRUE_LIT`/`FALSE_LIT` (int 1/0).

Terminais sem valor semântico: `PLUS MINUS TIMES DIVIDE LPAREN RPAREN
ASSIGN SEMICOLON T_INT T_FLOAT T_CHAR T_BOOL KW_IF KW_ELSE KW_WHILE KW_FOR
LBRACE RBRACE AND OR NOT EQ NE LT GT LE GE`.

---

## 2. Programa

```bnf
<programa>      ::= <lista-linhas>

<lista-linhas>  ::= /* vazio */
                  | <lista-linhas> <linha>

<linha>          ::= <comando>
```

---

## 3. Tipos

```bnf
<tipo-especificador>  ::= T_INT | T_FLOAT | T_CHAR | T_BOOL
```

---

## 4. Comandos

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

### 4.2 Bloco

```bnf
<lista-comandos> ::= /* vazio */
                   | <lista-comandos> <comando>
```

---

## 5. Expressões


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




