
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_IDENTIFIER = 258,
     T_STRINGLITERAL = 259,
     T_CONSTANT = 260,
     T_FUNCTION = 261,
     T_INITLOCALS = 262,
     T_INITARGS = 263,
     T_NOP = 264,
     T_POP = 265,
     T_STLOC_0 = 266,
     T_STLOC_1 = 267,
     T_STLOC_2 = 268,
     T_STLOC_3 = 269,
     T_STLOC_S = 270,
     T_LDLOC_0 = 271,
     T_LDLOC_1 = 272,
     T_LDLOC_2 = 273,
     T_LDLOC_3 = 274,
     T_LDLOC_S = 275,
     T_LDARG_0 = 276,
     T_LDARG_1 = 277,
     T_LDARG_2 = 278,
     T_LDARG_3 = 279,
     T_LDARG_S = 280,
     T_LDNULL = 281,
     T_LDC_I4 = 282,
     T_LDC_I4_M1 = 283,
     T_LDC_I4_0 = 284,
     T_LDC_I4_1 = 285,
     T_LDSTR = 286,
     T_NEWARR = 287,
     T_STELEM = 288,
     T_LDELEM = 289,
     T_RMELEM = 290,
     T_LDLEN = 291,
     T_ADD = 292,
     T_SUB = 293,
     T_MUL = 294,
     T_DIV = 295,
     T_REM = 296,
     T_NEG = 297,
     T_OR = 298,
     T_AND = 299,
     T_XOR = 300,
     T_NOT = 301,
     T_SHL = 302,
     T_SHR = 303,
     T_LOR = 304,
     T_LAND = 305,
     T_LNOT = 306,
     T_EQ = 307,
     T_NE = 308,
     T_GT = 309,
     T_GE = 310,
     T_LT = 311,
     T_LE = 312,
     T_JUMP = 313,
     T_BRTRUE = 314,
     T_BRFALSE = 315,
     T_SWITCH = 316,
     T_LDFUNC = 317,
     T_CALL = 318,
     T_ENTER = 319,
     T_RET = 320,
     T_REF = 321
   };
#endif
/* Tokens.  */
#define T_IDENTIFIER 258
#define T_STRINGLITERAL 259
#define T_CONSTANT 260
#define T_FUNCTION 261
#define T_INITLOCALS 262
#define T_INITARGS 263
#define T_NOP 264
#define T_POP 265
#define T_STLOC_0 266
#define T_STLOC_1 267
#define T_STLOC_2 268
#define T_STLOC_3 269
#define T_STLOC_S 270
#define T_LDLOC_0 271
#define T_LDLOC_1 272
#define T_LDLOC_2 273
#define T_LDLOC_3 274
#define T_LDLOC_S 275
#define T_LDARG_0 276
#define T_LDARG_1 277
#define T_LDARG_2 278
#define T_LDARG_3 279
#define T_LDARG_S 280
#define T_LDNULL 281
#define T_LDC_I4 282
#define T_LDC_I4_M1 283
#define T_LDC_I4_0 284
#define T_LDC_I4_1 285
#define T_LDSTR 286
#define T_NEWARR 287
#define T_STELEM 288
#define T_LDELEM 289
#define T_RMELEM 290
#define T_LDLEN 291
#define T_ADD 292
#define T_SUB 293
#define T_MUL 294
#define T_DIV 295
#define T_REM 296
#define T_NEG 297
#define T_OR 298
#define T_AND 299
#define T_XOR 300
#define T_NOT 301
#define T_SHL 302
#define T_SHR 303
#define T_LOR 304
#define T_LAND 305
#define T_LNOT 306
#define T_EQ 307
#define T_NE 308
#define T_GT 309
#define T_GE 310
#define T_LT 311
#define T_LE 312
#define T_JUMP 313
#define T_BRTRUE 314
#define T_BRFALSE 315
#define T_SWITCH 316
#define T_LDFUNC 317
#define T_CALL 318
#define T_ENTER 319
#define T_RET 320
#define T_REF 321




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 28 "ail.y"

	unsigned int num;
	char *str;



/* Line 1676 of yacc.c  */
#line 191 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE ail_lval;


