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
CFLAGS = -I$(BUILD_DIR)

# Compila lexer + parser juntos
all: $(EXEC)

$(EXEC): $(BISON_C) $(FLEX_C)
	$(CC) $(CFLAGS) -o $@ $(BISON_C) $(FLEX_C)

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

clean:
	rm -rf $(EXEC) $(LEXER_EXEC) $(BUILD_DIR)

.PHONY: all clean
