# Compiladores_G08

> ⚠️ **Projeto em estágio inicial.** O lexer está funcional e já está integrado ao parser (que por enquanto reconhece apenas expressões); a gramática de comandos, a análise semântica e o interpretador ainda estão por vir. Este README serve como guia de orientação para quem for contribuir ou acompanhar o projeto.

## Sobre o projeto

Este projeto é um **interpretador para a linguagem C, implementado na própria linguagem C**. O objetivo é ler código-fonte C e executá-lo diretamente, passando pelas etapas clássicas de um compilador/interpretador: análise léxica, análise sintática, análise semântica e execução.

É um projeto acadêmico (disciplina de Compiladores), servindo tanto como exercício prático de construção de linguagens quanto como ferramenta de estudo sobre como um código C é processado internamente.

## Estado atual do projeto

- ✅ **Lexer (análise léxica)** — implementado em Flex (`src/lexer.l`), reconhece palavras-chave, operadores, literais, identificadores, delimitadores e comentários de C, retornando os tokens e os valores semânticos (`yylval`)
- 🚧 **Parser (análise sintática, com Bison)** — `src/parser.y` já está integrado ao lexer e reconhece expressões; faltam declarações e comandos
- ⬜ Construção da Árvore Sintática Abstrata (AST)
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
   [ Parser ] → organiza os tokens em uma AST (Bison) -> em andamento
      │
      ▼
[ Análise semântica ] → valida tipos, escopos, declarações ->
      │
      ▼
 [ Interpretador ] → percorre a AST e executa o programa ->
```

O lexer e o parser compartilham os mesmos tokens e tipos de valor semântico (`yylval`): eles são declarados em `src/parser.y` (`%token` e `%union`) e o Bison os exporta no `parser.tab.h`, que o `lexer.l` inclui.

## Estrutura do projeto

```
.
├── docs/            # documentação em Markdown (gerada como site via MkDocs)
├── site/            # site estático gerado pelo MkDocs (não editar manualmente)
├── src/
│   ├── lexer.l      # regras do analisador léxico (Flex)
│   └── parser.y     # gramática do analisador sintático (Bison)
├── tests/           # arquivos de entrada para testar o lexer
├── Makefile         # compila lexer + parser (arquivos gerados ficam em build/)
├── mkdocs.yml       # configuração do site de documentação
├── LICENSE
└── README.md
```

## Como compilar e rodar

Pré-requisitos: `gcc`, `make`, [Flex](https://github.com/westes/flex) e Bison 3.0 ou superior (o `%precedence` exige essa versão; confira com `bison --version`).

```bash
# compila lexer + parser juntos (gera build/ e o executável parser_exe)
make

# roda o parser sobre uma expressão (código de saída 0 = aceita, 1 = erro sintático)
echo '10 + 5' | ./parser_exe

# compila só o lexer, imprimindo os tokens reconhecidos
make lexer_exe
./lexer_exe < tests/teste_lexer.txt

# remove os arquivos gerados
make clean
```

A saída do lexer isolado mostra o código do token, o lexema (texto exato) e a linha onde apareceu. Exemplo com `int x = 10;`:

```
Token: 258 | Lexema: 'int' | Linha: 1
Token: 277 | Lexema: 'x'   | Linha: 1
Token: 282 | Lexema: '='   | Linha: 1
Token: 273 | Lexema: '10'  | Linha: 1
Token: 299 | Lexema: ';'   | Linha: 1
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
- [x] Integrar o lexer ao Bison (tokens e `yylval` compartilhados)
- [ ] Implementar o parser (declarações e comandos)
- [ ] Construir a AST (Árvore Sintática Abstrata)
- [ ] Implementar análise semântica (tipos, escopos)
- [ ] Implementar interpretação de expressões simples (aritmética, atribuição)
- [ ] Suporte a estruturas de controle (`if`, `while`, `for`, `switch`)
- [ ] Suporte a funções
- [ ] Suporte a tipos compostos (structs, arrays, ponteiros)
- [ ] Testes automatizados cobrindo cada etapa do pipeline

## Como contribuir

1. Atualize a `develop` e crie sua branch a partir dela (ex: `feature/parser-comandos`)
2. Descreva no commit/PR qual parte do pipeline (lexer, parser, etc.) está sendo alterada
3. Sempre que possível, adicione um caso de teste cobrindo a mudança
4. Abra um Pull Request para a `develop` explicando o que foi feito e o porquê

Veja também `docs/contributing.md` para as convenções do projeto (branches, commits, issues e PRs).

## Licença

Este projeto está sob a licença descrita no arquivo [LICENSE](./LICENSE).