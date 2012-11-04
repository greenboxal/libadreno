%option reentrant
%option prefix="AILC_"
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno
%option never-interactive
%option case-sensitive

%{
/*	This file is part of libadreno.

    libadreno is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libadreno is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ail-parser.tab.hpp"
#include <adreno/ail/ail.h>

using namespace Adreno;

#define YY_EXTRA_TYPE Adreno::AilParserContext*
#define YY_USER_ACTION yylloc->first_line = yylineno;
#define YY_INPUT(buf, result, max_size)  { \
  char c;                                  \
  yyextra->Input() >> c;                   \
  if( yyextra->Input().eof() )             \
	 result = YY_NULL;                     \
  else {                                   \
	 buf[ 0 ] = c;                         \
	 result = 1;                           \
  }                                        \
}
%}

D [0-9]
L [A-Za-z_]
H [A-Fa-f0-9]

%s IN_COMMENT

%%

<INITIAL>{
"/*"                 BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"                 BEGIN(INITIAL);
[^*\n]+
"*"
}

":"						{ return ':'; }
";"						{ return ';'; }
"{"						{ return '{'; }
"}"						{ return '}'; }

"def"					{ return T_DEF; }

"nop"					{ return T_NOP; }

"pop"					{ return T_POP; }
"pop.s"					{ return T_POP_S; }

"ldnull"				{ return T_LDNULL; }
"ldnum"					{ return T_LDNUM; }
"ldnum.m1"				{ return T_LDNUM_M1; }
"ldnum.0"				{ return T_LDNUM_0; }
"ldnum.1"				{ return T_LDNUM_1; }
"ldfloat"				{ return T_LDFLOAT; }
"ldstr"					{ return T_LDSTR; }
"ldhash"				{ return T_LDHASH; }
"ldglob"				{ return T_LDGLOB; }
"ldcls"					{ return T_LDCLS; }
"ldtrue"				{ return T_LDTRUE; }
"ldfalse"				{ return T_LDFALSE; }

"ldarg.0"				{ return T_LDARG_0; }
"ldarg.1"				{ return T_LDARG_1; }
"ldarg.2"				{ return T_LDARG_2; }
"ldarg.3"				{ return T_LDARG_3; }
"ldarg.s"				{ return T_LDARG_S; }

"ldloc.0"				{ return T_LDLOC_0; }
"ldloc.1"				{ return T_LDLOC_1; } 
"ldloc.2"				{ return T_LDLOC_2; }
"ldloc.3"				{ return T_LDLOC_3; }
"ldloc.s"				{ return T_LDLOC_S; }

"stloc.0"				{ return T_STLOC_0; }
"stloc.1"				{ return T_STLOC_1; }
"stloc.2"				{ return T_STLOC_2; }
"stloc.3"				{ return T_STLOC_3; }
"stloc.s"				{ return T_STLOC_S; }

"new"					{ return T_NEW; }

"add"					{ return T_ADD; }
"sub"					{ return T_SUB; }
"mul"					{ return T_MUL; }
"div"					{ return T_DIV; }
"rem"					{ return T_REM; }
"neg"					{ return T_NEG; }

"and"					{ return T_AND; }
"or"					{ return T_OR; }
"xor"					{ return T_XOR; }
"not"					{ return T_NOT; }
"shr"					{ return T_SHR; }
"shl"					{ return T_SHL; }

"land"					{ return T_LAND; }
"lor"					{ return T_LOR; }
"lnot"					{ return T_LNOT; }

"eq"					{ return T_EQ; }
"ne"					{ return T_NE; }
"gt"					{ return T_GT; }
"ge"					{ return T_GE; }
"lt"					{ return T_LT; }
"le"					{ return T_LE; }

"call"					{ return T_CALL; }
"ret"					{ return T_RET; }

"jump"					{ return T_JUMP; }
"brtrue"				{ return T_BRTRUE; }
"brfalse"				{ return T_BRFALSE; }

0[Xx]{H}+            	{ yylval->ival = strtol(yytext, NULL, 16); return T_INTEGER; }
-?{D}+                 	{ yylval->ival = strtol(yytext, NULL, 10); return T_INTEGER; }
\"[^"]*\"            	{ yylval->sval = strdup(yytext); return T_STRING; }
{L}({L}|{D})*			{ yylval->sval = strdup(yytext); return T_IDENTIFIER; }
\/\/.*$              	; // Ignore comments
(\t\ )               	; // Ignore whitespace
\r?\n?               	; // Ignore newlines
.                    	;

%%

AilParserContext::AilParserContext(std::istream &input)
	: _Input(input)
{
	yylex_init(&_Scanner);
	yyset_extra(this, _Scanner);

	_DebugInfo = false;
	_Assembly = new AssemblyBuilder();
}

AilParserContext::~AilParserContext()
{
	yylex_destroy(_Scanner);
}

