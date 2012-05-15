%{

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ailp.h"

#define YYPARSE_PARAM ailcompiler
#define MyCompiler ((AilCompiler *)ailcompiler)

#define EmitOp(op) AdrenoEmit_EmitOp(MyCompiler->CurrentFunction, (unsigned char)((op) | MyCompiler->Prefix))
#define EmitOp2(op, val) AdrenoEmit_EmitOp2_I4(MyCompiler->CurrentFunction, (unsigned char)((op) | MyCompiler->Prefix), (val))
#define EmitJump(op, name) AdrenoEmit_EmitJump(MyCompiler->CurrentFunction, (unsigned char)((op) | MyCompiler->Prefix), (name))
#define SetPrefix(prefix) MyCompiler->Prefix = (prefix)
#define StartFunction(name) MyCompiler->CurrentFunction = AdrenoEmit_CreateFunction(MyCompiler->Script, (name))
#define SetLabel(name) AdrenoEmit_SetLabel(MyCompiler->CurrentFunction, (name))
#define AddString(string) AdrenoEmit_AddString(MyCompiler->Script, (string), strlen((string)))
#define SetLocalCount(count) MyCompiler->CurrentFunction->LocalsCount = (count)
#define SetArgCount(count) MyCompiler->CurrentFunction->ArgumentCount = (count)
#define FinalizeFunction() AdrenoEmit_Finalize(MyCompiler->CurrentFunction)

#define YYMALLOC AdrenoAlloc
#define YYFREE AdrenoFree

%}

%union {
	unsigned int num;
	char *str;
}

%token <str> T_IDENTIFIER T_STRINGLITERAL
%token <num> T_CONSTANT
%token T_FUNCTION

%token T_INITLOCALS T_INITARGS

%token T_NOP
%token T_POP T_POP_S
%token T_STLOC_0 T_STLOC_1 T_STLOC_2 T_STLOC_3 T_STLOC_S
%token T_LDLOC_0 T_LDLOC_1 T_LDLOC_2 T_LDLOC_3 T_LDLOC_S
%token T_LDARG_0 T_LDARG_1 T_LDARG_2 T_LDARG_3 T_LDARG_S
%token T_LDNULL T_LDC_I4 T_LDC_I4_M1 T_LDC_I4_0 T_LDC_I4_1 T_LDSTR
%token T_NEWARR T_STELEM T_LDELEM T_RMELEM T_LDLEN
%token T_ADD T_SUB T_MUL T_DIV T_REM T_NEG
%token T_OR T_AND T_XOR T_NOT T_SHL T_SHR
%token T_LOR T_LAND T_LNOT T_EQ T_NE T_GT T_GE T_LT T_LE
%token T_JUMP T_BRTRUE T_BRFALSE T_SWITCH
%token T_LDFUNC T_CALL T_ENTER T_RET

%token T_REF

%error-verbose

%start start

%%

start		: functions
			;

functions	: 
			| functions function
			;

function	: T_FUNCTION block
			;

block		: fstart statements fend
			;

fstart		: T_IDENTIFIER '{'									{ StartFunction($1); AdrenoFree($1); }
			;

fend		: '}'												{ FinalizeFunction(); }
			;
		
statements	: 
			| statements statement
			;
			
statement	: label
			| fopcode ';'
			| fopcode
			;
			
label		: T_IDENTIFIER ':'									{ SetLabel($1); AdrenoFree($1); }
			;

fopcode		: mopcode
			| prefix opcode
			;
			
mopcode		: T_INITLOCALS T_CONSTANT							{ SetLocalCount($2); }
			| T_INITARGS T_CONSTANT								{ SetArgCount($2); }
			;

prefix		: 													{ SetPrefix(P_NONE); }
			| T_REF 											{ SetPrefix(P_REFERENCE); }
			;

