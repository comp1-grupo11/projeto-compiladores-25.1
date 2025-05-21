/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PP_INCLUDE = 258,
     PP_DEFINE = 259,
     IDENTIFIER = 260,
     STRING_LITERAL = 261,
     CHAR_LITERAL = 262,
     INT_LITERAL = 263,
     FLOAT_LITERAL = 264,
     KW_AUTO = 265,
     KW_BREAK = 266,
     KW_CASE = 267,
     KW_CHAR = 268,
     KW_CONST = 269,
     KW_CONTINUE = 270,
     KW_DEFAULT = 271,
     KW_DO = 272,
     KW_ELSE = 273,
     KW_ENUM = 274,
     KW_EXTERN = 275,
     KW_FOR = 276,
     KW_IF = 277,
     KW_INT = 278,
     KW_LONG = 279,
     KW_RETURN = 280,
     KW_SHORT = 281,
     KW_SIGNED = 282,
     KW_STATIC = 283,
     KW_SWITCH = 284,
     KW_TYPEDEF = 285,
     KW_UNSIGNED = 286,
     KW_VOID = 287,
     KW_VOLATILE = 288,
     KW_WHILE = 289,
     TYPE_INT = 290,
     TYPE_CHAR = 291,
     TYPE_FLOAT = 292,
     TYPE_DOUBLE = 293,
     TYPE_VOID = 294,
     OP_ADD = 295,
     OP_SUB = 296,
     OP_MUL = 297,
     OP_DIV = 298,
     OP_ASSIGN = 299,
     OP_EQ = 300,
     OP_NE = 301,
     OP_LT = 302,
     OP_LE = 303,
     OP_GT = 304,
     OP_GE = 305,
     OP_AND = 306,
     OP_OR = 307,
     OP_NOT = 308,
     OP_INC = 309,
     OP_DEC = 310,
     OP_ADD_ASSIGN = 311,
     OP_SUB_ASSIGN = 312,
     OP_MUL_ASSIGN = 313,
     OP_DIV_ASSIGN = 314,
     LPAREN = 315,
     RPAREN = 316,
     LBRACE = 317,
     RBRACE = 318,
     LBRACKET = 319,
     RBRACKET = 320,
     SEMICOLON = 321,
     COMMA = 322,
     DOT = 323,
     COLON = 324,
     OP_TERNARY = 325
   };
#endif
/* Tokens.  */
#define PP_INCLUDE 258
#define PP_DEFINE 259
#define IDENTIFIER 260
#define STRING_LITERAL 261
#define CHAR_LITERAL 262
#define INT_LITERAL 263
#define FLOAT_LITERAL 264
#define KW_AUTO 265
#define KW_BREAK 266
#define KW_CASE 267
#define KW_CHAR 268
#define KW_CONST 269
#define KW_CONTINUE 270
#define KW_DEFAULT 271
#define KW_DO 272
#define KW_ELSE 273
#define KW_ENUM 274
#define KW_EXTERN 275
#define KW_FOR 276
#define KW_IF 277
#define KW_INT 278
#define KW_LONG 279
#define KW_RETURN 280
#define KW_SHORT 281
#define KW_SIGNED 282
#define KW_STATIC 283
#define KW_SWITCH 284
#define KW_TYPEDEF 285
#define KW_UNSIGNED 286
#define KW_VOID 287
#define KW_VOLATILE 288
#define KW_WHILE 289
#define TYPE_INT 290
#define TYPE_CHAR 291
#define TYPE_FLOAT 292
#define TYPE_DOUBLE 293
#define TYPE_VOID 294
#define OP_ADD 295
#define OP_SUB 296
#define OP_MUL 297
#define OP_DIV 298
#define OP_ASSIGN 299
#define OP_EQ 300
#define OP_NE 301
#define OP_LT 302
#define OP_LE 303
#define OP_GT 304
#define OP_GE 305
#define OP_AND 306
#define OP_OR 307
#define OP_NOT 308
#define OP_INC 309
#define OP_DEC 310
#define OP_ADD_ASSIGN 311
#define OP_SUB_ASSIGN 312
#define OP_MUL_ASSIGN 313
#define OP_DIV_ASSIGN 314
#define LPAREN 315
#define RPAREN 316
#define LBRACE 317
#define RBRACE 318
#define LBRACKET 319
#define RBRACKET 320
#define SEMICOLON 321
#define COMMA 322
#define DOT 323
#define COLON 324
#define OP_TERNARY 325




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 16 "parser.y"
{
    char* str_val;
    int int_val;
    float float_val;
    char* code;
}
/* Line 1529 of yacc.c.  */
#line 196 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

