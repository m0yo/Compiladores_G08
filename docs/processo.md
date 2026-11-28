# Processo de desenvolvimento

Esta página registra como a Equipe 08 organizou o trabalho: as sprints, as decisões técnicas e o motivo de cada uma, e os problemas encontrados com a solução adotada. Ela atende o item "Repositório no GitHub" do documento [Trabalho de Compiladores](trabalho_compiladores.md), que pede **decisões técnicas**, **planejamento das sprints** e **problemas encontrados e soluções adotadas**.

---

## Como a equipe trabalhou

* **Método:** Scrum com sprints semanais, como recomenda a proposta da disciplina. A revisão de cada sprint acontecia na aula prática de quarta-feira, onde se mostrava o que tinha sido concluído e se combinava o que entraria na sprint seguinte.
* **Tarefas:** cada trabalho relevante virou uma issue no GitHub, aberta com os modelos de `.github/ISSUE_TEMPLATE/` (`feature`, `bug`, `documentation`, `task`).
* **Branches:** `main` (versão estável), `develop` (integração) e branches de trabalho `feature/*`, `fix/*`, `docs/*` e `test/*` criadas a partir da `develop`. Nada entra na `main` nem na `develop` sem Pull Request revisado. As regras completas estão no [Guia de Commits](contributing.md).
* **Commits:** padrão `<emoji> <tipo>: <descrição>` em português.
* **Documentação:** escrita em `docs/` junto com o código e publicada pelo GitHub Actions (MkDocs) na branch `gh-docs` a cada push na `main`.
* **Pontos de controle:** P1 em 28/09/2026 e P2 em 09/11/2026. Entrevista final da equipe em 30/11/2026.

---

## Sprints

As sprints vão de terça a segunda, de modo que cada uma termina antes da revisão de quarta. A numeração segue a semana do semestre, a mesma usada na apresentação da P2. As sprints 1 a 13 foram reconstruídas a partir do histórico do git (commits e Pull Requests mesclados), porque a equipe não manteve um quadro separado antes da P2. Para as sprints 14 a 16, a tabela mostra o planejamento apresentado na P2 ao lado do que de fato entrou.

### Sprints 1 a 13: até a P2

