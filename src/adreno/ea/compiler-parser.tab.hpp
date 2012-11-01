/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     T_EOF = 0,
     T_INT = 258,
     T_EQU = 259,
     T_NEQ = 260,
     T_AND = 261,
     T_OR = 262,
     T_STR = 263,
     T_IDENT = 264,
     T_IF = 265,
     T_ELSE = 266,
     T_FOR = 267,
     T_SWITCH = 268,
     T_CASE = 269,
     T_DEFAULT = 270,
     T_WHILE = 271,
     T_DO = 272,
     T_MENU = 273,
     T_GLOBAL_TMP = 274,
     T_EOL = 275
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 18 "ea/compiler-parser.y"

   int ival;
   char* sval;

   EANode* node;
   EABlock* block;
   EAStatement* stmt;
   EAExpression* expr;
   EALabel* label;
   EAMenu* menu;
   EAMenuOptions* menu_opts;
   EASwitchCases* switch_cases;
   EASwitchCase* switch_case;
   EAIfStatement* if_stmt;
   std::vector<EAExpression*>* expr_list;




/* Line 2068 of yacc.c  */
#line 91 "/home/aaron/dev/libadreno/src/adreno/ea/compiler-parser.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



