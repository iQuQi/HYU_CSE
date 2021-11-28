/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IFX = 258,
    ELSE = 259,
    IF = 260,
    WHILE = 261,
    RETURN = 262,
    INT = 263,
    VOID = 264,
    ID = 265,
    NUM = 266,
    COMMA = 267,
    ASSIGN = 268,
    EQ = 269,
    NE = 270,
    LT = 271,
    LE = 272,
    GT = 273,
    GE = 274,
    PLUS = 275,
    MINUS = 276,
    TIMES = 277,
    OVER = 278,
    LPAREN = 279,
    RPAREN = 280,
    LBRACE = 281,
    RBRACE = 282,
    LCURLY = 283,
    RCURLY = 284,
    SEMI = 285,
    ERROR = 286,
    ENDFILE = 287
  };
#endif
/* Tokens.  */
#define IFX 258
#define ELSE 259
#define IF 260
#define WHILE 261
#define RETURN 262
#define INT 263
#define VOID 264
#define ID 265
#define NUM 266
#define COMMA 267
#define ASSIGN 268
#define EQ 269
#define NE 270
#define LT 271
#define LE 272
#define GT 273
#define GE 274
#define PLUS 275
#define MINUS 276
#define TIMES 277
#define OVER 278
#define LPAREN 279
#define RPAREN 280
#define LBRACE 281
#define RBRACE 282
#define LCURLY 283
#define RCURLY 284
#define SEMI 285
#define ERROR 286
#define ENDFILE 287

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
