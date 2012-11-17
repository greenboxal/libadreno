%pure-parser
%name-prefix="AILC_"
%error-verbose
%locations
%defines
%parse-param { Adreno::AilParserContext *context }
%lex-param { void *scanner }
%debug

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

#include <adreno/ail/ail.h>

namespace Adreno
{
	class APC_Detail
	{
	public:
		static void StartFunction(AilParserContext *me, const String &name)
		{
			me->_CurrentFunction = new FunctionEmitter(name);
		}

		static void EndFunction(AilParserContext *me)
		{
			me->_CurrentFunction->Finish();
			me->_Assembly.AddFunction(me->_CurrentFunction);
			me->_CurrentFunction = nullptr;
			me->_Labels.clear();
		}

		static void EmitOp(AilParserContext *me, unsigned char x)
		{
			me->_CurrentFunction->EmitOp(x);
		}

		static void EmitOp2(AilParserContext *me, unsigned char x, int y)
		{
			me->_CurrentFunction->EmitOp2(x, y);
		}

		static void EmitOp2(AilParserContext *me, unsigned char x, Label *y)
		{
			me->_CurrentFunction->EmitOp2(x, y);
		}

		static void EmitDOp2(AilParserContext *me, unsigned char x, double y)
		{
			me->_CurrentFunction->EmitDOp2(x, y);
		}

		static Label *FindLabel(AilParserContext *me, const String &name)
		{
			std::unordered_map<String, Label *>::iterator it = me->_Labels.find(name);

			if (it == me->_Labels.end())
			{
				Label *lbl = me->_CurrentFunction->CreateLabel();
				me->_Labels[name] = lbl;
				return lbl;
			}

			return it->second;
		}

		static void AddLabel(AilParserContext *me, const String &name)
		{
			FindLabel(me, name)->Bind();
		}

		static void AddString(AilParserContext *me, const String &str)
		{
			me->_Assembly.AddToStringPool(str);
		}

		static void AddDebugString(AilParserContext *me, const String &str)
		{
			if (me->_DebugInfo)
				me->_Assembly.AddToStringPool(str);
		}
	};
}

using namespace Adreno;

#define APC_StartFunction(x) APC_Detail::StartFunction(context, x)
#define APC_EndFunction() APC_Detail::EndFunction(context)
#define APC_EmitOp(x) APC_Detail::EmitOp(context, x)
#define APC_EmitOp2(x, y) APC_Detail::EmitOp2(context, x, y)
#define APC_EmitDOp2(x, y) APC_Detail::EmitDOp2(context, x, y)
#define APC_AddLabel(x) APC_Detail::AddLabel(context, x)
#define APC_FindLabel(x) APC_Detail::FindLabel(context, x)
#define APC_AddString(x) APC_Detail::AddString(context, x)
#define APC_AddDebugString(x) APC_Detail::AddDebugString(context, x)

%}

%union {
   int ival;
   double dval;
   char *sval;
}

%{
int AILC_lex(YYSTYPE *lvalp, YYLTYPE *llocp, void *scanner);

void AILC_error(YYLTYPE *locp, AilParserContext *context, const char *err) 
{
	std::cerr << locp->first_line << ":" << err << std::endl;
}

#define scanner context->Scanner()
%}

%token <ival> T_INTEGER
%token <dval> T_FLOAT
%token <sval> T_IDENTIFIER T_STRING

%token T_DEF T_END T_CLASS

%token T_NOP
%token T_POP T_POP_S
%token T_LDNULL T_LDNUM T_LDNUM_M1 T_LDNUM_0 T_LDNUM_1 T_LDFLOAT T_LDSTR T_LDHASH T_LDGLOB T_LDCLS T_LDTRUE T_LDFALSE
%token T_LDARG_0 T_LDARG_1 T_LDARG_2 T_LDARG_3 T_LDARG_S T_LDARGS
%token T_LDLOC_0 T_LDLOC_1 T_LDLOC_2 T_LDLOC_3 T_LDLOC_S
%token T_STLOC_0 T_STLOC_1 T_STLOC_2 T_STLOC_3 T_STLOC_S
%token T_NEW
%token T_ADD T_SUB T_MUL T_DIV T_REM T_NEG
%token T_AND T_OR T_XOR T_NOT T_SHR T_SHL
%token T_LAND T_LOR T_LNOT T_EQ T_NE T_GT T_GE T_LT T_LE
%token T_CALL T_RET
%token T_JUMP T_BRTRUE T_BRFALSE

%start unit

%%

unit		: functions
			;

functions	: 
			| functions function
			;

function	: fstart statements T_END							{ APC_EndFunction(); }
			;

fstart		: T_DEF T_IDENTIFIER								{ APC_StartFunction($2); free($2); }
			;

statements	: 
			| statements statement
			;
			
statement	: label
			| opcode ';'
			| opcode
			;
			
label		: T_IDENTIFIER ':'									{ APC_AddLabel($1); free($1); }
			;


