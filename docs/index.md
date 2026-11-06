# Interpretador em C

Documentação técnica do projeto desenvolvido pela **Equipe 08** da disciplina de Compiladores 1 da Universidade de Brasília.

## Membros da Equipe 08

- Clarice Gitirana Gusson
- Pedro Vieira Antunes
- Maria Clara Canuto Gontijo
- Jefferson de Souza Reis de Oliveira
- Júlia Amanda Silva Lima

## Sobre o projeto

Este projeto implementa um interpretador, escrito em C, capaz de ler, analisar e executar comandos de um subconjunto da linguagem C, passando pelas etapas clássicas de:

1. **Lexer:** responsável pela análise léxica, transforma o código-fonte em uma sequência de tokens. Implementado com **Flex** (`.l`). Veja [Analisador Léxico](lexer.md).
2. **Parser:** responsável pela análise sintática, organiza os tokens segundo a gramática da linguagem. Implementado com **Bison** (`.y`). Veja [Gramática](gramatica.md).
3. **AST e tabela de símbolos:** estruturas que representam o programa (árvore sintática abstrata) e guardam as variáveis declaradas. Veja [AST e Avaliador](ast.md) e [Tabela de Símbolos](tabela_simbolos.md).
4. **Interpretação/Execução:** percorre a árvore e executa as instruções correspondentes (`eval_ast`).

### Estado atual

| Etapa | Situação | Código |
|---|---|---|
| Lexer | Funcional (palavras-chave, operadores, literais, identificadores, comentários) | `src/lexer.l` |
| Parser | Reconhece uma única expressão; faltam declarações, atribuições, comandos e ações semânticas | `src/parser.y` |
| AST | Implementada e testada isoladamente; ainda não é construída pelo parser | `src/ast.c`, `src/ast.h` |
| Tabela de símbolos | Implementada (`int`, `float`, `char`; escopo global) | `src/tabela_simbolos.c`, `src/tabela_simbolos.h` |
| Avaliador | Executa expressões, declarações, atribuições, `if`, `while`, `for` e blocos; ainda não é chamado pelo parser | `src/ast.c` |
| Análise semântica | Não iniciada (hoje os erros são detectados durante a execução) | — |

Para compilar e testar cada parte, veja [Como executar](como_executar.md).

### Ferramentas utilizadas

- **Flex:** gera o analisador léxico a partir das regras definidas em um arquivo `.l`.
- **Bison:** gera o analisador sintático (parser) a partir da gramática definida em um arquivo `.y`.
- **C (gcc) e Make:** linguagem usada para as ações semânticas, a AST, o avaliador e o restante do interpretador, e automação do build.
- **MkDocs (tema Material):** gera este site a partir dos arquivos em `docs/`, publicado pelo GitHub Actions.
