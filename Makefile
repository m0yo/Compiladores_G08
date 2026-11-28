# Executáveis
EXEC       = parser_exe
LEXER_EXEC = lexer_exe
BUILD_DIR  = build

# Fontes
BISON_FILE = src/parser.y
FLEX_FILE  = src/lexer.l

# Arquivos gerados
BISON_C = $(BUILD_DIR)/parser.tab.c
BISON_H = $(BUILD_DIR)/parser.tab.h
FLEX_C  = $(BUILD_DIR)/lex.yy.c

CC     = gcc
CFLAGS = -Wall -I$(BUILD_DIR) -Isrc

# Módulos em C (AST, avaliador e tabela de símbolos)
C_SRCS = src/ast.c src/tabela_simbolos.c src/semantica.c

# Compila lexer + parser juntos
all: $(EXEC)

$(EXEC): $(BISON_C) $(FLEX_C) $(C_SRCS) src/ast.h src/tabela_simbolos.h src/semantica.h
	$(CC) $(CFLAGS) -o $@ $(BISON_C) $(FLEX_C) $(C_SRCS) -lm

# Lexer isolado: imprime os tokens (usa o parser.tab.h só para os códigos dos tokens)
$(LEXER_EXEC): $(BISON_H) $(FLEX_C)
	$(CC) $(CFLAGS) -DLEXER_STANDALONE -o $@ $(FLEX_C)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Bison primeiro: gera parser.tab.c e parser.tab.h em build/
$(BISON_C) $(BISON_H): $(BISON_FILE) | $(BUILD_DIR)
	bison -d --defines=$(BISON_H) -o $(BISON_C) $(BISON_FILE)

# Flex depois: o lexer inclui o parser.tab.h
$(FLEX_C): $(FLEX_FILE) $(BISON_H) | $(BUILD_DIR)
	flex -o $(FLEX_C) $(FLEX_FILE)

# Testes: unitários da AST + programas de exemplo e de análise semântica (saída comparada com .esperado)
TEST_AST = $(BUILD_DIR)/test_ast

$(TEST_AST): src/test_main.c $(C_SRCS) src/ast.h src/tabela_simbolos.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ src/test_main.c $(C_SRCS) -lm

test: $(EXEC) $(TEST_AST)
	@echo "=== Rodando testes unitários da AST ==="
	@./$(TEST_AST) > /dev/null && echo "[OK ] testes unitários da AST" || { echo "[FALHOU] testes unitários da AST"; exit 1; }
	@falhas=0; \
	$(MAKE) --no-print-directory test_lexer || falhas=1; \
	printf "\n=== Rodando programas (saída comparada com .esperado) ===\n"; \
	for f in tests/programas/* tests/semantica/*; do \
		[ -e "$$f" ] || continue; \
		[ -d "$$f" ] && continue; \
		if echo "$$f" | grep -q "\.esperado$$"; then continue; fi; \
		esp="$${f%.*}.esperado"; \
		if [ -f "$$esp" ]; then \
			if ./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" - > /dev/null; then \
				echo "[OK ] $$f"; \
			else \
				echo "[FALHOU] $$f (saída diferente do esperado)"; \
				./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" -; \
				falhas=1; \
			fi; \
		else \
			./$(EXEC) < "$$f" > /dev/null 2>&1; \
			echo "[OK ] $$f"; \
		fi; \
	done; \
	\
	printf "\n=== Rodando testes de erro (devem falhar) ===\n"; \
	for f in tests/erros/*; do \
		[ -e "$$f" ] || continue; \
		[ -d "$$f" ] && continue; \
		if echo "$$f" | grep -q "\.esperado$$"; then continue; fi; \
		esp="$${f%.*}.esperado"; \
		./$(EXEC) < "$$f" > /dev/null 2>&1; \
		status=$$?; \
		if [ $$status -eq 0 ]; then \
			echo "[FALHOU] $$f (esperava erro, mas passou com sucesso)"; \
			falhas=1; \
		else \
			if [ -f "$$esp" ]; then \
				if ./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" - > /dev/null; then echo "[OK ] $$f (rejeitado corretamente)"; \
				else echo "[FALHOU] $$f (erro ocorreu, mas stdout/stderr difere do .esperado)"; ./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" -; falhas=1; fi; \
			else \
				echo "[OK ] $$f (rejeitado corretamente)"; \
			fi; \
		fi; \
	done; \
	exit $$falhas
	@printf "\n=== Rodando testes do modo silencioso ===\n"
	@sh tests/quiet/test_quiet.sh ./$(EXEC)

clean:
	rm -rf $(EXEC) $(LEXER_EXEC) $(BUILD_DIR)

.PHONY: all clean test test_lexer

# Testes do scanner: cada tests/lexer/*.txt passa pelo lexer isolado e a saída (stdout e stderr
# juntos) é comparada com o .esperado. O código numérico do token é removido da comparação, para
# que um token novo no parser não mude os códigos dos demais e quebre os casos. O lexer isolado
# imprime stdout linha a linha (setvbuf no main de lexer.l); sem isso, as mensagens de erro (stderr)
# sairiam antes dos tokens quando a saída vai para um pipe.
test_lexer: $(LEXER_EXEC)
	@echo "=== Rodando testes do scanner ==="
	@falhas=0; for f in tests/lexer/*.txt; do \
		[ -e "$$f" ] || continue; \
		esp="$${f%.txt}.esperado"; \
		if ./$(LEXER_EXEC) < "$$f" 2>&1 | sed 's/^Token: [0-9]* | //' | diff -u "$$esp" - > /dev/null; then echo "[OK ] $$f"; \
		else echo "[FALHOU] $$f"; ./$(LEXER_EXEC) < "$$f" 2>&1 | sed 's/^Token: [0-9]* | //' | diff -u "$$esp" -; falhas=1; fi; \
	done; exit $$falhas
