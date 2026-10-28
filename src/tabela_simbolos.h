#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#define MAX_TAMANHO = 211

/*A tabela é responvsavel por armazenar pares de nome e valor inteiro para cada variavel declarada
utilizando uma hash table de tamanho fixo*/


//tipos suportados pelo interpretador
typedef enum{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR
} SymType;

//valor armazenado na tabela
typedef union{
    int intValue;
    float floatValue;
    char charValue;
} SymValue;

/*
strutura de uma entrada na tabela de simbolos,
cada entrada pertence a uma lista encadeada dentro de um bucket da hash table*/
typedef struct SymEntry {
    char *name; //nome da variavel
    SymType type; //tipo da variavel
    SymValue value; //valor da variavel
    struct SymEntry *next; //ponteiro para o proximo elemento no mesmo bucket ou NULL
} SymEntry;

// Funções de manipulação
SymEntry *buscar_simbolo(char *nome); // Retorna o ponteiro para a entrada ou NULL se não achar

SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor);

const char *tipo_para_string(SymType tipo); //retorna o tipo da variavel

void sym_print(void); //imprime a tabela de simbolos

void sym_limpar_tabela(void); //limpa a tabela de simbolos

#endif