opcode		: T_NOP												{ APC_EmitOp(Opcode::Nop); }
			| T_POP												{ APC_EmitOp(Opcode::Pop); }
			| T_POP_S T_INTEGER									{ APC_EmitOp2(Opcode::Pop_S, $2); }
			
			| T_LDNULL											{ APC_EmitOp(Opcode::Ldnull); }
			| T_LDNUM T_INTEGER									{ APC_EmitOp2(Opcode::Ldnum, $2); }
			| T_LDNUM_M1										{ APC_EmitOp(Opcode::Ldnum_M1); }
			| T_LDNUM_0											{ APC_EmitOp(Opcode::Ldnum_0); }
			| T_LDNUM_1											{ APC_EmitOp(Opcode::Ldnum_1); }
			| T_LDFLOAT	T_FLOAT									{ APC_EmitOp2(Opcode::Ldfloat, $2); }
			| T_LDSTR T_STRING									{ String str($2); APC_EmitOp2(Opcode::Ldstr, str.Hash()); APC_AddString(str); free($2); }
			| T_LDHASH T_STRING									{ String str($2); APC_EmitOp2(Opcode::Ldstr, str.Hash()); APC_AddDebugString(str); free($2); }
			| T_LDGLOB											{ APC_EmitOp(Opcode::Ldglob); }
			| T_LDTRUE											{ APC_EmitOp(Opcode::Ldtrue); }
			| T_LDFALSE											{ APC_EmitOp(Opcode::Ldfalse); }
			
			| T_LDARG_0											{ APC_EmitOp(Opcode::Ldarg_0); }
			| T_LDARG_1											{ APC_EmitOp(Opcode::Ldarg_1); }
			| T_LDARG_2											{ APC_EmitOp(Opcode::Ldarg_2); }
			| T_LDARG_3											{ APC_EmitOp(Opcode::Ldarg_3); }
			| T_LDARG_S T_INTEGER								{ APC_EmitOp2(Opcode::Ldarg_S, $2); }
			
			| T_LDLOC_0											{ APC_EmitOp(Opcode::Ldloc_0); }
			| T_LDLOC_1											{ APC_EmitOp(Opcode::Ldloc_1); }
			| T_LDLOC_2											{ APC_EmitOp(Opcode::Ldloc_2); }
			| T_LDLOC_3											{ APC_EmitOp(Opcode::Ldloc_3); }
			| T_LDLOC_S T_INTEGER								{ APC_EmitOp2(Opcode::Ldloc_S, $2); }
			
			| T_STLOC_0											{ APC_EmitOp(Opcode::Stloc_0); }
			| T_STLOC_1											{ APC_EmitOp(Opcode::Stloc_1); }
			| T_STLOC_2											{ APC_EmitOp(Opcode::Stloc_2); }
			| T_STLOC_3											{ APC_EmitOp(Opcode::Stloc_3); }
			| T_STLOC_S T_INTEGER								{ APC_EmitOp2(Opcode::Stloc_S, $2); }
			
			| T_NEW												{ APC_EmitOp(Opcode::New); }
			
			| T_ADD												{ APC_EmitOp(Opcode::Add); }
			| T_SUB												{ APC_EmitOp(Opcode::Sub); }
			| T_MUL												{ APC_EmitOp(Opcode::Mul); }
			| T_DIV												{ APC_EmitOp(Opcode::Div); }
			| T_REM												{ APC_EmitOp(Opcode::Rem); }
			| T_NEG												{ APC_EmitOp(Opcode::Neg); }

			| T_AND												{ APC_EmitOp(Opcode::And); }
			| T_OR												{ APC_EmitOp(Opcode::Or); }
			| T_XOR												{ APC_EmitOp(Opcode::Xor); }
			| T_NOT												{ APC_EmitOp(Opcode::Not); }
			| T_SHR												{ APC_EmitOp(Opcode::RightShift); }
			| T_SHL												{ APC_EmitOp(Opcode::LeftShift); }

			| T_LAND											{ APC_EmitOp(Opcode::LogAnd); }
			| T_LOR												{ APC_EmitOp(Opcode::LogOr); }
			| T_LNOT											{ APC_EmitOp(Opcode::LogNot); }
			| T_EQ												{ APC_EmitOp(Opcode::Equal); }
			| T_NE												{ APC_EmitOp(Opcode::NotEqual); }
			| T_GT												{ APC_EmitOp(Opcode::Greater); }
			| T_GE												{ APC_EmitOp(Opcode::GreaterEq); }
			| T_LT												{ APC_EmitOp(Opcode::Lesser); }
			| T_LE												{ APC_EmitOp(Opcode::LesserEq); }

			| T_CALL											{ APC_EmitOp(Opcode::Call); }
			| T_RET												{ APC_EmitOp(Opcode::Return); }
			
			| T_JUMP T_IDENTIFIER								{ APC_EmitOp2(Opcode::Jump, APC_FindLabel($2)); free($2); }
			| T_BRTRUE T_IDENTIFIER								{ APC_EmitOp2(Opcode::Brtrue, APC_FindLabel($2)); free($2); }
			| T_BRFALSE T_IDENTIFIER							{ APC_EmitOp2(Opcode::Brfalse, APC_FindLabel($2)); free($2); }
			;
