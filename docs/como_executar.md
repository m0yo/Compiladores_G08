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
| `make test` | Roda todos os testes: unitários da AST, scanner, programas comparados com a saída esperada e programas que devem ser rejeitados |
| `make test_lexer` | Roda só os testes do scanner (`tests/lexer/`) |
| `make lexer_exe` | Compila só o lexer, em modo isolado, e gera o executável `lexer_exe` |
| `make clean` | Remove `parser_exe`, `lexer_exe` e a pasta `build/` |

A ordem importa: o Bison roda primeiro e gera `build/parser.tab.h`, que o Flex inclui ao gerar `build/lex.yy.c`. O `Makefile` já cuida disso.

## Rodar o interpretador

O `parser_exe` lê um programa (de um arquivo ou da entrada padrão), monta a AST e a executa. Com `-a`, imprime a árvore antes de executar:

```bash
./parser_exe tests/teste_parser.txt
./parser_exe -a tests/programas/controle.c    # mostra a AST e depois executa
echo 'int x = 10; x = x * 2 + 1;' | ./parser_exe
```

A execução imprime o que o avaliador faz (`Declarado: x : int = 10`, `int x = 21`, `Resultado da expressão: ...`). Código de saída: `1` se houver erro sintático ou de execução, o valor do `return` se o programa executar um (`Programa retornou: N`), e `0` nos demais casos:

```bash
echo 'int x = ;' | ./parser_exe
echo "saída: $?"      # 1 (Erro sintático na linha 1: syntax error)
```

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

O alvo roda quatro grupos de testes, que cobrem cada fase do interpretador:

1. **Testes unitários** (`src/test_main.c`, compilado em `build/test_ast`): tabela de símbolos (inclusive escopos), expressões, declarações, atribuições, `if/else`, `while`, `for`, `switch` com `default` e `char`, montando a AST à mão.
2. **Scanner** (`tests/lexer/*.txt`, alvo `make test_lexer`): cada arquivo passa pelo `lexer_exe` e a saída (lexema e linha de cada token, mais as mensagens de erro léxico) é comparada com o `.esperado`. O código numérico do token é ignorado, para que um token novo no parser não quebre os casos. Há entradas válidas (tokens, literais, comentários) e inválidas (caractere desconhecido, comentário de bloco não fechado).
3. **Programas comparados com a saída esperada** (`tests/programas/*` e `tests/semantica/*`, veja [Análise Semântica](semantica.md)): cada programa é executado pelo `parser_exe` e a saída (`stdout` e `stderr`) é comparada com o arquivo `.esperado` de mesmo nome. Os de `tests/semantica/` incluem programas com erros e avisos semânticos, e a comparação confere as mensagens.
4. **Testes de erro** (`tests/erros/*`): programas que devem ser rejeitados (erro léxico ou sintático, como strings, funções, structs, um comentário de bloco não fechado, `;` ou parêntese faltando, `else` sem `if`, `case` fora de `switch`). O teste falha se o `parser_exe` terminar com código de saída `0`; se houver `.esperado`, a saída também é comparada.

Para adicionar um caso, crie o arquivo na pasta certa, **confira a saída** e só então salve-a:

```bash
./parser_exe tests/programas/nome.c > tests/programas/nome.esperado 2>&1     # programa (ou tests/erros/, tests/semantica/)
./lexer_exe < tests/lexer/nome.txt 2>&1 | sed 's/^Token: [0-9]* | //' > tests/lexer/nome.esperado   # scanner
```

Para rodar só os testes unitários e ver a saída completa: `make build/test_ast && ./build/test_ast`.

Cada verificação imprime `[OK ]` ou `[FALHOU]`. Ao final, o programa mostra `TODOS OS TESTES PASSARAM (0 falha(s))` e devolve código de saída `0`; se algum teste falhar, mostra `HOUVE FALHAS` e devolve `1`. A saída também inclui trechos impressos pelo avaliador (`Declarado: ...`, `int x = ...`) e a árvore do laço `for` desenhada por `print_ast`.

## Visualizar a documentação

O site é gerado pelo MkDocs com o tema Material (configurado em `mkdocs.yml`):

```bash
pip install mkdocs mkdocs-material
mkdocs serve
```

O servidor local costuma ficar em `http://127.0.0.1:8000`.

A publicação é automática: um push na `main` executa `.github/workflows/deploy.yml`, que roda `mkdocs gh-deploy` e publica o site na branch `gh-docs`, servida pelo GitHub Pages. Veja o fluxo completo em [Guia de Commits](contributing.md).
