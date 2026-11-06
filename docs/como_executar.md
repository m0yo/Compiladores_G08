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
| `make lexer_exe` | Compila só o lexer, em modo isolado, e gera o executável `lexer_exe` |
| `make clean` | Remove `parser_exe`, `lexer_exe` e a pasta `build/` |

A ordem importa: o Bison roda primeiro e gera `build/parser.tab.h`, que o Flex inclui ao gerar `build/lex.yy.c`. O `Makefile` já cuida disso.

## Rodar o parser

O `parser_exe` lê a entrada padrão e devolve como código de saída `0` se a sintaxe está correta ou `1` se há erro sintático:

```bash
echo '10 + 5' | ./parser_exe
echo "saída: $?"      # 0 (aceita)

echo '10 +' | ./parser_exe
echo "saída: $?"      # 1 (erro sintático)
```

Por enquanto o parser aceita apenas **uma expressão** e não imprime resultado nenhum: veja [Gramática](gramatica.md).

## Rodar o lexer isolado

```bash
make lexer_exe
./lexer_exe < tests/teste_lexer.txt
```

Para cada token é impresso o código, o lexema e a linha. Veja o formato e os detalhes em [Analisador Léxico](lexer.md).

## Testes da AST, do avaliador e da tabela de símbolos

O arquivo `src/test_main.c` testa a tabela de símbolos, as expressões, declarações, atribuições, `if/else`, `while`, `for` e `char`. Ainda não há um alvo no `Makefile` para ele; compile e rode manualmente:

```bash
mkdir -p build
gcc -Wall -o build/test_ast src/test_main.c src/ast.c src/tabela_simbolos.c -lm
./build/test_ast
```

Cada verificação imprime `[OK ]` ou `[FALHOU]`. Ao final, o programa mostra `TODOS OS TESTES PASSARAM (0 falha(s))` e devolve código de saída `0`; se algum teste falhar, mostra `HOUVE FALHAS` e devolve `1`. A saída também inclui trechos impressos pelo avaliador (`Declarado: ...`, `int x = ...`) e a árvore do laço `for` desenhada por `print_ast`.

> O parser ainda não é testado automaticamente: a pasta `tests/` só tem a entrada do lexer.

## Visualizar a documentação

O site é gerado pelo MkDocs com o tema Material (configurado em `mkdocs.yml`):

```bash
pip install mkdocs mkdocs-material
mkdocs serve
```

O servidor local costuma ficar em `http://127.0.0.1:8000`.

A publicação é automática: um push na `main` executa `.github/workflows/deploy.yml`, que roda `mkdocs gh-deploy` e publica o site na branch `gh-docs`, servida pelo GitHub Pages. Veja o fluxo completo em [Guia de Commits](contributing.md).
