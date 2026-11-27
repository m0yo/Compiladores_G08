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
    int nivel;             // nível de escopo da declaração (0 = mais externo)
    struct SymEntry *next; // próxima entrada do mesmo bucket, ou NULL
} SymEntry;
```

Só existe **uma** tabela, estática dentro de `tabela_simbolos.c`, mas cada entrada guarda o **nível de escopo** em que foi declarada (veja [Escopos](#escopos)).

## Funções

| Função | O que faz |
|---|---|
| `SymEntry *buscar_simbolo(char *nome)` | Devolve a entrada da variável (a declaração mais interna visível), ou `NULL` se ela não existe |
| `SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor)` | Cria a entrada no escopo atual (copiando o nome com `strdup`) e a devolve. Se o nome já existe **no escopo atual**, só atualiza o valor; se existe apenas num escopo externo, cria uma entrada nova que o sombreia |
| `void entrar_escopo(void)` | Abre um escopo (aumenta o nível atual) |
| `void sair_escopo(void)` | Remove as variáveis do escopo atual e volta ao nível anterior |
| `int escopo_atual(void)` | Devolve o nível atual |
| `SymEntry *buscar_simbolo_no_escopo_atual(char *nome)` | Como `buscar_simbolo`, mas só considera o escopo atual |
| `const char *tipo_para_string(SymType tipo)` | Devolve `"int"`, `"float"`, `"char"` (ou `"?"`), usado nas mensagens do avaliador |
| `void sym_print(void)` | Imprime todas as entradas (nome, tipo e valor); serve para depuração |
| `void limpar_tabela(void)` | Libera todas as entradas e os nomes, deixando a tabela vazia e o nível em 0 |

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

## Escopos

O avaliador abre um escopo em cada bloco `{ ... }`, em cada `for` (para a variável declarada no `init`) e no corpo de cada `switch`. O próprio programa é um bloco, então as variáveis do nível mais externo ficam no nível 1.

* Como a inserção é no início do bucket e as variáveis de um escopo interno são inseridas depois das de fora, `buscar_simbolo` encontra sempre a mais interna: ela **sombreia** a de fora.
* `sair_escopo` percorre os buckets e remove as entradas do nível que está sendo fechado; a variável de fora volta a ser visível.

```c
int a = 1;
{ int a = 2; a = a + 10; int b = 5; }   // não altera o a externo; b deixa de existir
for (int i = 0; i < 3; i = i + 1) { }   // este i só existe dentro do for
int i = 100;                            // não conflita com o i do for
```

## Limitações

* Só há os tipos `int`, `float` e `char`; `long` e `double` (que o lexer já reconhece) ainda não são suportados.
* A tabela não verifica redeclaração: declarar de novo uma variável no mesmo escopo apenas atualiza o valor. Quem rejeita a redeclaração é a [análise semântica](semantica.md), antes da execução.

## Testes

`src/test_main.c` verifica inserir, buscar, buscar um nome inexistente, reinserir (atualização), `limpar_tabela` e os escopos (sombreamento, remoção ao sair e o nível atual). `tests/programas/escopos.c` testa os escopos no pipeline completo. Veja [Como executar](como_executar.md).
