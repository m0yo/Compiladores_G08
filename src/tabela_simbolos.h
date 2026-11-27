#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#define MAX_TAMANHO  211

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
    int nivel; //nivel de escopo em que foi declarada (0 = mais externo)
    struct SymEntry *next; //ponteiro para o proximo elemento no mesmo bucket ou NULL
} SymEntry;

// Funções de manipulação
SymEntry *buscar_simbolo(char *nome); // Retorna o ponteiro para a entrada ou NULL se não achar

/*
 Insere no escopo atual. Se o nome ja existe NO ESCOPO ATUAL, so atualiza o valor;
 se existe apenas num escopo externo, cria uma entrada nova que o sombreia.
*/
SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor);

/*
 Escopos aninhados: cada bloco { }, cada for e cada corpo de switch abre um escopo.
 buscar_simbolo encontra sempre a declaracao mais interna visivel.
*/
void entrar_escopo(void);
void sair_escopo(void);   //remove as variaveis do escopo atual
int escopo_atual(void);
SymEntry *buscar_simbolo_no_escopo_atual(char *nome);

const char *tipo_para_string(SymType tipo); //retorna o tipo da variavel

void sym_print(void); //imprime a tabela de simbolos

void limpar_tabela(void); //limpa a tabela de simbolos e volta ao escopo 0

#endif