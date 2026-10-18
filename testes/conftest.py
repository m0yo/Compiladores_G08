# /home/pedro/Documents/Compiladores_G08/testes/conftest.py
import ctypes
import os
import platform
import pytest

# Caminho da biblioteca compartilhada compilada do Flex
LIB_PATH = os.path.abspath(
    "./liblexer.so" if platform.system() != "Windows" else "./liblexer.dll"
)

lexer_lib = ctypes.CDLL(LIB_PATH)

# Mapeamento do valor retornado pelas macros em C para a string do Token
TOKEN_MAP = {
    258: "L_FLOAT", 259: "L_INT", 260: "L_CHAR", 261: "CHAR_LIT",
    262: "T_INT", 263: "T_FLOAT", 264: "T_CHAR", 265: "T_LONG", 266: "T_DOUBLE",
    267: "F_IF", 268: "F_ELSE", 269: "F_WHILE", 270: "F_FOR", 271: "F_DO",
    272: "F_SWITCH", 273: "F_CASE", 274: "F_BREAK", 275: "F_CONTINUE", 276: "F_RETURN",
    277: "O_PLUS", 278: "O_MINUS", 279: "O_MULTI", 280: "O_DIV", 281: "O_ASSIGN",
    282: "R_EQ", 283: "R_NE", 284: "R_LT", 285: "R_GT", 286: "R_LE", 287: "R_GE",
    288: "DM_AND", 289: "DM_OR", 290: "DM_NOT",
    291: "LBRACE", 292: "RBRACE", 293: "LBRACKET", 294: "RBRACKET",
    295: "LPAREN", 296: "RPAREN", 297: "COLON", 298: "SEMICOLON", 299: "COMMA",
    300: "IDENT"
}

# Assinaturas das funções nativas do Flex no ctypes
yytext_ptr = ctypes.c_char_p.in_dll(lexer_lib, "yytext")
yylex = lexer_lib.yylex
yylex.restype = ctypes.c_int

yy_scan_string = lexer_lib.yy_scan_string
yy_scan_string.argtypes = [ctypes.c_char_p]
yy_scan_string.restype = ctypes.c_void_p

yy_delete_buffer = lexer_lib.yy_delete_buffer
yy_delete_buffer.argtypes = [ctypes.c_void_p]


@pytest.fixture
def scan():
    def _scan(code_str: str):
        tokens = []
        buffer = yy_scan_string(code_str.encode("utf-8"))

        while True:
            token_id = yylex()
            if token_id == 0:  # Fim do buffer (EOF)
                break

            val = yytext_ptr.value.decode("utf-8") if yytext_ptr.value else ""
            token_type = TOKEN_MAP.get(token_id, "UNKNOWN")
            tokens.append({"type": token_type, "value": val})

        yy_delete_buffer(buffer)
        return tokens

    return _scan