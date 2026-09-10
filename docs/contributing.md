# Guia de Contribuição

Este documento estabelece as regras para desenvolvimento, documentação, commits, Issues e Pull Requests do projeto.

## 1. Organização das branches

O projeto utiliza a seguinte organização:

| Branch | Finalidade |
|---|---|
| `main` | Versão estável do projeto |
| `develop` | Integração das alterações em desenvolvimento |
| `feature/*` | Desenvolvimento de novas funcionalidades |
| `fix/*` | Correção de problemas |
| `docs/*` | Alterações exclusivamente na documentação |
| `test/*` | Criação ou alteração de testes |
| `refactor/*` | Refatoração do código |
| `gh-docs` | Publicação da documentação gerada pelo MkDocs |

### 1.1 Branch `main`

A `main` representa a versão estável do projeto.

Não devem ser realizadas alterações diretamente nela.

Alterações devem chegar à `main` por meio de Pull Requests.

### 1.2 Branch `develop`

A `develop` é utilizada para integração das funcionalidades que ainda estão em desenvolvimento.

O fluxo recomendado é:

```text
main
  │
  └── develop
        │
        ├── feature/*
        ├── fix/*
        ├── docs/*
        └── test/*
```

Após as alterações serem desenvolvidas e revisadas, elas são integradas à `develop`.

Quando a versão estiver estável, a `develop` pode ser integrada à `main` por meio de Pull Request.

### 1.3 Branches de trabalho

Branches de trabalho devem ser criadas a partir da `develop`.

Exemplos:

```text
feature/analisador-lexico
feature/tabela-simbolos
fix/erro-parser
docs/arquitetura
test/analisador-sintatico
refactor/estrutura-parser
```

Evite nomes genéricos como:

```text
teste
mudancas
nova-branch
coisas
final
```

## 2. Fluxo de desenvolvimento

O fluxo padrão para uma alteração é:

```text
1. Atualizar develop
        ↓
2. Criar branch de trabalho
        ↓
3. Implementar alteração
        ↓
4. Criar commits
        ↓
5. Executar testes
        ↓
6. Abrir Pull Request
        ↓
7. Revisão
        ↓
8. Merge na develop
        ↓
9. Versão estável → main
```

Antes de iniciar uma nova tarefa:

```bash
git switch develop
git pull origin develop
```

Crie sua branch:

```bash
git switch -c feature/nome-da-feature
```

## 3. Padrão de commits

O projeto utiliza o padrão de commits baseado em:

https://github.com/iuricode/padroes-de-commits

O formato utilizado é:

```text
<emoji> <tipo>: <descrição>
```

Os emojis são **opicionais**, eles só ajudam na identificação do commit

### 3.1 Tipos de commit

| Tipo | Emoji | Utilização |
|---|---|---|
| `feat` | ✨ | Nova funcionalidade |
| `fix` | 🐛 | Correção de bug |
| `docs` | 📚 | Documentação |
| `test` | 🧪 | Testes |
| `refactor` | ♻️ | Refatoração |
| `style` | 👌 | Alterações de estilo/formatação |
| `build` | 📦 | Build e dependências |
| `ci` | 🧱 | Integração contínua |
| `perf` | ⚡ | Melhorias de desempenho |
| `chore` | 🔧 | Manutenção/configuração |
| `cleanup` | 🧹 | Limpeza |
| `remove` | 🗑️ | Remoção |
| `raw` | 🗃️ | Arquivos ou dados auxiliares |

### 3.2 Exemplos

```text
✨ feat: implementar analisador léxico
```

```text
🐛 fix: corrigir reconhecimento de identificadores
```

```text
📚 docs: documentar analisador sintático
```

```text
🧪 test: adicionar testes do analisador léxico
```

```text
♻️ refactor: reorganizar tabela de símbolos
```

### 3.3 Regras

- Escrever os commits em português.
- Utilizar uma descrição objetiva.
- Não terminar a mensagem com ponto.
- Evitar mensagens genéricas.
- Cada commit deve representar uma alteração lógica.
- Não misturar alterações sem relação no mesmo commit.

Evite:

```text
update
```

```text
fix
```

```text
mudanças
```

```text
alterações finais
```

Prefira:

```text
🐛 fix: corrigir reconhecimento de strings
```

