/*ANALISE SEMANTICA: verifica a AST antes de executar*/

#ifndef SEMANTICA_H
#define SEMANTICA_H

#include "ast.h"

/*
 Percorre a AST uma vez, sem executar nada.
 Erros e avisos saem em stderr ("Erro semântico ..." / "Aviso ...").
 Devolve o numero de erros; com 0 erros o programa pode ser executado.
 Os avisos nao contam como erro.
*/
int analisar_semantica(const ASTNode *programa);

#endif
