/*le o codigo fonte e gera os token pra o bison(parser)*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include "parser.tab.h" descomentar isso depois que estiver integrado com o bison*/  

int linha_erro = 1;

enum{
    /*tipos*/
        T_INT = 256, //x <= 255 reservados pra tabela ascii
        T_FLOAT,
        T_CHAR,
        T_LONG,
        T_DOUBLE,

    /*controle de fluxo e laços de repeticao*/
        F_IF,
        F_ELSE,
        F_WHILE,
        F_FOR,
        F_DO,
        F_SWITCH,
        F_CASE,
        F_BREAK,
        F_CONTINUE,
        F_RETURN,

    /*literais(valores semanticos)*/
        L_INT,
        L_FLOAT,
        L_STRING, 
        L_CHAR,

    /*identificador*/
        IDENT,

    /*operadores aritméticos*/
        O_PLUS,
        O_MINUS,
        O_MULTI,
        O_DIV,

    /*operador de atribuicao*/
        O_ASSIGN,

    /*operadores relacionais*/
        R_EQ,
        R_NE,
        R_LT,
        R_GT,
        R_LE,
        R_GE,

    /*operadores logicos*/
        DM_AND,
        DM_OR,
        DM_NOT,

    /*delimitadores*/
        LBRACE, 
        RBRACE, 
        LPAREN, 
        RPAREN, 
        LBRACKET, 
        RBRACKET, 
        COLON,
        SEMICOLON, 
        COMMA
};

typedef union{
    int ival;
    float dval;
    char cval;
    char *sval;
} YYSTYPE;

YYSTYPE yylval;

static char trad_esc(char escape){
    switch (escape) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '0': return '\0';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '"';
        default: return escape;
    }
}

static char *dup_string(const char *text){
    size_t leng = strlen(text) + 1;
    char *dup = malloc(leng);

    if (dup == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(dup, text, leng);
    return dup;
}

static char *decod_string(const char *text){
    size_t leng = strlen(text);
    char *res = malloc(leng + 1);
    size_t i = 1;
    size_t j = 0;

    if (res == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (i < leng - 1){
        if (text[i] == '\\' && i + 1 < leng - 1){
            res[j++] = trad_esc(text[i + 1]);
            i += 2;
        }
        else {
            res[j++] = text[i++];
        }
    }

    res[j] = '\0';
    return res;
}

static char decod_char(const char *text){
    if (text[1] == '\\') return trad_esc(text[2]);

    return text[1];
}

%}

%option noyywrap

%x COMENTARIO

/*expressao regular*/
%%

"int"       { return T_INT; }
"float"     { return T_FLOAT; }
"char"      { return T_CHAR; }
"long"      { return T_LONG; }
"double"    { return T_DOUBLE; }

"if"        { return F_IF; }
"else"      { return F_ELSE; }
"while"     { return F_WHILE;}
"for"       { return F_FOR; }
"do"        { return F_DO; }
"switch"    { return F_SWITCH; }
"case"      { return F_CASE; }
"break"     { return F_BREAK; }
"continue"  { return F_CONTINUE; }
"return"    { return F_RETURN; }

"=="        { return R_EQ; }
"!="        { return R_NE; }
"<"         { return R_LT; }
">"         { return R_GT; }
"<="        { return R_LE; }
">="        { return R_GE; }

"&&"        { return DM_AND; }
"||"        { return DM_OR; }
"!"         { return DM_NOT; }

"+"         { return O_PLUS; }
"-"         { return O_MINUS; }
"*"         { return O_MULTI; }
"/"         { return O_DIV; }

"="         { return O_ASSIGN; }

"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
"("         { return LPAREN; }
")"         { return RPAREN; }
":"         {return COLON; }
";"         { return SEMICOLON; }
","         { return COMMA; }

/* comentarios */
"//" [^\n]* {

}

"/*" {
    BEGIN(COMENTARIO)
}

<COMENTARIO>[^*\n]+ {
    /* texto do comentario */
}

<COMENTARIO>\n {
    linha_erro++;
}

<COMENTARIO>"*/" {
    BEGIN(INITIAL);
}

<COMENTARIO>\*+[^/\n]* {
    /* texto */
}

<COMENTARIO>\*+ {
    /* asteriscos que nao fecham o comentario */
}

<COMENTARIO><<EOF>> {
    fprintf(stderr,
        "erro na linha %d: comentario de bloco nao terminado\n",
        linha_erro
    );
    return 0;
}

/* Literais */
[0-9]+\.[0-9]+ {
    yylval.dval = strtod(yytext, NULL);
    return L_FLOAT;
}

[0-9]+ {
    yyval.ival = (int)strtol(yytext, NULL, 10);
    retrun L_INT;
}

'(\\.|[^"\\])*\' {
    yylval.cval = decod_string(yytext);
    return L_CHAR;
}

\"(\\.|[^"\\])*\" {
    yylval.sval = decod_string(yytext);
    return L_STRING;
}

[a-zA-Z_][a-zA-Z0-9]* {
    yylval.sval = dup_string(yytext);
    return IDENT;
}

/* espacos e linhas */
\n  { 
    linha_erro++;
}

[ \t\r]+ {
    /* ignora espaços */
}


/* caractere nao conhecido */
. {
    fprintf(stderr, "erro na linha %d: '%s'\n", linha_erro, yytext);
}

%%

#ifndef PARSER_TAB_H
int main(void) {
    int token;
    while ((token = yylex()) != 0) {
        printf("Token: %d | Lexema: '%s' | Linha: %d\n", token, yytext, linha_erro);
    }
    return 0;
}
#endif

// Teste: gcc lex.yy.c -o teste -lfl
// Execução: ./teste < teste.txt
