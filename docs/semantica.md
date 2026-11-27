# Análise Semântica

A análise semântica está em `src/semantica.c` e `src/semantica.h`. Ela percorre a [AST](ast.md) **uma vez, antes da execução**, e detecta erros que a gramática não consegue expressar. Se houver ao menos um erro, o programa **não é executado** e o `parser_exe` termina com código de saída 1.

```
código-fonte → lexer → parser → AST → análise semântica → avaliador (eval_ast)
                                            │
                                    erro? imprime e sai com 1
```

Sem essa fase, os mesmos problemas só apareciam durante a execução, depois de parte do programa já ter rodado.

---

## O que é verificado

| Verificação | Tipo | Mensagem |
|---|---|---|
| Variável usada ou atribuída sem declaração | erro | `variável 'x' não declarada` |
| Variável declarada duas vezes | erro | `variável 'x' já declarada` |
| `break` fora de laço e de `switch` | erro | `'break' fora de laço ou switch` |
| `continue` fora de laço (dentro de `switch` não basta) | erro | `'continue' fora de laço` |
| Valor repetido em `case` do mesmo `switch` | erro | `valor 'case N' repetido no switch` |
| Operador `%` com operando `float` | erro | `operador '%' exige operandos inteiros (int ou char)` |
| Divisão ou resto por zero com divisor literal (`a / 0`, `a % 0`) | aviso | `divisão por zero` |
| Variável usada sem nenhuma atribuição antes no texto | aviso | `variável 'x' pode ser usada sem valor` |
| `float` atribuído a `int` ou `char` | aviso | `'x' recebe um float e será convertido para int (perde a parte decimal)` |

Os **avisos** não impedem a execução e não mudam o código de saída. Todos os erros do programa são informados de uma vez, na ordem em que aparecem no código.

Formato das mensagens (em `stderr`):

```
Erro semântico na linha 3: variável 'b' não declarada
Aviso semântico na linha 2: 'i' recebe um float e será convertido para int (perde a parte decimal)
```

### Exemplo

```c
int a = 1;
int a = 2;
b = 3;
int c = a / 0;
continue;
```

```
Erro semântico na linha 2: variável 'a' já declarada
Erro semântico na linha 3: variável 'b' não declarada
Aviso semântico na linha 4: divisão por zero
Erro semântico na linha 5: 'continue' fora de laço
```

---

## Como funciona

* `analisar_semantica(programa)` devolve o número de erros. O `main` em `src/parser.y` a chama logo depois de montar a AST.
* A função mantém uma lista própria de variáveis declaradas (nome e tipo), **separada** da tabela de símbolos do avaliador, que só existe durante a execução.
* O **escopo é único (global)**, como no avaliador: uma variável declarada dentro de um bloco continua visível depois dele.
* Para cada expressão ela calcula o tipo (`int`, `float` ou `char`), seguindo a mesma promoção do avaliador: se algum operando é `float`, o resultado é `float`; comparações e operadores lógicos dão `int`.
* Dois contadores (`prof_laco` e `prof_switch`) dizem se o ponto atual está dentro de um laço ou de um `switch`, o que decide se `break` e `continue` são válidos.

### Número de linha

Cada nó da AST guarda o campo `linha`, copiado da linha atual do lexer no momento em que o nó é criado. O Bison às vezes lê o próximo token antes de reduzir uma regra, então para construções que terminam em outra linha o valor pode ser a linha seguinte. Por isso o `case` registra a sua linha à parte, e as mensagens de declaração, atribuição, `break`, `continue` e divisão usam a linha em que o comando termina.

---

## O que **não** é verificado

* Valores só conhecidos em execução: `a / b` com `b` igual a zero continua dando `Erro: Divisão por zero!` durante a execução. Com divisor literal (`a / 0`), a análise só avisa; se a divisão for executada, o avaliador também para com esse erro.
* O aviso de "sem valor" é simples: qualquer atribuição anterior no texto conta, mesmo dentro de um `if` ou laço que pode não executar.
* Uso de uma variável declarada em um ramo que não executa (a análise é estática e o escopo é único).
* `long` e `double` são tratados como `int` e `float` (a tabela de símbolos só tem esses tipos).
* Funções, strings, structs e ponteiros não existem na linguagem (veja [Gramática](gramatica.md)).

---

## Testes

Os casos estão em `tests/semantica/`. Cada `nome.c` tem um `nome.esperado` com a saída (`stdout` e `stderr`) esperada, e o `make test` os executa junto com `tests/programas/`.

| Arquivo | O que testa |
|---|---|
| `nao_declarada_atribuicao.c` | atribuição a variável não declarada |
| `nao_declarada_expressao.c` | variável não declarada dentro de uma expressão |
| `redeclaracao.c` | variável declarada duas vezes |
| `divisao_por_zero.c` | divisor literal igual a zero (só avisa; a divisão não é executada) |
| `aviso_sem_valor.c` | variável usada sem atribuição antes (só avisa) |
| `break_fora_de_laco.c` | `break` no nível do programa |
| `continue_fora_de_laco.c` | `continue` dentro de `switch` fora de laço |
| `case_repetido.c` | dois `case 1` no mesmo `switch` |
| `aviso_float_para_int.c` | avisos de conversão; o programa ainda executa |
| `varios_erros.c` | vários erros no mesmo programa; nada executa |
| `validos.c` | `continue` em laço, `break` em `switch`, `for` com declaração; sem erro nem aviso |

Para adicionar um caso, crie `nome.c`, confira a saída e salve-a com `./parser_exe tests/semantica/nome.c > tests/semantica/nome.esperado 2>&1`.

## Veja também

* [AST e Avaliador](ast.md): os nós que a análise percorre.
* [Tabela de Símbolos](tabela_simbolos.md): a tabela usada só na execução.
* [Como executar](como_executar.md): compilar e rodar os testes.
