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

int inserir_simbolo(char *nome, int tipo, double valor) {
    // Verifica se já existe e atualiza o valor
    int idx = buscar_simbolo(nome);
    if (idx != -1) {
        tabela_valores[idx] = valor;
        tabela_tipos[idx] = tipo;
        return idx;
    }

    // Se não existe e há espaço, insere na próxima posição livre
    if (total_simbolos < MAX_SIMBOLOS) {
        strcpy(tabela_nomes[total_simbolos], nome);
        tabela_tipos[total_simbolos] = tipo;
        tabela_valores[total_simbolos] = valor;
        
        int pos_criada = total_simbolos;
        total_simbolos++;
        return pos_criada; // Retorna o índice do novo símbolo
    } else {
        fprintf(stderr, "Erro: Tabela de símbolos cheia!\n");
        return -1;
    }
}