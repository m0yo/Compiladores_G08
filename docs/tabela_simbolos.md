# Tabela de Símbolos

A tabela de símbolos guarda as variáveis declaradas pelo programa: o nome, o tipo e o valor atual de cada uma. O código está em `src/tabela_simbolos.h` (interface) e `src/tabela_simbolos.c` (implementação) e é usado pelo avaliador da [AST](ast.md).

## Estrutura

A tabela é uma **tabela hash** com `MAX_TAMANHO` = 211 posições (*buckets*). Colisões são tratadas por encadeamento: cada posição guarda uma lista ligada de entradas com o mesmo índice, e uma nova entrada é inserida no início da lista.

O índice é calculado com a função de hash `djb2` (valor inicial 5381, `hash * 33 + caractere`) módulo 211.

```c
typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_CHAR } SymType;

typedef union {
    int   intValue;
    float floatValue;
    char  charValue;
} SymValue;

typedef struct SymEntry {
    char *name;            // nome da variável
    SymType type;          // tipo da variável
    SymValue value;        // valor atual (o campo usado depende de type)
    struct SymEntry *next; // próxima entrada do mesmo bucket, ou NULL
} SymEntry;
```

Só existe **uma** tabela, global e estática dentro de `tabela_simbolos.c`. Por isso não há escopos aninhados: um bloco `{ ... }` não cria um novo escopo.

## Funções

| Função | O que faz |
|---|---|
| `SymEntry *buscar_simbolo(char *nome)` | Devolve a entrada da variável, ou `NULL` se ela não existe |
| `SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor)` | Cria a entrada (copiando o nome com `strdup`) e a devolve. Se o nome já existe, **só atualiza o valor** e mantém o tipo original |
| `const char *tipo_para_string(SymType tipo)` | Devolve `"int"`, `"float"`, `"char"` (ou `"?"`), usado nas mensagens do avaliador |
| `void sym_print(void)` | Imprime todas as entradas (nome, tipo e valor); serve para depuração |
| `void limpar_tabela(void)` | Libera todas as entradas e os nomes, deixando a tabela vazia |

Se a alocação de uma entrada falha, `inserir_simbolo` imprime uma mensagem em `stderr` e encerra o programa.

## Exemplo

```c
SymValue v;
v.intValue = 7;
inserir_simbolo("x", TYPE_INT, v);

SymEntry *e = buscar_simbolo("x");   // e->value.intValue == 7

v.intValue = 9;
inserir_simbolo("x", TYPE_INT, v);   // atualiza: x agora vale 9

limpar_tabela();                      // buscar_simbolo("x") passa a devolver NULL
```

## Limitações

* Só há os tipos `int`, `float` e `char`; `long` e `double` (que o lexer já reconhece) ainda não são suportados.
* Não há escopos: todas as variáveis são globais.
* Não há verificação de redeclaração: declarar de novo uma variável apenas atualiza o valor.

## Testes

`src/test_main.c` verifica inserir, buscar, buscar um nome inexistente, reinserir (atualização) e `limpar_tabela`. Veja [Como executar](como_executar.md).
