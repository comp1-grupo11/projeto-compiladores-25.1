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
    PP_INCLUDE = 258,              /* PP_INCLUDE  */
    PP_DEFINE = 259,               /* PP_DEFINE  */
    IDENTIFIER = 260,              /* IDENTIFIER  */
    STRING_LITERAL = 261,          /* STRING_LITERAL  */
    CHAR_LITERAL = 262,            /* CHAR_LITERAL  */
    INT_LITERAL = 263,             /* INT_LITERAL  */
    FLOAT_LITERAL = 264,           /* FLOAT_LITERAL  */
    KW_AUTO = 265,                 /* KW_AUTO  */
    KW_BREAK = 266,                /* KW_BREAK  */
    KW_CASE = 267,                 /* KW_CASE  */
    KW_CHAR = 268,                 /* KW_CHAR  */
    KW_CONST = 269,                /* KW_CONST  */
    KW_CONTINUE = 270,             /* KW_CONTINUE  */
    KW_DEFAULT = 271,              /* KW_DEFAULT  */
    KW_DO = 272,                   /* KW_DO  */
    KW_ELSE = 273,                 /* KW_ELSE  */
    KW_ENUM = 274,                 /* KW_ENUM  */
    KW_EXTERN = 275,               /* KW_EXTERN  */
    KW_FOR = 276,                  /* KW_FOR  */
    KW_IF = 277,                   /* KW_IF  */
    KW_INT = 278,                  /* KW_INT  */
    KW_LONG = 279,                 /* KW_LONG  */
    KW_RETURN = 280,               /* KW_RETURN  */
    KW_SHORT = 281,                /* KW_SHORT  */
    KW_SIGNED = 282,               /* KW_SIGNED  */
    KW_STATIC = 283,               /* KW_STATIC  */
    KW_SWITCH = 284,               /* KW_SWITCH  */
    KW_TYPEDEF = 285,              /* KW_TYPEDEF  */
    KW_UNSIGNED = 286,             /* KW_UNSIGNED  */
    KW_VOID = 287,                 /* KW_VOID  */
    KW_VOLATILE = 288,             /* KW_VOLATILE  */
    KW_WHILE = 289,                /* KW_WHILE  */
    TYPE_INT = 290,                /* TYPE_INT  */
    TYPE_CHAR = 291,               /* TYPE_CHAR  */
    TYPE_FLOAT = 292,              /* TYPE_FLOAT  */
    TYPE_DOUBLE = 293,             /* TYPE_DOUBLE  */
    TYPE_VOID = 294,               /* TYPE_VOID  */
    OP_ADD = 295,                  /* OP_ADD  */
    OP_SUB = 296,                  /* OP_SUB  */
    OP_MUL = 297,                  /* OP_MUL  */
    OP_DIV = 298,                  /* OP_DIV  */
    OP_ASSIGN = 299,               /* OP_ASSIGN  */
    OP_EQ = 300,                   /* OP_EQ  */
    OP_NE = 301,                   /* OP_NE  */
    OP_LT = 302,                   /* OP_LT  */
    OP_LE = 303,                   /* OP_LE  */
    OP_GT = 304,                   /* OP_GT  */
    OP_GE = 305,                   /* OP_GE  */
    OP_AND = 306,                  /* OP_AND  */
    OP_OR = 307,                   /* OP_OR  */
    OP_NOT = 308,                  /* OP_NOT  */
    OP_INC = 309,                  /* OP_INC  */
    OP_DEC = 310,                  /* OP_DEC  */
    OP_ADD_ASSIGN = 311,           /* OP_ADD_ASSIGN  */
    OP_SUB_ASSIGN = 312,           /* OP_SUB_ASSIGN  */
    OP_MUL_ASSIGN = 313,           /* OP_MUL_ASSIGN  */
    OP_DIV_ASSIGN = 314,           /* OP_DIV_ASSIGN  */
    LPAREN = 315,                  /* LPAREN  */
    RPAREN = 316,                  /* RPAREN  */
    LBRACE = 317,                  /* LBRACE  */
    RBRACE = 318,                  /* RBRACE  */
    LBRACKET = 319,                /* LBRACKET  */
    RBRACKET = 320,                /* RBRACKET  */
    SEMICOLON = 321,               /* SEMICOLON  */
    COMMA = 322,                   /* COMMA  */
    DOT = 323,                     /* DOT  */
    COLON = 324,                   /* COLON  */
    OP_TERNARY = 325               /* OP_TERNARY  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "parser.y"

    char* str_val;
    int int_val;
    float float_val;

#line 140 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
