# Compiladores_G08

> ⚠️ **Projeto em estágio inicial.** O lexer já está funcional; parser e interpretador ainda estão por vir. Este README serve como guia de orientação para quem for contribuir ou acompanhar o projeto.

## Sobre o projeto

Este projeto é um **interpretador para a linguagem C, implementado na própria linguagem C**. O objetivo é ler código-fonte C e executá-lo diretamente, passando pelas etapas clássicas de um compilador/interpretador: análise léxica, análise sintática, análise semântica e execução.

É um projeto acadêmico (disciplina de Compiladores), servindo tanto como exercício prático de construção de linguagens quanto como ferramenta de estudo sobre como um código C é processado internamente.

## Estado atual do projeto

- **Lexer (análise léxica)** — implementado em Flex (`src/lexer.l`), reconhece palavras-chave, operadores, literais, identificadores e delimitadores de C, retornando os tokens correspondentes
- ⬜ Parser (análise sintática, com Bison) — construção da Árvore Sintática Abstrata (AST)
- ⬜ Análise semântica
- ⬜ Motor de execução (interpretação da AST)
- ⬜ Suporte completo a tipos, ponteiros, structs, etc.

> Atualize este checklist conforme o projeto avança — é a forma mais rápida de qualquer pessoa entender "onde estamos" sem ler código.

## Como o interpretador vai funcionar (visão geral)

```
código-fonte (.c)
      │
      ▼
   [ Lexer ]  → quebra o texto em tokens (Flex) -> pronto
      │
      ▼
   [ Parser ] → organiza os tokens em uma AST (Bison) -> próximo passo
      │
      ▼
[ Análise semântica ] → valida tipos, escopos, declarações ->
      │
      ▼
 [ Interpretador ] → percorre a AST e executa o programa ->
```

O lexer atual já define um `enum` com o código de cada tipo de token (tipos, palavras-chave de controle de fluxo, operadores, literais, delimitadores) em `src/lexer.l`. O próximo passo natural é integrá-lo ao Bison, descomentando o include do `parser.tab.h` já deixado como comentário no arquivo.

## Estrutura do projeto

```
.
├── docs/            # documentação em Markdown (gerada como site via MkDocs)
│   ├── index.md
│   ├── lexer.md
│   └── contributing.md
├── site/            # site estático gerado pelo MkDocs (não editar manualmente)
├── src/
│   └── lexer.l      # regras do analisador léxico (Flex)
├── mkdocs.yml       # configuração do site de documentação
├── LICENSE
└── README.md
```

## Como compilar e rodar o lexer

Pré-requisito: ter o [Flex](https://github.com/westes/flex) instalado (`flex --version` para conferir).

```bash
cd src

# gera o código C a partir das regras do lexer
flex lexer.l

# compila o código gerado
gcc lex.yy.c -o teste

# roda o lexer sobre um arquivo de exemplo
./teste < exemplo.c
```

A saída mostra, para cada token reconhecido: o código numérico do token, o lexema (texto exato) e a linha onde apareceu. Exemplo com `int x = 10;`:

```
Token: 256 | Lexema: 'int' | Linha: 1
Token: 275 | Lexema: 'x'   | Linha: 1
Token: 280 | Lexema: '='   | Linha: 1
Token: 271 | Lexema: '10'  | Linha: 1
Token: 297 | Lexema: ';'   | Linha: 1
```

## Documentação

A documentação detalhada do projeto (incluindo a especificação do lexer) está em `docs/` e pode ser visualizada localmente com o MkDocs:

```bash
pip install mkdocs
mkdocs serve
```

Isso sobe um servidor local (geralmente em `http://127.0.0.1:8000`) com o site de documentação navegável.

## Roadmap

- [x] Implementar o lexer com Flex
- [ ] Integrar o lexer ao Bison e implementar o parser
- [ ] Construir a AST (Árvore Sintática Abstrata)
- [ ] Implementar análise semântica (tipos, escopos)
- [ ] Implementar interpretação de expressões simples (aritmética, atribuição)
- [ ] Suporte a estruturas de controle (`if`, `while`, `for`, `switch`)
- [ ] Suporte a funções
- [ ] Suporte a tipos compostos (structs, arrays, ponteiros)
- [ ] Testes automatizados cobrindo cada etapa do pipeline

## Como contribuir

1. Crie uma branch a partir da `main` (ex: `feature/parser-bison`)
2. Descreva no commit/PR qual parte do pipeline (lexer, parser, etc.) está sendo alterada
3. Sempre que possível, adicione um caso de teste (`.c` de exemplo) cobrindo a mudança
4. Abra um Pull Request explicando o que foi feito e o porquê

Veja também `docs/contributing.md` para convenções específicas do projeto.

## Licença

Este projeto está sob a licença descrita no arquivo [LICENSE](./LICENSE).