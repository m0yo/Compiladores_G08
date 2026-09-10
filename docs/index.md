# Interpretador em C

Documentação técnica do projeto desenvolvido pela **Equipe 08** da disciplina de Compiladores 1 da Universidade de Brasília.

## Membros da Equipe 08

- Clarice Gitirana Gusson
- Pedro Vieira Antunes
- Maria Clara Canuto Gontijo
- Jefferson de Souza Reis de Oliveira
- Júlia Amanda Silva Lima

## Sobre o projeto

Este projeto implementa um interpretador capaz de ler, analisar e executar comandos de uma linguagem simples, passando pelas etapas clássicas de:

1. **Lexer:** Responsável pela análise léxica, transforma o código-fonte em uma sequência de tokens. Implementado com **Lex** (`.l`).
2. **Parser:** Responsável pela análise sintática, organiza os tokens em uma estrutura (como uma árvore sintática). Implementado com **Bison** (`.y`).
3. **Interpretação/Execução:** Pecorre essa estrutura e executa as instruções correspondentes.

### Ferramentas utilizadas

- **Lex/Flex:** Gera o analisador léxico a partir das regras definidas em um arquivo `.l`.
- **Bison:** Gera o analisador sintático (parser) a partir da gramática definida em um arquivo `.y`.
- **C:** Linguagem usada para as ações semânticas e o restante do interpretador.