| Sprint | Período | Foco | O que entrou | Quem |
|---|---|---|---|---|
| 1 e 2 | 11/08 a 24/08 | Ambiente | Repositório criado; estrutura inicial com Flex e Bison; `.gitignore` | Maria Clara |
| 3 | 25/08 a 31/08 | Organização e primeiros tokens | Pasta `src/` e `docs/`; tokens aritméticos no `lexer.l` (PRs #1 e #2) | Pedro |
| 4 | 01/09 a 07/09 | Lexer e documentação | Lexer independente do Bison com mais operadores e literais; MkDocs configurado e publicado pelo GitHub Actions; regras de commit e documentação (PRs #4 e #5) | Pedro, Maria Clara |
| 5 | 08/09 a 14/09 | Build | `Makefile` rodando o lexer avulso; README; página com a proposta da disciplina; correção do diretório dos modelos de issue (PRs #18, #19 e #20) | Pedro, Jefferson, Maria Clara |
| 6 | 15/09 a 21/09 | Especificação da linguagem | Especificação inicial do subconjunto de C e gramática em BNF (PR #23); guia de expressões regulares do Flex | Pedro, Clarice |
| 7 | 22/09 a 28/09 | Preparação da P1 | Gramática formal e guia de expressões regulares no site (PRs #28 a #31); parser inicial (PR #32); integração `develop` → `main` (PR #33). **P1 em 28/09** | Clarice, Júlia, Maria Clara |
| 8 | 29/09 a 05/10 | Lexer completo | Comentários de linha e de bloco; valores semânticos dos literais em `yylval`; correções no `parser.y` (`%start`, ordem de tokens) e no `.gitignore` (PR #41) | Maria Clara, Clarice |
| 9 | 06/10 a 12/10 | Teste do scanner | Correção das regras do lexer e teste do scanner (PR #43) | Maria Clara |
| 10 | 13/10 a 19/10 | Tokens do parser | Correção da declaração e da ordem dos tokens; remoção do `enum` de teste do lexer | Pedro |
| 11 | 20/10 a 26/10 | Parser | Tokens `{`, `}`, `;` e `%union` para os literais; raiz da gramática definida (PR #46) | Pedro |
| 12 | 27/10 a 02/11 | Tabela de símbolos, AST e avaliador | Tabela de símbolos ligada ao parser (PR #47); lexer e parser integrados com tokens e `%union` compartilhados (PR #48); migração da tabela para hash, tipos de nó, construtores e avaliador recursivo `eval_ast`, desenvolvidos nesta sprint e mesclados no PR #52 | Clarice, Pedro, Jefferson |
| 13 | 03/11 a 09/11 | Preparação da P2 | Arquivos gerados fora do git (`build/`); AST, tabela hash e avaliador mesclados; documentação atualizada; `develop` → `main` (PRs #51, #52, #55 e #56). **P2 em 09/11** | Maria Clara, Pedro |

### Sprints 14 a 16: da P2 à entrega

| Sprint | Período | Planejado na P2 | O que entrou | Quem |
|---|---|---|---|---|
| 14 | 10/11 a 18/11 | Parser completo (declarações, atribuições, `if`, `while`, `for`, blocos); ações semânticas que criam a AST | Parser com declarações, expressões e todos os comandos de controle, documentado em `docs/parser.md` (PR #57); parser passa a construir a AST | Jefferson, Pedro |
| 15 | 19/11 a 23/11 | Execução de ponta a ponta; análise semântica básica; `make test` | Programas completos executados de ponta a ponta (PR #58); strings, funções e structs retirados do lexer e do parser, com testes de rejeição (PR #59). A análise semântica e o `make test` ficaram para a sprint seguinte | Pedro, Jefferson |
| 16 | 24/11 a 25/11 | Documentação, revisão cruzada, congelamento em 25/11 | `main` e `develop` reunidas (PRs #60 e #61); módulo de análise semântica com a linha de cada nó na AST; primeiros ajustes do `make test` e triagem dos testes de erro | Jefferson, Maria Clara, Clarice |
| Fechamento | 26/11 a 29/11 | Ensaio da entrevista | Escopos aninhados, operador `%`, `default` no `switch`, curto-circuito em `&&`/`\|\|` e erro léxico para comentário não terminado (PR #81); `make test` com programas comparados com `.esperado` e programas que devem ser rejeitados, além de mover arquivos de erro para `tests/erros/` e documentar mudanças em `docs/como_executar.md` (PR #79); `case` com constante negativa e `+` unário (PR #82); testes do scanner (`tests/lexer/`, alvo `make test_lexer`) e de erros sintáticos (PR #83); README e tabela de estado atualizados (PR #84); documentação atualizada. Ensaio da entrevista | Pedro, Clarice, Jefferson, Júlia |

### Ajustes no planejamento

* **Depois da P1**, a prioridade passou a ser ligar o parser à AST: a AST, a tabela de símbolos e o avaliador ficaram prontos antes do parser, e sem as ações semânticas nenhum programa executava.
* **Na P2**, o escopo foi reduzido: strings, funções e structs saíram da entrega. O foco ficou em `int`, `float` e `char` com todos os comandos de controle.
* **Na sprint 15**, a análise semântica e o `make test` atrasaram, porque a execução de ponta a ponta (PR #58) e a redução de escopo (PR #59) foram mescladas em branches diferentes e precisaram ser reunidas antes (veja "Problemas e soluções").
* **O congelamento planejado para 25/11 passou para 27/11.** Uma revisão do código em 24/11 encontrou falhas de semântica da linguagem (sem escopos de bloco, sem curto-circuito, sem `%` e sem `default`) que não podiam ficar para depois da entrega. Elas viraram as issues #62 a #68 e foram corrigidas em 26/11 e 27/11.

---

## Decisões técnicas

| Decisão | Motivo | Onde está |
|---|---|---|
| Implementar em C | É a linguagem das ações do Flex e do Bison e a dos exemplos da disciplina; também é a linguagem interpretada, o que facilita comparar resultados com o `gcc` | `src/` |
| Flex para o lexer e Bison para o parser | Ferramentas adotadas na disciplina; geram o scanner e um parser LALR(1) a partir de especificações declarativas | `src/lexer.l`, `src/parser.y` |
| Interpretador, não compilador | O guia do professor oferece as duas opções. Percorrer a AST e executar cada nó substitui a geração de código intermediário e cabe no prazo do semestre | `eval_ast` em `src/ast.c` |
| Subconjunto de C: `int`, `float` e `char`; sem strings, funções e structs | Reduz o trabalho a um núcleo completo e testável (declarações, expressões e todos os comandos de controle) em vez de muitas partes incompletas | PR #59; [Gramática](gramatica.md) |
| `long` e `double` aceitos, mas tratados como `int` e `float` | O lexer reconhece as palavras-chave, mas a união de valores da AST tem só `int`, `float` e `char`; tipos próprios ficaram como melhoria opcional | `src/lexer.l`, `src/ast.h` |
| AST com um `union` por tipo de nó (19 tipos) | Cada nó guarda só os campos que usa; os construtores (`new_*_node`) isolam a alocação e o parser só os chama | `src/ast.h`, `src/ast.c` |
| `else` pendente resolvido com `%precedence` | `%precedence SEM_ELSE` e `%precedence F_ELSE` ligam o `else` ao `if` mais próximo, como em C, sem conflito *shift/reduce* | `src/parser.y` |
| Tabela de símbolos em tabela hash (djb2, 211 posições, encadeamento) | Busca em tempo médio constante; a primeira versão, com arrays paralelos, fazia busca linear | `src/tabela_simbolos.c` |
| Escopos aninhados pelo nível guardado em cada entrada | A inserção é no início de cada lista do hash, então a busca já encontra a declaração mais interna; sair de um escopo remove as entradas daquele nível. Abre-se um escopo por bloco `{ }`, por `for` e por corpo de `switch`, como em C | `src/tabela_simbolos.c`, `src/ast.c` |
| Análise semântica em módulo separado, numa passada antes da execução | Erros como variável não declarada, redeclaração e `break` fora de laço aparecem todos de uma vez, com a linha, e nada é executado. Avisos (conversão de `float` para `int`, divisão por zero literal, uso sem valor) não impedem a execução | `src/semantica.c`; [Análise Semântica](semantica.md) |
| Linha do código-fonte gravada em cada nó da AST | Permite mensagens com a linha na análise semântica. Em `case` e `default`, a linha vem de uma ação no meio da regra, porque ao reduzir a regra o lexer já leu o token seguinte | `src/parser.y`, `src/ast.c` |
| Erros em `stderr` e código de saída 1 | Separa o resultado do programa dos diagnósticos e permite que o `make test` confira se um programa foi rejeitado | `src/parser.y` |
| `make test` com saídas esperadas | Cada programa válido de teste tem um `.esperado` com `stdout` e `stderr`, ignorando o código de saída do `return` da `main`; programas inválidos ficam isolados em `tests/erros/` e precisam terminar com código diferente de zero. Assim uma regressão quebra o `make test` | `Makefile`; [Como executar](como_executar.md) |
| Rótulos de `case` com sinal e `+` unário resolvidos na gramática | Reaproveita os tokens `O_MINUS` e `O_PLUS` já gerados pelo scanner, evitando complexidade ou ambiguidades léxicas com números negativos | `src/parser.y` |
| Testes do scanner ignoram o código numérico dos tokens | O código muda quando um token é adicionado ao parser (`%` e `default` mudaram os demais); comparar só o lexema e a linha evita quebrar os `.esperado` a cada token novo | `Makefile` (`make test_lexer`), `tests/lexer/` |
| Arquivos gerados fora do git | `lex.yy.c`, `parser.tab.*` e binários vão para `build/`, que é ignorado; evita conflitos em arquivos que ninguém edita | `.gitignore`, `Makefile` |
| Documentação em MkDocs publicada automaticamente | O site é sempre gerado a partir da `main`, sem edição manual da `gh-docs` | `.github/workflows/deploy.yml` |

### Saída de dados

Sem strings não há `printf`. A issue #71 considerou duas opções: (a) manter o rastro do avaliador e documentá-lo como a saída do interpretador, ou (b) criar um comando embutido que imprime números, sem strings. A equipe escolheu a **opção (a)**.

A saída do interpretador é o rastro impresso a cada declaração, atribuição e expressão (`Declarado: x : int = 3`, `int x = 4`, `Resultado da expressão: 8`), seguido de `Programa retornou: N` quando o programa executa um `return`. Criar um comando de impressão exigiria mudar o lexer, a gramática e a AST, e o rastro já atende a demonstração e os testes, que comparam essa saída com os arquivos `.esperado`.

Para programas com laços longos, em que o rastro imprime uma linha por atribuição, a opção `-q` (ou `--quiet`) do `parser_exe` desliga o rastro: só aparecem o `Programa retornou: N`, os erros e os avisos. A mudança fica toda no avaliador (`definir_rastro_execucao` em `src/ast.c`) e no `main`, sem alterar tokens, gramática ou nós da AST (PR #88).

---

## Problemas encontrados e soluções

| Problema | Quando | Solução adotada | Referência |
|---|---|---|---|
| A primeira reorganização das pastas foi revertida e o `parser.y` deixou de ser rastreado pelo git | 26/08 a 13/09 | Estrutura refeita com `src/` e `docs/` e o `parser.y` adicionado de novo | PRs #1, #2 e #23 |
| O guia de expressões regulares entrou desatualizado em relação ao `lexer.l` | 22/09 a 23/09 | O PR foi revertido e refeito com as regras atuais do lexer | PRs #28, #30 e #31 |
| Pastas `.vscode/` e arquivos gerados pelo Flex e pelo Bison eram versionados; o `.gitignore` chegou a ter marcadores de conflito | 17/08 a 06/11 | Marcadores removidos, arquivos gerados movidos para `build/` e ignorados, binário de teste retirado do versionamento | PR #51 |
| Na P1 o lexer usava um `enum` próprio de tokens e não conversava com o parser | 28/09 a 31/10 | Os tokens passaram a vir do Bison (`parser.tab.h`) e lexer e parser compartilham o `%union` | PR #48 |
| A tabela de símbolos com arrays paralelos fazia busca linear | 27/10 a 29/10 | Migração para tabela hash (djb2) com encadeamento | PR #52 |
| AST, tabela e avaliador prontos, mas o parser não construía a AST: nenhum programa executava | P2 (09/11) | Prioridade da sprint 14: ações semânticas no `parser.y` que criam os nós e chamam `eval_ast` | PRs #57 e #58 |
| *Else* pendente gerava conflito *shift/reduce* na gramática | Sprint 14 | `%precedence SEM_ELSE` e `%prec SEM_ELSE` na regra do `if` sem `else` | PR #57 |
| `main` e `develop` divergiram: o parser ligado à AST entrou só na `main` e a redução de escopo só na `develop` | 20/11 a 24/11 | Integração da `develop` na `main` com os conflitos de documentação resolvidos à mão, e depois sincronização da `develop` | PRs #60 e #61 |
| Erros semânticos só apareciam durante a execução, depois de saída parcial | Até 24/11 | Módulo de análise semântica antes do avaliador, que reporta todos os erros com a linha | Issue #62 |
| Variáveis de bloco "vazavam": `int a = 1; { int a = 2; }` deixava `a` valendo 2, e variáveis de bloco continuavam existindo depois dele | Até 26/11 | Escopos aninhados na tabela de símbolos | Issue #63, PR #81 |
| `&&` e `||` avaliavam os dois lados: `a && (1 / a)` com `a == 0` abortava com divisão por zero | Até 26/11 | Curto-circuito no avaliador; o resultado é sempre 0 ou 1 | Issue #64, PR #81 |
| Comentário `/*` sem fechamento: o lexer mostrava o erro, mas o programa executava e saía com código 0 | Até 26/11 | Contador de erros léxicos consultado pelo `main` depois do `yyparse()`; o programa não executa e sai com código 1 | Issue #67, PR #81 |
| O `make test` da análise semântica e o da issue #68 conflitaram no `Makefile`; `echo "\n"` imprimia o `\n` literal | 26/11 | Merge manual que manteve a estrutura da #68, incluiu `tests/semantica/` e trocou `echo` por `printf` | PRs #79 e #81 |
| `case -1:` e `+x` davam erro sintático porque `rotulo` só aceitava literais sem sinal e não havia regra para `+` unário | Até 27/11 | Inclusão de `O_MINUS L_INT` e `O_PLUS L_INT` na regra `rotulo` e adição da regra `O_PLUS expressao %prec UMINUS` no `parser.y`, com novos testes em `tests/programas/case_negativo.c` | Issue #76, PR #82 |
| O `make test` reprovava o que estava certo e aprovava o que estava errado: programas rejeitados de propósito no grupo dos que devem passar, `.esperado` executados como código, `return N` tratado como falha e um `#` dentro da receita que comentava o resto do comando | 26/11 | Separação dos arquivos com erro intencional para `tests/erros/`, comparação de `stdout/stderr` com `.esperado` nos programas válidos (ignorando o código de retorno da `main`) e atualização do guia em `docs/como_executar.md` | PR #79 |
| O `+` unário passava no parser mas falhava na execução (`Operador unário desconhecido '+'`), e o teste não acusava porque não tinha `.esperado` | 27/11 | `+` tratado no avaliador e `.esperado` criado para o teste | PR #82 |
| Nos testes do scanner, as mensagens de erro (`stderr`) saíam antes dos tokens (`stdout`) quando os dois iam juntos para um pipe | 27/11 | `setvbuf` linha a linha no `main` do lexer isolado; os `.esperado` seguem a ordem do programa de entrada | PR #83 |

---

## O que ficou de fora

Melhorias opcionais registradas como issues e não implementadas na entrega:

* mensagens de erro sintático mais claras e recuperação após o primeiro erro (#72);
* tipos próprios para `long` e `double` (#73);
* otimização por *constant folding* (#74);
* vazamento de memória quando o programa tem erro sintático (#75).
