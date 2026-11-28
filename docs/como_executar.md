# Como executar

Esta página explica como compilar o projeto, rodar o lexer e o parser, executar os testes e visualizar esta documentação.

## Pré-requisitos

* `gcc` e `make`
* [Flex](https://github.com/westes/flex)
* Bison 3.0 ou superior (o `%precedence` exige essa versão); confira com `bison --version`

## Compilar

Todos os arquivos gerados ficam em `build/`; os executáveis ficam na raiz do projeto. Nenhum deles é versionado.

| Comando | O que faz |
|---|---|
| `make` | Compila lexer e parser juntos e gera o executável `parser_exe` |
| `make test` | Roda todos os testes: unitários da AST, scanner, programas comparados com a saída esperada, programas que devem ser rejeitados e modo silencioso|
| `make test_lexer` | Roda só os testes do scanner (`tests/lexer/`) |
| `make lexer_exe` | Compila só o lexer, em modo isolado, e gera o executável `lexer_exe` |
| `make clean` | Remove `parser_exe`, `lexer_exe` e a pasta `build/` |

A ordem importa: o Bison roda primeiro e gera `build/parser.tab.h`, que o Flex inclui ao gerar `build/lex.yy.c`. O `Makefile` já cuida disso.

## Rodar o interpretador

O `parser_exe` lê um programa (de um arquivo ou da entrada padrão), monta a AST e a executa.

| Opção | O que faz |
|---|---|
| `-a` | Imprime a AST antes de executar |
| `-q` ou `--quiet` | Desabilita o rastro intermediário de execução |

```bash
./parser_exe tests/teste_parser.txt
./parser_exe -a tests/programas/controle.c
echo 'int x = 10; x = x * 2 + 1;' | ./parser_exe
```

Por padrão, a execução imprime o rastro do avaliador (`Declarado: x : int = 10`, `int x = 21`, `Resultado da expressão: ...`).

O modo silencioso (`-q` ou `--quiet`) suprime a impressão de declarações, atribuições e resultados intermediários de expressões, sendo útil para executar programas com laços longos. O valor do `return`, os erros e os avisos continuam sendo apresentados normalmente.

Sem `return`, erros, avisos ou solicitação da AST com `-a`, o modo silencioso não produz saída.

```bash
./parser_exe -q tests/quiet/laco_longo.c
./parser_exe --quiet < tests/quiet/laco_longo.c
./parser_exe -a -q tests/programas/controle.c
```

O programa `laco_longo.c` executa um milhão de iterações. No modo silencioso, sua única saída é:

```text
Programa retornou: 1000000
```

As opções `-a` e `-q` podem ser utilizadas juntas: a AST é exibida, mas o rastro intermediário da execução é omitido.

Código de saída: `1` se houver erro léxico, sintático, semântico ou de execução, o valor inteiro do `return` se o programa executar um (`Programa retornou: N`), e `0` nos demais casos. No Linux, o código de saída observado pelo terminal corresponde ao valor retornado módulo 256.

```bash
echo 'int x = ;' | ./parser_exe
echo "saída: $?"      # 1 (Erro sintático na linha 1: syntax error)
```

O modo silencioso não altera a execução nem o código de saída do programa.

O que a linguagem aceita está em [Parser](parser.md) e [Gramática](gramatica.md).

## Rodar o lexer isolado

```bash
make lexer_exe
./lexer_exe < tests/teste_lexer.txt
```

Para cada token é impresso o código, o lexema e a linha. Veja o formato e os detalhes em [Analisador Léxico](lexer.md).

## Testes

```bash
make test
```

O alvo roda cinco grupos de testes, que cobrem as fases e os modos de execução do interpretador:

1. **Testes unitários** (`src/test_main.c`, compilado em `build/test_ast`): tabela de símbolos (inclusive escopos), expressões, declarações, atribuições, `if/else`, `while`, `for`, `switch` com `default` e `char`, montando a AST à mão.
2. **Scanner** (`tests/lexer/*.txt`, alvo `make test_lexer`): cada arquivo passa pelo `lexer_exe` e a saída (lexema e linha de cada token, mais as mensagens de erro léxico) é comparada com o `.esperado`, na ordem em que aparecem no programa de entrada (o `lexer_exe` imprime o `stdout` linha a linha para garantir isso). O código numérico do token é ignorado, para que um token novo no parser não quebre os casos. Há entradas válidas (tokens, literais, comentários) e inválidas (caractere desconhecido, comentário de bloco não fechado).
3. **Programas comparados com a saída esperada** (`tests/programas/*` e `tests/semantica/*`, veja [Análise Semântica](semantica.md)): cada programa é executado pelo `parser_exe` e a saída (`stdout` e `stderr`) é comparada com o arquivo `.esperado` de mesmo nome. Os de `tests/semantica/` incluem programas com erros e avisos semânticos, e a comparação confere as mensagens.
4. **Testes de erro** (`tests/erros/*`): programas que devem ser rejeitados (erro léxico ou sintático, como strings, funções, structs, um comentário de bloco não fechado, `;` ou parêntese faltando, `else` sem `if`, `case` fora de `switch`). O teste falha se o `parser_exe` terminar com código de saída `0`; se houver `.esperado`, a saída também é comparada. Os `.esperado` dos erros sintáticos fixam a linha e a mensagem que o Bison reporta (por exemplo, a linha do fim do arquivo em `sintaxe_chave_aberta`), então devem ser revistos se a gramática mudar.
5. **Modo silencioso** (`tests/quiet/`): verifica as opções `-q` e `--quiet`, a execução de um laço de um milhão de iterações, a leitura pela entrada padrão, a compatibilidade com `-a` e a preservação dos erros léxicos, sintáticos, semânticos, de execução e dos avisos semânticos. Os demais grupos continuam utilizando o modo padrão, preservando a compatibilidade com os arquivos `.esperado` existentes.

Para adicionar um caso, crie o arquivo na pasta certa, **confira a saída** e só então salve-a:

```bash
./parser_exe tests/programas/nome.c > tests/programas/nome.esperado 2>&1     # programa (ou tests/erros/, tests/semantica/)
./lexer_exe < tests/lexer/nome.txt 2>&1 | sed 's/^Token: [0-9]* | //' > tests/lexer/nome.esperado   # scanner
```

Para rodar só os testes unitários e ver a saída completa: `make build/test_ast && ./build/test_ast`.

Para rodar só os testes do modo silencioso:

```bash
make
sh tests/quiet/test_quiet.sh ./parser_exe
```

Para conferir vazamento de memória, inclusive nos programas rejeitados:

```bash
    make test_vazamento
```

O alvo compila o parser com AddressSanitizer (`build/parser_asan`) e roda todos os programas de `tests/programas/`, `tests/semantica/` e `tests/erros/`. Ele falha se o sanitizador acusar vazamento ou acesso inválido. Fica fora do `make test` porque depende do suporte a `-fsanitize=address` do compilador (o `gcc` no Linux tem).

Cada verificação imprime `[OK ]` ou `[FALHOU]`. Ao final, o programa mostra `TODOS OS TESTES PASSARAM (0 falha(s))` e devolve código de saída `0`; se algum teste falhar, mostra `HOUVE FALHAS` e devolve `1`. A saída também inclui trechos impressos pelo avaliador (`Declarado: ...`, `int x = ...`) e a árvore do laço `for` desenhada por `print_ast`.

## Visualizar a documentação

O site é gerado pelo MkDocs com o tema Material (configurado em `mkdocs.yml`):

```bash
pip install mkdocs mkdocs-material
mkdocs serve
```

O servidor local costuma ficar em `http://127.0.0.1:8000`.

A publicação é automática: um push na `main` executa `.github/workflows/deploy.yml`, que roda `mkdocs gh-deploy` e publica o site na branch `gh-docs`, servida pelo GitHub Pages. Veja o fluxo completo em [Guia de Commits](contributing.md).