## 4. Issues

Todo trabalho relevante deve estar associado a uma Issue.

Antes de iniciar uma tarefa, verifique se já existe uma Issue relacionada.

As Issues devem possuir:

- descrição do problema ou tarefa;
- objetivo;
- tarefas necessárias;
- critérios de aceite;
- referências, quando necessário.

### 4.1 Tipos de Issue

Utilize os seguintes tipos:

- `feature` — nova funcionalidade;
- `bug` — comportamento incorreto;
- `documentation` — documentação;
- `task` — tarefa de manutenção ou configuração.

### 4.2 Título

Os títulos devem ser objetivos.

Exemplos:

```text
feat: implementar analisador léxico
```

```text
fix: corrigir precedência de operadores
```

```text
docs: documentar arquitetura do compilador
```

```text
chore: configurar ambiente de testes
```

## 5. Pull Requests

Toda alteração destinada à `develop` ou `main` deve ser realizada por meio de Pull Request.

Não faça push diretamente nessas branches.

### 5.1 Título

O título do Pull Request deve seguir o mesmo padrão dos commits.

Exemplo:

```text
✨ feat: implementar analisador léxico
```

### 5.2 Descrição

O Pull Request deve informar:

- o que foi alterado;
- por que a alteração foi realizada;
- Issue relacionada;
- testes realizados;
- eventuais impactos na documentação.

Sempre que possível, utilize:

```text
Closes #XX
```

para vincular o PR à Issue correspondente.

### 5.3 Checklist

Antes de solicitar a revisão:

- [ ] Código implementado
- [ ] Testes executados
- [ ] Novos testes adicionados quando necessário
- [ ] Documentação atualizada quando necessário
- [ ] Commits seguem o padrão do projeto
- [ ] Não existem arquivos desnecessários
- [ ] Issue relacionada foi vinculada
- [ ] Branch está atualizada com a branch de destino

## 6. Revisão de código

Pull Requests devem ser revisados antes do merge.

O revisor deve verificar:

- funcionamento da implementação;
- legibilidade do código;
- possíveis erros;
- testes;
- impacto em outras partes do projeto;
- documentação;
- conformidade com as regras deste documento.

Alterações solicitadas durante a revisão devem ser corrigidas antes do merge.

## 7. Documentação

A documentação fonte do projeto está localizada em:

```text
docs/
```

O arquivo de configuração do MkDocs é:

```text
mkdocs.yml
```

A documentação faz parte da `main` e deve ser atualizada junto ao projeto quando necessário.

Alterações exclusivamente na documentação devem utilizar branches:

```text
docs/nome-da-alteracao
```

Exemplo:

```text
docs/documentar-analisador-lexico
```

A documentação publicada pelo GitHub Pages é gerada automaticamente pelo MkDocs e disponibilizada na branch:

```text
gh-docs
```

A branch `gh-docs` não deve ser utilizada para editar manualmente a documentação.

## 8. Testes

Antes de abrir um Pull Request, execute os testes disponíveis no projeto.

Alterações que modificam o comportamento do compilador devem possuir testes correspondentes sempre que aplicável.

Novos testes devem ser adicionados à estrutura existente de testes.

## 9. Boas práticas gerais

- Mantenha o código simples e legível.
- Evite duplicação de código.
- Não envie arquivos temporários ou gerados desnecessariamente.
- Não faça commits diretamente na `main` ou `develop`.
- Mantenha sua branch atualizada.
- Faça commits pequenos e relacionados.
- Documente alterações que afetem o funcionamento do projeto.
- Não altere arquivos de outros módulos sem necessidade.
- Resolva conflitos com atenção antes de realizar o merge.

## 10. Resumo do fluxo

```text
Issue
  ↓
develop
  ↓
feature/fix/docs/test
  ↓
implementação
  ↓
commits padronizados
  ↓
testes
  ↓
Pull Request
  ↓
revisão
  ↓
develop
  ↓
versão estável
  ↓
Pull Request
  ↓
main
  ↓
GitHub Actions
  ↓
MkDocs
  ↓
gh-docs
  ↓
GitHub Pages
```

Seguindo esse fluxo, o projeto mantém o desenvolvimento, a documentação e a publicação organizados e versionados de forma consistente.