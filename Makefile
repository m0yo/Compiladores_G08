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
C_SRCS = src/ast.c src/tabela_simbolos.c

# Compila lexer + parser juntos
all: $(EXEC)

$(EXEC): $(BISON_C) $(FLEX_C) $(C_SRCS) src/ast.h src/tabela_simbolos.h
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

# Testes: unitários da AST + programas de exemplo (saída comparada com .esperado)
TEST_AST = $(BUILD_DIR)/test_ast

$(TEST_AST): src/test_main.c $(C_SRCS) src/ast.h src/tabela_simbolos.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ src/test_main.c $(C_SRCS) -lm

test: $(EXEC) $(TEST_AST)
	@echo "=== Rodando testes unitários da AST ==="
	@./$(TEST_AST) > /dev/null && echo "[OK ] testes unitários da AST" || { echo "[FALHOU] testes unitários da AST"; exit 1; }
	
	@echo "\n=== Rodando testes de escopo / válidos ==="
	@falhas=0; for f in tests/escopo/*.txt tests/programas/*; do \
		[ -e "$$f" ] || continue; \
		[ -d "$$f" ] && continue; \
		if echo "$$f" | grep -q "\.esperado$$"; then continue; fi; \
		esp="$${f%.*}.esperado"; \
		if [ -f "$$esp" ]; then \
			# Executa o interpretador e compara o resultado com o .esperado, ignorando o exit code
			if ./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" - > /dev/null; then \
				echo "[OK ] $$f"; \
			else \
				echo "[FALHOU] $$f (saída diferente do esperado)"; \
				./$(EXEC) < "$$f" 2>&1 | diff -u "$$esp" -; \
				falhas=1; \
			fi; \
		else \
			# Se não tem .esperado, apenas roda para ver se executa
			./$(EXEC) < "$$f" > /dev/null 2>&1; \
			echo "[OK ] $$f"; \
		fi; \
	done; \
	\
	echo "\n=== Rodando testes de erro (devem falhar) ==="; \
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

clean:
	rm -rf $(EXEC) $(LEXER_EXEC) $(BUILD_DIR)

.PHONY: all clean test