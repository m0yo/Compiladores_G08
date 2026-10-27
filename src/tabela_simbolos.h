#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#define MAX_SIMBOLOS 100
#define MAX_TAMANHO_NOME 50

#define TIPO_INT 1
#define TIPO_FLOAT 2
#define TIPO_CHAR 3

// Arrays paralelos
extern char tabela_nomes[MAX_SIMBOLOS][MAX_TAMANHO_NOME];
extern int tabela_tipos[MAX_SIMBOLOS];
extern double tabela_valores[MAX_SIMBOLOS];
extern int total_simbolos;

// Funções de manipulação
void inicializar_tabela();
int inserir_simbolo(char *nome, int tipo, double valor);
int buscar_simbolo(char *nome); // Retorna o índice da variável ou -1 se não achar


#endif