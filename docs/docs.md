## Documentação

A documentação do projeto deve acompanhar a evolução do código e ser mantida de forma clara, objetiva e organizada.

### Onde documentar

Os arquivos da documentação devem ser armazenados no diretório `docs/`:

```text
docs/
├── index.md
├── contributing.md
├── lexer.md
├── parser.md
└── ...
```

Os arquivos devem utilizar o formato **Markdown (`.md`)** e seguir a estrutura definida no `mkdocs.yml`.

### Quando atualizar a documentação

A documentação deve ser atualizada sempre que uma alteração no projeto modificar seu funcionamento, arquitetura ou forma de utilização.

Exemplos:

- Implementação de uma nova funcionalidade;
- Alteração no analisador léxico;
- Alteração no analisador sintático;
- Alteração na análise semântica;
- Alteração na geração de código;
- Alteração na estrutura do projeto;
- Alteração no processo de compilação ou execução;
- Inclusão ou alteração de testes relevantes.

### Padrão dos arquivos

Cada página deve possuir um título principal e organizar seu conteúdo utilizando subtítulos.

Exemplo:

```md
# Analisador Léxico

Descrição geral do analisador léxico.

## Funcionamento

Explicação sobre seu funcionamento.

## Estrutura

Descrição dos principais componentes.

## Exemplos

Exemplos de utilização ou funcionamento.
```

Sempre que necessário, podem ser utilizados:

- Blocos de código;
- Tabelas;
- Listas;
- Diagramas;
- Imagens;
- Links para outras páginas da documentação.

### Organização da navegação

Toda nova página criada em `docs/` deve ser adicionada ao `nav` do arquivo `mkdocs.yml`.

Exemplo:

```yaml
nav:
  - Home: index.md
  - Contribuição: contributing.md
  - Compilador:
      - Analisador Léxico: lexer.md
      - Analisador Sintático: parser.md
```

Isso garante que a página esteja disponível na navegação do site.

### Responsabilidade do contribuinte

Ao realizar uma alteração que afete a documentação, o contribuinte deve:

- [ ] Criar ou atualizar a página correspondente;
- [ ] Adicionar a página ao `mkdocs.yml`, quando necessário;
- [ ] Verificar se os links estão funcionando;
- [ ] Verificar se os exemplos estão corretos;
- [ ] Conferir a renderização da página;
- [ ] Manter a documentação consistente com o código atual.

### Documentação e Pull Requests

Alterações na documentação devem ser incluídas no mesmo Pull Request da funcionalidade ou correção quando estiverem diretamente relacionadas a ela.

Para alterações exclusivamente documentais, utilizar uma branch seguindo o padrão:

```text
docs/<descricao>
```

Exemplo:

```text
docs/atualizar-documentacao-lexer
```

O commit deve seguir o padrão definido pelo projeto:

```text
📚 docs: atualizar documentação do lexer
```

O Pull Request deve indicar claramente quais páginas foram criadas ou modificadas.