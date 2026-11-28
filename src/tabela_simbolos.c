#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela_simbolos.h"

/*tabela hash em que cada indice é o endereço do primeio elemento (bucket)
de uma lista encadeada de entradas que tem o mesmo indice de hash*/
static SymEntry *tabela[MAX_TAMANHO];

//nivel de escopo atual (0 = mais externo)
static int nivel_atual = 0;


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
    SymEntry *entry = tabela[idx]; //inicio da lista encadeada no bucket
    while(entry) {
        if(strcmp(entry->name, nome) == 0) {
            return entry; //encontrou, regtorna ponteiro para a entrada
        }
        entry = entry->next; //proximo nó

    }
    return NULL; //não encontrou
}

/*
como a insercao e no inicio do bucket e as variaveis de um escopo interno
sao inseridas depois das de fora, buscar_simbolo ja encontra a mais interna
*/
SymEntry *buscar_simbolo_no_escopo_atual(char *nome){
    SymEntry *entry = buscar_simbolo(nome);
    return (entry && entry->nivel == nivel_atual) ? entry : NULL;
}

void entrar_escopo(void){
    nivel_atual++;
}

//remove de cada bucket as entradas declaradas no escopo que esta sendo fechado
void sair_escopo(void){
    for (int i = 0; i < MAX_TAMANHO; i++) {
        SymEntry **p = &tabela[i];
        while (*p) {
            if ((*p)->nivel == nivel_atual) {
                SymEntry *temp = *p;
                *p = temp->next;
                free(temp->name);
                free(temp);
            } else {
                p = &(*p)->next;
            }
        }
    }
    if (nivel_atual > 0) nivel_atual--;
}

int escopo_atual(void){
    return nivel_atual;
}

SymEntry *inserir_simbolo(char *nome, SymType tipo, SymValue valor) {
    // Se ja existe no escopo atual, so atualiza o valor
    SymEntry *entrada = buscar_simbolo_no_escopo_atual(nome);
    if(entrada){
        entrada->value = valor; //atualiza o valor existente
        return entrada;
    }


    //cria uma nova entrada
    unsigned int idx = hash(nome);
    entrada = (SymEntry *)malloc(sizeof(SymEntry));
    if(!entrada){
        fprintf(stderr, "Erro: Falha na alocação de memória para a entrada '%s' da tabela de símbolos!\n", nome);
        exit(EXIT_FAILURE);
    }
    entrada->name = strdup(nome);
    entrada->type = tipo;
    entrada->value = valor;
    entrada->nivel = nivel_atual;
    entrada->next = tabela[idx]; //insere no inicio da lista encadeada do bucket
    tabela[idx] = entrada; //atualiza o ponteiro do bucket para a nova
    return entrada; //retorna ponteiro para a nova entrada
}

const char *tipo_para_string(SymType tipo) {
    switch (tipo) {
        case TYPE_INT:   return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR:  return "char";
        default:         return "?";
    }
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
    nivel_atual = 0;
}