/* Le o codigo fonte e gera os tokens para o bison (parser) */

%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*#include "parser.tab.h"*/

int linha_erro = 1;
%}

%%

 /* Literais numericos */
[0-9]+\.[0-9]*|\.[0-9]+  {
    yylval.floatValue = atof(yytext);
    return L_FLOAT;
}

[0-9]+  {
    yylval.intValue = atoi(yytext);
    return L_INT;
}

 /* Literais de caractere (sequências de escape devem vir antes) */
'\\.'     {
    switch (yytext[2]) {
        case 'a':  yylval.charValue = '\a'; break;
        case 'b':  yylval.charValue = '\b'; break;
        case 'f':  yylval.charValue = '\f'; break;
        case 'n':  yylval.charValue = '\n'; break;
        case 'r':  yylval.charValue = '\r'; break;
        case 't':  yylval.charValue = '\t'; break;
        case 'v':  yylval.charValue = '\v'; break;
        case '\\': yylval.charValue = '\\'; break;
        case '\'': yylval.charValue = '\''; break;
        case '"':  yylval.charValue = '"';  break;
        case '?':  yylval.charValue = '?';  break;
        case '0':  yylval.charValue = '\0'; break;
        default:
            fprintf(stderr, "Sequência de escape inválida: %s\n", yytext);
            yylval.charValue = yytext[2];
            break;
    }
    return CHAR_LIT;
}

'[^\\']'  {
    yylval.charValue = yytext[1];
    return L_CHAR;
}

 /* Palavras-chave de tipos de dados */
"int"       { return T_INT; }
"float"     { return T_FLOAT; }
"char"      { return T_CHAR; }
"long"      { return T_LONG; }
"double"    { return T_DOUBLE; }

 /* Palavras-chave de controle de fluxo */
"if"        { return F_IF; }
"else"      { return F_ELSE; }
"while"     { return F_WHILE; }
"for"       { return F_FOR; }
"do"        { return F_DO; }
"switch"    { return F_SWITCH; }
"case"      { return F_CASE; }
"break"     { return F_BREAK; }
"continue"  { return F_CONTINUE; }
"return"    { return F_RETURN; }

 /* Operadores compostos (devem vir antes dos de 1 caractere) */
"=="        { return R_EQ; }
"!="        { return R_NE; }
"<="        { return R_LE; }
">="        { return R_GE; }
"&&"        { return DM_AND; }
"||"        { return DM_OR; }

 /* Operadores simples */
"+"         { return O_PLUS; }
"-"         { return O_MINUS; }
"*"         { return O_MULTI; }
"/"         { return O_DIV; }
"="         { return O_ASSIGN; }
"<"         { return R_LT; }
">"         { return R_GT; }
"!"         { return DM_NOT; }

 /* Delimitadores e Pontuações */
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
"("         { return LPAREN; }
")"         { return RPAREN; }
":"         { return COLON; }
";"         { return SEMICOLON; }
","         { return COMMA; }

 /* Identificadores */
[a-zA-Z_][a-zA-Z0-9_]*  {
    yylval.strValue = strdup(yytext);
    return IDENT;
}

 /* Ignora espacos, tabs e quebras de linha */
[ \t\n]+  { /* ignora */ }

 /* Fallback para caracteres desconhecidos */
.         { printf("Caractere não reconhecido: %s\n", yytext); }

%%

int yywrap(void) {
    return 1;
}