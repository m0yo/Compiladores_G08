# Compiladores_G08

> ⚠️ **Projeto em desenvolvimento.** O lexer está funcional e integrado ao parser, que por enquanto reconhece apenas **uma expressão** (sem comandos). A AST, a tabela de símbolos e o avaliador já existem em `src/`, mas ainda **não estão conectados ao parser**: a execução de programas completos está por vir. Este README serve como guia de orientação para quem for contribuir ou acompanhar o projeto.

Documentação publicada: <https://m0yo.github.io/Compiladores_G08/>

## Sobre o projeto

Este projeto é um **interpretador para a linguagem C, implementado na própria linguagem C**. O objetivo é ler código-fonte C e executá-lo diretamente, passando pelas etapas clássicas de um compilador/interpretador: análise léxica, análise sintática, análise semântica e execução.

É um projeto acadêmico (disciplina de Compiladores), servindo tanto como exercício prático de construção de linguagens quanto como ferramenta de estudo sobre como um código C é processado internamente.

## Estado atual do projeto

- ✅ **Lexer (análise léxica)** — implementado em Flex (`src/lexer.l`), reconhece palavras-chave, operadores, literais (`int`, `float`, `char` e string), identificadores, delimitadores e comentários (`//` e `/* */`), retornando os tokens e os valores semânticos (`yylval`) e contando as linhas para mensagens de erro
- 🚧 **Parser (análise sintática, com Bison)** — `src/parser.y` está integrado ao lexer e reconhece **uma única expressão** (aritmética, relacional, lógica, parênteses, literais e identificadores); faltam declarações, atribuições e comandos, e ainda não há ações semânticas
- 🚧 **Árvore Sintática Abstrata (AST)** — `src/ast.c` e `src/ast.h`: nós, construtores, impressão (`print_ast`) e liberação (`free_ast`) implementados e testados; ainda não é construída pelo parser
- 🚧 **Tabela de símbolos** — `src/tabela_simbolos.c` e `.h`: tabela hash com os tipos `int`, `float` e `char`, escopo único (global)
- 🚧 **Avaliador (`eval_ast`)** — executa expressões, declarações, atribuições, `if`, `while`, `for` e blocos sobre uma AST; validado por `src/test_main.c`, mas ainda não é chamado a partir do parser
- ⬜ Ações semânticas no parser (construir a AST a partir do código-fonte)
- ⬜ Análise semântica separada da execução (verificação de tipos e escopos; hoje os erros são detectados em tempo de execução)
- ⬜ Suporte a `long`/`double`, `do-while`, `switch`, `break`/`continue`/`return` e funções (os tokens já existem no lexer, mas nada os consome ainda)
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
   [ Parser ] → valida a sintaxe e monta a AST (Bison) -> reconhece expressões; ainda não monta a AST
      │
      ▼
[ AST + Tabela de símbolos ] → estruturas do programa -> implementadas (src/ast.*, src/tabela_simbolos.*)
      │
      ▼
[ Análise semântica ] → valida tipos, escopos, declarações -> não iniciada
      │
      ▼
 [ Interpretador ] → percorre a AST e executa o programa (eval_ast) -> pronto para o subconjunto atual, sem ligação com o parser
```

O lexer e o parser compartilham os mesmos tokens e tipos de valor semântico (`yylval`): eles são declarados em `src/parser.y` (`%token` e `%union`) e o Bison os exporta no `parser.tab.h`, que o `lexer.l` inclui.

## Estrutura do projeto

```
.
├── .github/                 # templates de issue/PR e workflow de deploy da documentação
├── docs/                    # documentação em Markdown (gerada como site via MkDocs)
├── site/                    # site estático gerado pelo MkDocs (não editar manualmente)
├── src/
│   ├── lexer.l              # regras do analisador léxico (Flex)
│   ├── parser.y             # gramática do analisador sintático (Bison)
│   ├── ast.h / ast.c        # AST, avaliador (eval_ast), print_ast e free_ast
│   ├── tabela_simbolos.h/.c # tabela de símbolos (hash table)
│   └── test_main.c          # testes da AST, do avaliador e da tabela de símbolos
├── tests/                   # arquivos de entrada para testar o lexer
├── Makefile                 # compila lexer + parser (arquivos gerados ficam em build/)
├── mkdocs.yml               # configuração do site de documentação
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
echo "saída: $?"

# compila só o lexer, imprimindo os tokens reconhecidos
make lexer_exe
./lexer_exe < tests/teste_lexer.txt

# testes da AST, do avaliador e da tabela de símbolos (ainda não há alvo no Makefile)
mkdir -p build
gcc -Wall -o build/test_ast src/test_main.c src/ast.c src/tabela_simbolos.c -lm
./build/test_ast

# remove os arquivos gerados
make clean
```

A saída do lexer isolado mostra o código do token, o lexema (texto exato) e a linha onde apareceu. Exemplo com `int x = 10;`:

```
Token: 258 | Lexema: 'int' | Linha: 1
Token: 277 | Lexema: 'x' | Linha: 1
Token: 282 | Lexema: '=' | Linha: 1
Token: 273 | Lexema: '10' | Linha: 1
Token: 299 | Lexema: ';' | Linha: 1
```

Os códigos numéricos são atribuídos pelo Bison conforme a ordem dos `%token` em `src/parser.y`, então mudam quando tokens são adicionados ou removidos.

O teste da AST termina com `TODOS OS TESTES PASSARAM (0 falha(s))` e código de saída 0 quando tudo está correto.

## Documentação

A documentação detalhada do projeto (lexer, expressões regulares, gramática, AST e tabela de símbolos) está em `docs/` e pode ser visualizada localmente com o MkDocs. O tema Material é necessário, pois é o configurado em `mkdocs.yml`:

```bash
pip install mkdocs mkdocs-material
mkdocs serve
```

Isso sobe um servidor local (geralmente em `http://127.0.0.1:8000`) com o site de documentação navegável. A publicação é automática: um push na `main` dispara o workflow `.github/workflows/deploy.yml`, que gera o site e o publica na branch `gh-docs`.

## Roadmap

- [x] Implementar o lexer com Flex
- [x] Integrar o lexer ao Bison (tokens e `yylval` compartilhados)
- [x] Implementar a tabela de símbolos
- [x] Definir a AST e o avaliador `eval_ast` (expressões, declaração, atribuição, `if`, `while`, `for`)
- [ ] Implementar o parser (declarações e comandos)
- [ ] Conectar o parser à AST (ações semânticas que chamam os construtores `new_*_node`)
- [ ] Executar programas completos (parser → AST → `eval_ast`)
- [ ] Implementar análise semântica (tipos, escopos)
- [ ] Suporte a estruturas de controle (`if`, `while` e `for` já são avaliados na AST; faltam o parser, `do-while` e `switch`)
- [ ] Suporte a funções
- [ ] Suporte a tipos compostos (structs, arrays, ponteiros)
- [ ] Testes automatizados cobrindo cada etapa do pipeline (hoje existem testes da AST e da tabela de símbolos em `src/test_main.c`; faltam parser e pipeline completo)

## Como contribuir

1. Atualize a `develop` e crie sua branch a partir dela (ex: `feature/parser-comandos`)
2. Descreva no commit/PR qual parte do pipeline (lexer, parser, etc.) está sendo alterada
3. Sempre que possível, adicione um caso de teste cobrindo a mudança
4. Abra um Pull Request para a `develop` explicando o que foi feito e o porquê

Veja também `docs/contributing.md` para as convenções do projeto (branches, commits, issues e PRs).

## Licença

Este projeto está sob a licença descrita no arquivo [LICENSE](./LICENSE).
