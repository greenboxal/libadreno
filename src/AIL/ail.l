D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]

%{

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ailp.h"
#include "y.tab.h"

void ail_comment();

%}

%option case-sensitive

%%

"/*"					{ ail_comment(); }

"function"				{ return T_FUNCTION; }

".initlocals"			{ return T_INITLOCALS; }
".initargs"				{ return T_INITARGS; }

"nop"					{ return T_NOP; }
"pop"					{ return T_POP; }
"pop.s"					{ return T_POP_S; }

"stloc.0"				{ return T_STLOC_0; }
"stloc.1"				{ return T_STLOC_1; }
"stloc.2"				{ return T_STLOC_2; }
"stloc.3"				{ return T_STLOC_3; }
"stloc.s"				{ return T_STLOC_S; }

"ldloc.0"				{ return T_LDLOC_0; }
"ldloc.1"				{ return T_LDLOC_1; } 
"ldloc.2"				{ return T_LDLOC_2; }
"ldloc.3"				{ return T_LDLOC_3; }
"ldloc.s"				{ return T_LDLOC_S; }

"ldarg.0"				{ return T_LDARG_0; }
"ldarg.1"				{ return T_LDARG_1; }
"ldarg.2"				{ return T_LDARG_2; }
"ldarg.3"				{ return T_LDARG_3; }
"ldarg.s"				{ return T_LDARG_S; }

"ldnull"				{ return T_LDNULL; }
"ldc.i4"				{ return T_LDC_I4; }
"ldc.i4.m1"				{ return T_LDC_I4_M1; }
"ldc.i4.0"				{ return T_LDC_I4_0; }
"ldc.i4.1"				{ return T_LDC_I4_1; }
"ldstr"					{ return T_LDSTR; }

"add"					{ return T_ADD; }
"sub"					{ return T_SUB; }
"mul"					{ return T_MUL; }
"div"					{ return T_DIV; }
"rem"					{ return T_REM; }
"neg"					{ return T_NEG; }

"or"					{ return T_OR; }
"and"					{ return T_AND; }
"xor"					{ return T_XOR; }
"not"					{ return T_NOT; }
"shl"					{ return T_SHL; }
"shr"					{ return T_SHR; }

"lor"					{ return T_LOR; }
"land"					{ return T_LAND; }
"lnot"					{ return T_LNOT; }
"eq"					{ return T_EQ; }
"ne"					{ return T_NE; }
"gt"					{ return T_GT; }
"ge"					{ return T_GE; }
"lt"					{ return T_LT; }
"le"					{ return T_LE; }

"jump"					{ return T_JUMP; }
"brtrue"				{ return T_BRTRUE; }
"brfalse"				{ return T_BRFALSE; }
"switch"				{ return T_SWITCH; }

"ldfunc"				{ return T_LDFUNC; }
"call"					{ return T_CALL; }
"enter"					{ return T_ENTER; }
"ret"					{ return T_RET; }

"ref"					{ return T_REF; }

\"(\\.|[^\\"])*\"		{ 
							ail_lval.str = ail_parse_string(yytext);
							return T_STRINGLITERAL; 
						}
						
{L}({L}|{D})*			{ 
							ail_lval.str = AdrenoStrdup(yytext); 
							return T_IDENTIFIER; 
						}
						
0[xX]{H}+				{ 
							ail_lval.num = strtoul(yytext, NULL, 0);
							return T_CONSTANT; 
						}
0{D}+					{ 
							ail_lval.num = strtoul(yytext, NULL, 0);
							return T_CONSTANT; 
						}
-?{D}+					{ 
							ail_lval.num = strtoul(yytext, NULL, 0);
							return T_CONSTANT; 
						}

[ \t\v\r\n\f]			{ }
.						{ return yytext[0]; }

%%

void ail_error(const char* errmsg)
{
	fprintf(yyout, "\n*** Error: %s\n", errmsg);
}
 
int ail_wrap() 
{
	return 1; 
}

void ail_comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
		putchar(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}

	if (c != 0)
		putchar(c1);
}

char *ail_parse_string(char *str)
{
	int i = 0, j = 0, maxLen = strlen(str);
	char *ret = (char *)AdrenoAlloc(maxLen);

	for (i = 1; i < maxLen - 1; i++)
	{
		if (str[i] == '\\')
		{
			i++;

			if (str[i] == '\\')
				ret[j] = '\\';
			else if (str[i] == 'r')
				ret[j] = '\r';
			else if (str[i] == 'n')
				ret[j] = '\n';
			else if (str[i] == 't')
				ret[j] = '\t';
			else if (str[i] == 'b')
				ret[j] = '\b';
			else if (str[i] == '"')
				ret[j] = '"';
			else if (str[i] == '\'')
				ret[j] = '\'';
			else
			{
				ret[j++] = '\\';
				ret[j] = str[i];
			}
		}
		else
		{
			ret[j] = str[i];
		}

		j++;
	}

	ret[j] = 0;

	return ret;
}

static YY_BUFFER_STATE ail_buffState;
void ail_setup_buffer(char *buffer)
{
	ail_buffState = yy_scan_string(buffer);
	yy_switch_to_buffer(ail_buffState);
}

void ail_free_buffer()
{
	yy_delete_buffer(ail_buffState);
}
