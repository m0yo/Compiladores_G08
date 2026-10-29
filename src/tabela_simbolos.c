#include <stdio.h>
#include <string.h>
#include "tabela_simbolos.h"

/*tabela hash em que cada indice é o endereço do primeio elemento (bucket)
de uma lista encadeada de entradas que tem o mesmo indice de hash*/
static SymEntry *tabela[MAX_TAMANHO];


//calcula o indice de hash para uma string
static unsigned int hash(const char *nome) {
    unsigned int hash = 5381;
    while (*nome) {
        hash = ((hash<<5) + hash) + (unsigned char)(*nome++); 
    }
    return hash % MAX_TAMANHO;
}

SymEntry *buscar_simbolo(char *nome){
    unsigned int idx = hash(nome); //determina o bucket da hash table
    SymEntry *entry = table[idx]; //inicio da lista encadeada no bucket
    while(e) {
        if(strcmp(entry->name, nome) == 0) {
            return entry; //encontrou, regtorna ponteiro para a entrada
        }
        entry = entry->next; //proximo nó

    }
    return NULL; //não encontrou
}

SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor) {
    // Verifica se já existe e atualiza o valor caso exista
    SymEntry *entrada = buscar_simbolo(nome);
    if(entrada){
        entrada->value = valor; //atualiza o valor existente
        return entrada;
    }


    //cria uma nova entrada
    unsignet int idx = hash(nome);
    entrada = (SymEntry *)malloc(sizeof(SymEntry));
    if(!entrada){
        fprintf(stderr, "Erro: Falha na alocação de memória para a entrada '%s' da tabela de símbolos!\n", nome);
        exit(EXIT_FAILURE);
    }
    entrada->name = strdup(nome);
    entrada->type = tipo;
    entrada->value = valor;
    entrada->next = tabela[idx]; //insere no inicio da lista encadeada do bucket
    tabela[idx] = entrada; //atualiza o ponteiro do bucket para a nova
    return entrada; //retorna ponteiro para a nova entrada
}

//função que imprime as entradas da tabela, feita pelo chat pra debuggar
void sym_print(void) {
    printf("Tabela de Símbolos:\n");
    for (int i = 0; i < MAX_TAMANHO; i++) {
        SymEntry *entry = tabela[i];
        while (entry) {
            printf("Nome: %s, Tipo: %s, Valor: ", entry->name, tipo_para_string(entry->type));
            switch (entry->type) {
                case TYPE_INT:
                    printf("%d", entry->value.intValue);
                    break;
                case TYPE_FLOAT:
                    printf("%f", entry->value.floatValue);
                    break;
                case TYPE_CHAR:
                    printf("%c", entry->value.charValue);
                    break;
            }
            printf("\n");
            entry = entry->next;
        }
    }
}

//libera a memoria alocada para a tabela de simbolos
void limpar_tabela(void) {
    for (int i = 0; i < MAX_TAMANHO; i++) {
        SymEntry *entry = tabela[i];
        while (entry) {
            SymEntry *temp = entry;
            entry = entry->next;
            free(temp->name);
            free(temp);
        }
        tabela[i] = NULL;
    }
}
