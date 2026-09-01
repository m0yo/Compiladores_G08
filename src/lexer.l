/*le o codigo fonte e gera os token pra o bison(parser)*/
/*por enquanto mockado com um enum para cada simbolo*/

%{ 
#include<stdio.h>
#include<stdlib.h>
/*#include "parser.tab.h" descomentar isso depois que estiver integrado com o bison*/  


int linha_erro = 1;


enum{
    /*tipos*/
        T_INT = 256, 
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

%}

%option noyywrap

/*adicionar regex  para ignorar comentarios e logica de contagem de linhas para apontar o erro, em cima desse regex*/

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

"+"         { return O_PLUS; }
"-"         { return O_MINUS; }
"*"         { return O_MULTI; }
"/"         { return O_DIV; }

"="         { return O_ASSIGN; }

"=="        { return R_EQ; }
"!="        { return R_NE; }
"<"         { return R_LT; }
">"         { return R_GT; }
"<="        { return R_LE; }
">="        { return R_GE; }

"&&"        { return DM_AND; }
"||"        { return DM_OR; }
"!"         { return DM_NOT; }

"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
"("         { return LPAREN; }
")"         { return RPAREN; }
":"         {return COLON; }
";"         { return SEMICOLON; }
","         { return COMMA; }

[0-9]+\.[0-9]       { return L_FLOAT; }
[0-9]+              { return L_INT; }
'(\\.|[^'\\])'      { return L_CHAR; } /*aceita caracteres com \x ou caracteres comuns que não sejam ' nem \*/
\"(\\.|[^"\\])*\"   { return L_STRING; }/*aceita 0 ou mais caracteres internos, permitindo sequencia de esquape (\n) e texto normal sem ser " ou \*/

[a-zA-Z_][a-zA-Z0-9_]*      {return IDENT; }

\n      {linha_erro++;}
[ \t\r]+ {}

.       {fprintf(stderr, "erro na linha %d: '%s'\n", linha_erro, yytext);}

%%

int main(void) {
    int token;
    while ((token = yylex()) != 0) {
        printf("Token: %d | Lexema: '%s' | Linha: %d\n", token, yytext, linha_erro);
    }
    return 0;
}

//utilizar gcc lex.yy.c -o teste para rodar essa iteração mockada


