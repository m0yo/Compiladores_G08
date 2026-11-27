# Compiladores_G08

> **Estado:** o pipeline completo (lexer → parser → AST → análise semântica → avaliador) está conectado e testado. O interpretador executa programas com declarações (`int`, `float`, `char`), atribuições, expressões (inclusive `%`, `&&` e `||` com curto-circuito), escopos de bloco e os comandos `if`, `while`, `do-while`, `for`, `switch` (com `default`), `break`, `continue` e `return`. Antes de executar, ele faz uma análise semântica e recusa programas com erro. Funções, strings e structs estão **fora do escopo** de propósito (veja [Limitações conhecidas](#limitações-conhecidas)). Este README serve como guia de orientação para quem for contribuir ou acompanhar o projeto.

Documentação publicada: <https://m0yo.github.io/Compiladores_G08/>

## Sobre o projeto

Este projeto é um **interpretador para a linguagem C, implementado na própria linguagem C**. O objetivo é ler código-fonte C e executá-lo diretamente, passando pelas etapas clássicas de um compilador/interpretador: análise léxica, análise sintática, análise semântica e execução.

É um projeto acadêmico (disciplina de Compiladores), servindo tanto como exercício prático de construção de linguagens quanto como ferramenta de estudo sobre como um código C é processado internamente.

## Estado atual do projeto

- ✅ **Lexer (análise léxica)** — implementado em Flex (`src/lexer.l`), reconhece palavras-chave, operadores, literais (`int`, `float` e `char`), identificadores, delimitadores e comentários (`//` e `/* */`), retornando os tokens e os valores semânticos (`yylval`) e contando as linhas para mensagens de erro; comentário de bloco não fechado e caractere desconhecido são erros léxicos e o programa não é executado
- ✅ **Parser (análise sintática, com Bison)** — `src/parser.y` reconhece declarações, atribuições, expressões e os comandos `if`/`else`, `while`, `do-while`, `for`, `switch`/`case`, `break`, `continue` e `return`, e constrói a AST com ações semânticas
- ✅ **Árvore Sintática Abstrata (AST)** — `src/ast.c` e `src/ast.h`: nós para todos os comandos do parser, construtores, impressão (`print_ast`) e liberação (`free_ast`)
- ✅ **Tabela de símbolos** — `src/tabela_simbolos.c` e `.h`: tabela hash com os tipos `int`, `float` e `char`, escopos aninhados (bloco, `for` e `switch`)
- ✅ **Avaliador (`eval_ast`)** — executa todos os comandos acima, chamado pelo `main` do parser; `&&` e `||` têm curto-circuito, `%` exige operandos inteiros e `long`/`double` são tratados como `int`/`float`
- ✅ **Análise semântica** — `src/semantica.c` e `.h`: percorre a AST antes de executar e rejeita o programa com `Erro semântico na linha N: ...` (variável não declarada ou redeclarada, `break`/`continue` fora de laço, `case`/`default` repetido, `%` com `float`); conversão de `float`, variável sem valor e divisão por zero literal geram só um aviso (veja `docs/semantica.md`)
- ✅ Testes automatizados com `make test`, cobrindo cada fase: unitários da AST, scanner (`tests/lexer/`), programas de ponta a ponta (`tests/programas/`), análise semântica (`tests/semantica/`) e programas que devem ser rejeitados (`tests/erros/`)
- ⬜ Saída de dados na linguagem (`printf`) e `int main() { ... }`
- ⬜ Verificação de tipos mais completa (hoje só há o aviso de conversão `float` → `int`/`char` e o erro de `%` com `float`)
- ⬜ Tipos próprios para `long`/`double`
- ⬜ Arrays e ponteiros (funções, strings e structs estão fora do escopo; veja `tests/erros/`)
- ⬜ `case` com constante negativa e `+` unário (`case -1:`), `++`, `+=` e afins

> Atualize este checklist conforme o projeto avança — é a forma mais rápida de qualquer pessoa entender "onde estamos" sem ler código.

## Como o interpretador vai funcionar (visão geral)

```
código-fonte (.c)
      │
      ▼
   [ Lexer ]  → quebra o texto em tokens (Flex) -> pronto
      │
      ▼
   [ Parser ] → valida a sintaxe e monta a AST (Bison) -> pronto para o subconjunto atual
      │
      ▼
[ AST + Tabela de símbolos ] → estruturas do programa -> implementadas (src/ast.*, src/tabela_simbolos.*)
      │
      ▼
[ Análise semântica ] → declarações e escopos, break/continue, case/default repetido, % com float (+ avisos) -> pronta (src/semantica.*)
      │
      ▼
 [ Interpretador ] → percorre a AST e executa o programa (eval_ast) -> pronto para o subconjunto atual
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
│   ├── tabela_simbolos.h/.c # tabela de símbolos (hash table com escopos)
│   ├── semantica.h/.c       # análise semântica (roda antes do avaliador)
│   └── test_main.c          # testes da AST, do avaliador e da tabela de símbolos
├── tests/
│   ├── teste_lexer.txt      # entrada para testar o lexer
│   ├── teste_parser.txt     # programa com todos os comandos do parser
│   ├── lexer/               # casos do scanner (válidos e inválidos) + .esperado
│   ├── programas/           # programas .c + saída .esperado (parser e execução)
│   ├── semantica/           # programas com erros e avisos semânticos + .esperado
│   └── erros/               # programas que devem ser rejeitados (+ .esperado opcional)
├── Makefile                 # compila lexer + parser (arquivos gerados ficam em build/) e roda os testes
├── mkdocs.yml               # configuração do site de documentação
├── LICENSE
└── README.md
```

## Como compilar e rodar

Pré-requisitos: `gcc`, `make`, [Flex](https://github.com/westes/flex) e Bison 3.0 ou superior (o `%precedence` exige essa versão; confira com `bison --version`).

```bash
# compila lexer + parser juntos (gera build/ e o executável parser_exe)
make

# executa um programa (use -a para ver a AST antes da execução)
./parser_exe tests/teste_parser.txt
echo 'int x = 10; x = x * 2 + 1;' | ./parser_exe

# compila só o lexer, imprimindo os tokens reconhecidos
make lexer_exe
./lexer_exe < tests/teste_lexer.txt

# todos os testes: unitários da AST, scanner, programas e programas rejeitados
make test

# só os testes do scanner
make test_lexer

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

O `make test` imprime `[OK ]` para cada teste e termina com código de saída diferente de 0 se algum falhar. Veja a descrição de cada grupo em `docs/como_executar.md`.

### Saída e código de saída

A linguagem não tem `printf`. O que o programa mostra é o rastro do avaliador (`Declarado: x : int = 10`, `int x = 21`, `Resultado da expressão: ...`). `return expr;` encerra o programa e vira o código de saída do `parser_exe`, como o `main` de um programa C (`Programa retornou: N`). Erros léxicos, sintáticos e semânticos terminam com código `1` e **nada é executado**; avisos (`Aviso semântico na linha N: ...`) não impedem a execução.

## Limitações conhecidas

- **Fora do escopo, por decisão do grupo:** strings, funções e structs são rejeitados com erro (testados em `tests/erros/`). Também não há arrays nem ponteiros.
- **Sem `printf`:** a saída é o rastro do avaliador (veja acima).
- **Tipos:** só `int`, `float` e `char`. `long` e `double` são tratados como `int` e `float`, e o estouro de `int` é silencioso.
- **Operadores:** não há `++`, `+=` e afins, e `case -1:` e `+x` ainda não são aceitos.
- **Erros sintáticos:** o parser para no primeiro erro e informa só a linha (`syntax error`).

## Documentação

A documentação detalhada do projeto (lexer, expressões regulares, gramática, parser, AST, tabela de símbolos e análise semântica) está em `docs/` e pode ser visualizada localmente com o MkDocs. O tema Material é necessário, pois é o configurado em `mkdocs.yml`:

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
- [x] Implementar o parser (declarações e comandos)
- [x] Conectar o parser à AST (ações semânticas que chamam os construtores `new_*_node`)
- [x] Executar programas completos (parser → AST → `eval_ast`)
- [x] Implementar análise semântica (declarações, `break`/`continue`, `case`/`default` repetido, `%` com `float`, avisos)
- [x] Escopos de bloco
- [x] Curto-circuito em `&&` e `||` e operador `%`
- [x] Suporte a estruturas de controle (`if`, `while`, `do-while`, `for`, `switch`, `break`, `continue`, `return` e `default`)
- [x] Erro léxico para comentário de bloco não fechado
- [x] Testes automatizados cobrindo cada etapa do pipeline (scanner, parser, semântica e execução)
- [ ] Verificação de tipos mais completa
- [ ] Decidir a saída de dados da linguagem (`printf` ou comando de impressão)
- [ ] Suporte a arrays e ponteiros (funções, strings e structs estão fora do escopo)

## Como contribuir

1. Atualize a `develop` e crie sua branch a partir dela (ex: `feature/parser-comandos`)
2. Descreva no commit/PR qual parte do pipeline (lexer, parser, etc.) está sendo alterada
3. Sempre que possível, adicione um caso de teste cobrindo a mudança
4. Abra um Pull Request para a `develop` explicando o que foi feito e o porquê

Veja também `docs/contributing.md` para as convenções do projeto (branches, commits, issues e PRs).

## Licença

Este projeto está sob a licença descrita no arquivo [LICENSE](./LICENSE).
