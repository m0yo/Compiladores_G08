
#!/bin/sh
set -eu

exec_path=${1:-./parser_exe}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

falhar() {
    echo "[FALHOU] $1" >&2
    exit 1
}

# Laço longo
status=0
"$exec_path" -q tests/quiet/laco_longo.c > "$tmp/out" 2> "$tmp/err" || status=$?

[ "$status" -eq 64 ] || falhar "código de saída do laço longo"

diff -u tests/quiet/laco_longo.esperado "$tmp/out" || falhar "saída do laço longo"

[ ! -s "$tmp/err" ] || falhar "diagnósticos inesperados no laço longo"

echo "[OK ] modo silencioso: laço longo"

# Alias, entrada padrão e ausência de return
status=0
printf 'int x; int y = 3; x = y + 1; x * 2;\n' | "$exec_path" --quiet > "$tmp/out" 2> "$tmp/err" || status=$?

[ "$status" -eq 0 ] || falhar "--quiet pela entrada padrão"

[ ! -s "$tmp/out" ] && [ ! -s "$tmp/err" ] || falhar "saída sem return"

echo "[OK ] modo silencioso: alias e entrada padrão"

# Compatibilidade com a impressão da AST
status=0
printf 'int x = 3; return x;\n' | "$exec_path" -a -q > "$tmp/out" 2> "$tmp/err" || status=$?

[ "$status" -eq 3 ] || falhar "código de saída com -a -q"

grep -Fxq '=== AST ===' "$tmp/out" || falhar "impressão da AST"

grep -Fxq 'Programa retornou: 3' "$tmp/out" || falhar "return com -a -q"

if grep -Eq '^(Declarado:|int x =|Resultado da expressão:)' "$tmp/out"; then
    falhar "rastro indevido com -a -q"
fi

[ ! -s "$tmp/err" ] || falhar "diagnósticos inesperados com -a -q"

echo "[OK ] modo silencioso: compatibilidade com -a"

# Preservação de erros e avisos existentes
for f in tests/erros/comentario_nao_terminado.c tests/semantica/nao_declarada_expressao.c tests/semantica/aviso_float_para_int.c; do
    status_normal=0
    status_quiet=0

    "$exec_path" "$f" > "$tmp/normal" 2> "$tmp/err_normal" || status_normal=$?

    "$exec_path" -q "$f" > "$tmp/out" 2> "$tmp/err" || status_quiet=$?

    [ "$status_normal" -eq "$status_quiet" ] || falhar "código de saída: $f"

    [ ! -s "$tmp/out" ] || falhar "stdout indevido: $f"

    [ -s "$tmp/err_normal" ] || falhar "diagnóstico ausente: $f"

    diff -u "$tmp/err_normal" "$tmp/err" || falhar "diagnóstico alterado: $f"

    echo "[OK ] modo silencioso: $f"
done

# Erro sintático
status=0
printf 'int x = ;\n' | "$exec_path" -q > "$tmp/out" 2> "$tmp/err" || status=$?

[ "$status" -eq 1 ] && [ ! -s "$tmp/out" ] || falhar "erro sintático"

grep -Fq 'Erro sintático' "$tmp/err" || falhar "mensagem de erro sintático"

echo "[OK ] modo silencioso: erro sintático"

# Erro de execução
status=0
printf 'int x = 0; 10 / x;\n' | "$exec_path" -q > "$tmp/out" 2> "$tmp/err" || status=$?

[ "$status" -eq 1 ] && [ ! -s "$tmp/out" ] || falhar "erro de execução"

grep -Fxq 'Erro: Divisão por zero!' "$tmp/err" || falhar "mensagem de erro de execução"

echo "[OK ] modo silencioso: erro de execução"
