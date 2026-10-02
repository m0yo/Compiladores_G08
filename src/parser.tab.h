/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_INT = 258,                   /* T_INT  */
    T_FLOAT = 259,                 /* T_FLOAT  */
    T_CHAR = 260,                  /* T_CHAR  */
    T_DOUBLE = 261,                /* T_DOUBLE  */
    T_LONG = 262,                  /* T_LONG  */
    F_IF = 263,                    /* F_IF  */
    F_ELSE = 264,                  /* F_ELSE  */
    F_WHILE = 265,                 /* F_WHILE  */
    F_FOR = 266,                   /* F_FOR  */
    F_DO = 267,                    /* F_DO  */
    F_SWITCH = 268,                /* F_SWITCH  */
    F_CASE = 269,                  /* F_CASE  */
    F_BREAK = 270,                 /* F_BREAK  */
    F_CONTINUE = 271,              /* F_CONTINUE  */
    F_RETURN = 272,                /* F_RETURN  */
    L_INT = 273,                   /* L_INT  */
    L_FLOAT = 274,                 /* L_FLOAT  */
    L_CHAR = 275,                  /* L_CHAR  */
    L_STRING = 276,                /* L_STRING  */
    IDENT = 277,                   /* IDENT  */
    O_PLUS = 278,                  /* O_PLUS  */
    O_MINUS = 279,                 /* O_MINUS  */
    O_MULTI = 280,                 /* O_MULTI  */
    O_DIV = 281,                   /* O_DIV  */
    O_ASSIGN = 282,                /* O_ASSIGN  */
    R_EQ = 283,                    /* R_EQ  */
    R_NE = 284,                    /* R_NE  */
    R_LT = 285,                    /* R_LT  */
    R_GT = 286,                    /* R_GT  */
    R_LE = 287,                    /* R_LE  */
    R_GE = 288,                    /* R_GE  */
    DM_AND = 289,                  /* DM_AND  */
    DM_OR = 290,                   /* DM_OR  */
    DM_NOT = 291,                  /* DM_NOT  */
    LBRACE = 292,                  /* LBRACE  */
    RBRACE = 293,                  /* RBRACE  */
    LPAREN = 294,                  /* LPAREN  */
    RPAREN = 295,                  /* RPAREN  */
    LBRACKET = 296,                /* LBRACKET  */
    RBRACKET = 297,                /* RBRACKET  */
    COLON = 298,                   /* COLON  */
    SEMICOLON = 299,               /* SEMICOLON  */
    COMMA = 300,                   /* COMMA  */
    UMINUS = 301                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
