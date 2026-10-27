#include <stdio.h>
#include <string.h>
#include "tabela_simbolos.h"

// Definição oficial dos arrays paralelos
char tabela_nomes[MAX_SIMBOLOS][MAX_TAMANHO_NOME];
int tabela_tipos[MAX_SIMBOLOS];
double tabela_valores[MAX_SIMBOLOS];
int total_simbolos = 0;

void inicializar_tabela() {
    total_simbolos = 0;
}

int buscar_simbolo(char *nome) {
    for (int i = 0; i < total_simbolos; i++) {
        if (strcmp(tabela_nomes[i], nome) == 0) {
            return i; // Retorna o índice do símbolo encontrado
        }
    }
    return -1;
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