opcode		: T_NOP												{ EmitOp(OP_NOP); }
			| T_POP												{ EmitOp(OP_POP); }
			| T_POP_S T_CONSTANT								{ EmitOp2(OP_POP_S, $2); }
			
			| T_STLOC_0											{ EmitOp(OP_STLOC_0); }
			| T_STLOC_1											{ EmitOp(OP_STLOC_1); }
			| T_STLOC_2											{ EmitOp(OP_STLOC_2); }
			| T_STLOC_3											{ EmitOp(OP_STLOC_3); }
			| T_STLOC_S T_CONSTANT								{ EmitOp2(OP_STLOC_S, $2); }
			
			| T_LDLOC_0											{ EmitOp(OP_LDLOC_0); }
			| T_LDLOC_1											{ EmitOp(OP_LDLOC_1); }
			| T_LDLOC_2											{ EmitOp(OP_LDLOC_2); }
			| T_LDLOC_3											{ EmitOp(OP_LDLOC_3); }
			| T_LDLOC_S T_CONSTANT								{ EmitOp2(OP_LDLOC_S, $2); }
			
			| T_LDARG_0											{ EmitOp(OP_LDARG_0); }
			| T_LDARG_1											{ EmitOp(OP_LDARG_1); }
			| T_LDARG_2											{ EmitOp(OP_LDARG_2); }
			| T_LDARG_3											{ EmitOp(OP_LDARG_3); }
			| T_LDARG_S T_CONSTANT								{ EmitOp2(OP_LDARG_S, $2); }

			| T_LDNULL											{ EmitOp(OP_LDNULL); }
			| T_LDC_I4 T_CONSTANT								{ EmitOp2(OP_LDC_I4, $2); }
			| T_LDC_I4_M1										{ EmitOp(OP_LDC_I4_M1); }
			| T_LDC_I4_0										{ EmitOp(OP_LDC_I4_0); }
			| T_LDC_I4_1										{ EmitOp(OP_LDC_I4_1); }
			| T_LDSTR T_STRINGLITERAL							{ EmitOp2(OP_LDSTR, AddString($2)); AdrenoFree($2); }
			
			| T_NEWARR											{ EmitOp(OP_NEWARR); }
			| T_STELEM											{ EmitOp(OP_STELEM); }
			| T_LDELEM											{ EmitOp(OP_LDELEM); }
			| T_RMELEM											{ EmitOp(OP_RMELEM); }
			| T_LDLEN											{ EmitOp(OP_LDLEN); }
			
			| T_ADD												{ EmitOp(OP_ADD); }
			| T_SUB												{ EmitOp(OP_SUB); }
			| T_MUL												{ EmitOp(OP_MUL); }
			| T_DIV												{ EmitOp(OP_DIV); }
			| T_REM												{ EmitOp(OP_REM); }
			| T_NEG												{ EmitOp(OP_NEG); }

			| T_OR												{ EmitOp(OP_OR); }
			| T_AND												{ EmitOp(OP_AND); }
			| T_XOR												{ EmitOp(OP_XOR); }
			| T_NOT												{ EmitOp(OP_NOT); }
			| T_SHL												{ EmitOp(OP_SHL); }
			| T_SHR												{ EmitOp(OP_SHR); }

			| T_LOR												{ EmitOp(OP_LOR); }
			| T_LAND											{ EmitOp(OP_LAND); }
			| T_LNOT											{ EmitOp(OP_LNOT); }
			| T_EQ												{ EmitOp(OP_EQ); }
			| T_NE												{ EmitOp(OP_NE); }
			| T_GT												{ EmitOp(OP_GT); }
			| T_GE												{ EmitOp(OP_GE); }
			| T_LT												{ EmitOp(OP_LT); }
			| T_LE												{ EmitOp(OP_LE); }
			
			| T_JUMP T_IDENTIFIER								{ EmitJump(OP_JUMP, $2); AdrenoFree($2); }
			| T_BRTRUE T_IDENTIFIER								{ EmitJump(OP_BRTRUE, $2); AdrenoFree($2); }
			| T_BRFALSE T_IDENTIFIER							{ EmitJump(OP_BRFALSE, $2); AdrenoFree($2); }						
			| T_SWITCH											{ EmitOp(OP_SWITCH); }

			| T_LDFUNC T_STRINGLITERAL							{ EmitOp2(OP_LDFUNC, AddString($2)); AdrenoFree($2); }
			| T_CALL											{ EmitOp(OP_CALL); }
			| T_ENTER											{ EmitOp(OP_ENTER); }
			| T_RET												{ EmitOp(OP_RET); }
			;
