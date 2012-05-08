#include "ailc.h"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <sys/stat.h>

#include "../AdrenoVM/AdrenoEmit.h"

#include "engine.h"
#include "ailgrammar.h"

extern void (*RuleJumpTable[])(struct TokenStruct *Token, AilCompiler *compiler);

void AilCompiler_Initialize(AilCompiler *compiler, wchar_t *data)
{
	compiler->data = data;
}

AdrenoScript *AilCompiler_Compile(AilCompiler *compiler)
{
	int result;
	struct TokenStruct *token;
	
	compiler->currentPrefix = P_NONE;
	compiler->script = (AdrenoScript *)AdrenoAlloc(sizeof(AdrenoScript));
	AdrenoScript_Initialize(compiler->script);

	result = Parse(compiler->data, wcslen(compiler->data), 1, 0, &token);

	RuleJumpTable[token->ReductionRule](token, compiler);

	return compiler->script;
}

void AilCompiler_Free(AilCompiler *compiler)
{

}

void RuleTemplate(struct TokenStruct *Token, AilCompiler *compiler)
{
	int i;


	for (i = 0; i < Grammar.RuleArray[Token->ReductionRule].SymbolsCount; i++) 
	{
		if (Token->Tokens[i]->ReductionRule > 0) 
		{
			RuleJumpTable[Token->Tokens[i]->ReductionRule](Token->Tokens[i],compiler);
		}
	}
}




/***** Rule subroutines *****************************************************/




/* <Program> ::= <SourceElements> */
void Rule_Program(struct TokenStruct *Token, AilCompiler *compiler) {

	RuleTemplate(Token,compiler);
};




/* <SourceElements> ::= <SourceElement> */
void Rule_SourceElements(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <SourceElements> ::= <SourceElement> <SourceElements> */
void Rule_SourceElements2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <SourceElement> ::= <FunctionDeclaration> */
void Rule_SourceElement(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <SourceElement> ::= <Statement> */
void Rule_SourceElement2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <FunctionDeclaration> ::= function IdentifierName <Block> */
void Rule_FunctionDeclaration_function_IdentifierName(struct TokenStruct *Token, AilCompiler *compiler) {
	compiler->currentFunction = AdrenoEmit_CreateFunction(compiler->script, Token->Tokens[1]->Data);
	
	RuleTemplate(Token,compiler);
};




/* <StatementList> ::= <Statement> */
void Rule_StatementList(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <StatementList> ::= <Statement> <StatementList> */
void Rule_StatementList2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Statement> ::= <Opcodes> */
void Rule_Statement(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Statement> ::= <Label> */
void Rule_Statement2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcodes> ::= <Prefix> <Opcode> */
void Rule_Opcodes(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcodes> ::= <Opcode> */
void Rule_Opcodes2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Label> ::= IdentifierName ':' */
void Rule_Label_IdentifierName_Colon(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_SetLabel(compiler->currentFunction, Token->Tokens[0]->Data);

	RuleTemplate(Token,compiler);
};




/* <Block> ::= '{' '}' */
void Rule_Block_LBrace_RBrace(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Block> ::= '{' <StatementList> '}' */
void Rule_Block_LBrace_RBrace2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <RefPrefix> ::= ref */
void Rule_RefPrefix_ref(struct TokenStruct *Token, AilCompiler *compiler) {
	compiler->currentPrefix = P_REFERENCE;
	RuleTemplate(Token,compiler);
};




/* <Prefix> ::= <RefPrefix> */
void Rule_Prefix(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <LocalsInit> ::= '.initlocals' DecLiteralTerminal */
void Rule_LocalsInit_Dotinitlocals_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	compiler->currentFunction->LocalsCount = _wtoi(Token->Tokens[1]->Data);

	RuleTemplate(Token,compiler);
};




/* <ArgsInit> ::= '.initargs' DecLiteralTerminal */
void Rule_ArgsInit_Dotinitargs_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	compiler->currentFunction->ArgumentCount = _wtoi(Token->Tokens[1]->Data);

	RuleTemplate(Token,compiler);
};




/* <PopOp> ::= pop */
void Rule_PopOp_pop(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_POP));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Stloc0Op> ::= 'stloc.0' */
void Rule_Stloc0Op_stlocDot0(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STLOC_0));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Stloc1Op> ::= 'stloc.1' */
void Rule_Stloc1Op_stlocDot1(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STLOC_1));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Stloc2Op> ::= 'stloc.2' */
void Rule_Stloc2Op_stlocDot2(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STLOC_2));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Stloc3Op> ::= 'stloc.3' */
void Rule_Stloc3Op_stlocDot3(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STLOC_3));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <StlocSOp> ::= 'stloc.s' DecLiteralTerminal */
void Rule_StlocSOp_stlocDots_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STLOC_S), _wtoi(Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldloc0Op> ::= 'ldloc.0' */
void Rule_Ldloc0Op_ldlocDot0(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLOC_0));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldloc1Op> ::= 'ldloc.1' */
void Rule_Ldloc1Op_ldlocDot1(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLOC_1));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldloc2Op> ::= 'ldloc.2' */
void Rule_Ldloc2Op_ldlocDot2(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLOC_2));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldloc3Op> ::= 'ldloc.3' */
void Rule_Ldloc3Op_ldlocDot3(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLOC_3));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdlocSOp> ::= 'ldloc.s' DecLiteralTerminal */
void Rule_LdlocSOp_ldlocDots_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLOC_S), _wtoi(Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldarg0Op> ::= 'ldarg.0' */
void Rule_Ldarg0Op_ldargDot0(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDARG_0));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldarg1Op> ::= 'ldarg.1' */
void Rule_Ldarg1Op_ldargDot1(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDARG_1));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldarg2Op> ::= 'ldarg.2' */
void Rule_Ldarg2Op_ldargDot2(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDARG_2));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldarg3Op> ::= 'ldarg.3' */
void Rule_Ldarg3Op_ldargDot3(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDARG_3));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdargSOp> ::= 'ldarg.s' DecLiteralTerminal */
void Rule_LdargSOp_ldargDots_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDARG_S), _wtoi(Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdnullOp> ::= ldnull */
void Rule_LdnullOp_ldnull(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDNULL));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldci4Op> ::= 'ldc.i4' DecLiteralTerminal */
void Rule_Ldci4Op_ldcDoti4_DecLiteralTerminal(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDC_I4), _wtoi(Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldci4M1Op> ::= 'ldc.i4.m1' */
void Rule_Ldci4M1Op_ldcDoti4Dotm1(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDC_I4_M1));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldci40Op> ::= 'ldc.i4.0' */
void Rule_Ldci40Op_ldcDoti4Dot0(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDC_I4_0));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Ldci41Op> ::= 'ldc.i4.1' */
void Rule_Ldci41Op_ldcDoti4Dot1(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDC_I4_1));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdstrOp> ::= ldstr StringLiteralTerminalWithQuote */
void Rule_LdstrOp_ldstr_StringLiteralTerminalWithQuote(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDSTR), AdrenoEmit_AddString(compiler->script, Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <NewarrOp> ::= newarr */
void Rule_NewarrOp_newarr(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_NEWARR));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <StelemOp> ::= stelem */
void Rule_StelemOp_stelem(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_STELEM));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdelemOp> ::= ldelem */
void Rule_LdelemOp_ldelem(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDELEM));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <RmelemOp> ::= rmelem */
void Rule_RmelemOp_rmelem(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_RMELEM));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdlenOp> ::= ldlen */
void Rule_LdlenOp_ldlen(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDLEN));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <AddOp> ::= add */
void Rule_AddOp_add(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_ADD));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <SubOp> ::= sub */
void Rule_SubOp_sub(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_SUB));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <MulOp> ::= mul */
void Rule_MulOp_mul(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_MUL));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <DivOp> ::= div */
void Rule_DivOp_div(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_DIV));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <RemOp> ::= rem */
void Rule_RemOp_rem(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_REM));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <NegOp> ::= neg */
void Rule_NegOp_neg(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_NEG));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <OrOp> ::= or */
void Rule_OrOp_or(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_OR));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <AndOp> ::= and */
void Rule_AndOp_and(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_AND));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <XorOp> ::= xor */
void Rule_XorOp_xor(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_XOR));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <NotOp> ::= not */
void Rule_NotOp_not(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_NOT));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <ShlOp> ::= shl */
void Rule_ShlOp_shl(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_SHL));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <ShrOp> ::= shr */
void Rule_ShrOp_shr(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_SHR));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LOrOp> ::= lor */
void Rule_LOrOp_lor(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LOR));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LAndOp> ::= land */
void Rule_LAndOp_land(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LAND));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LNotOp> ::= lnot */
void Rule_LNotOp_lnot(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LNOT));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <EqOp> ::= eq */
void Rule_EqOp_eq(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_EQ));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <NeOp> ::= ne */
void Rule_NeOp_ne(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_NE));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <GtOp> ::= gt */
void Rule_GtOp_gt(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_GT));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <GeOp> ::= ge */
void Rule_GeOp_ge(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_GE));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LtOp> ::= lt */
void Rule_LtOp_lt(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LT));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LeOp> ::= le */
void Rule_LeOp_le(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LE));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <JumpOp> ::= 'jump ' IdentifierName */
void Rule_JumpOp_jump_IdentifierName(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitJump(compiler->currentFunction, Token->Tokens[1]->Data);
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <BrtrueOp> ::= brtrue */
void Rule_BrtrueOp_brtrue(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_BRTRUE));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <BrfalseOp> ::= brfalse */
void Rule_BrfalseOp_brfalse(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_BRFALSE));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <LdfuncOp> ::= ldfunc StringLiteralTerminalWithQuote */
void Rule_LdfuncOp_ldfunc_StringLiteralTerminalWithQuote(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp2_I4(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_LDFUNC), _wtoi(Token->Tokens[1]->Data));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <CallOp> ::= call */
void Rule_CallOp_call(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_CALL));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <EnterOp> ::= enter */
void Rule_EnterOp_enter(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_ENTER));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <RetOp> ::= ret */
void Rule_RetOp_ret(struct TokenStruct *Token, AilCompiler *compiler) {
	AdrenoEmit_EmitOp(compiler->currentFunction, (AdrenoOpcodes)(compiler->currentPrefix | OP_RET));
	compiler->currentPrefix = P_NONE;

	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LocalsInit> */
void Rule_Opcode(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <ArgsInit> */
void Rule_Opcode2(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <PopOp> */
void Rule_Opcode3(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Stloc0Op> */
void Rule_Opcode4(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Stloc1Op> */
void Rule_Opcode5(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Stloc2Op> */
void Rule_Opcode6(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Stloc3Op> */
void Rule_Opcode7(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <StlocSOp> */
void Rule_Opcode8(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldloc0Op> */
void Rule_Opcode9(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldloc1Op> */
void Rule_Opcode10(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldloc2Op> */
void Rule_Opcode11(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldloc3Op> */
void Rule_Opcode12(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdlocSOp> */
void Rule_Opcode13(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldarg0Op> */
void Rule_Opcode14(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldarg1Op> */
void Rule_Opcode15(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldarg2Op> */
void Rule_Opcode16(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldarg3Op> */
void Rule_Opcode17(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdargSOp> */
void Rule_Opcode18(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdnullOp> */
void Rule_Opcode19(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldci4Op> */
void Rule_Opcode20(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldci4M1Op> */
void Rule_Opcode21(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldci40Op> */
void Rule_Opcode22(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <Ldci41Op> */
void Rule_Opcode23(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdstrOp> */
void Rule_Opcode24(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <NewarrOp> */
void Rule_Opcode25(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <StelemOp> */
void Rule_Opcode26(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdelemOp> */
void Rule_Opcode27(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <RmelemOp> */
void Rule_Opcode28(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdlenOp> */
void Rule_Opcode29(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <AddOp> */
void Rule_Opcode30(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <SubOp> */
void Rule_Opcode31(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <MulOp> */
void Rule_Opcode32(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <DivOp> */
void Rule_Opcode33(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <RemOp> */
void Rule_Opcode34(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <NegOp> */
void Rule_Opcode35(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <OrOp> */
void Rule_Opcode36(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <AndOp> */
void Rule_Opcode37(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <XorOp> */
void Rule_Opcode38(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <NotOp> */
void Rule_Opcode39(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <ShlOp> */
void Rule_Opcode40(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <ShrOp> */
void Rule_Opcode41(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LOrOp> */
void Rule_Opcode42(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LAndOp> */
void Rule_Opcode43(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LNotOp> */
void Rule_Opcode44(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <EqOp> */
void Rule_Opcode45(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <NeOp> */
void Rule_Opcode46(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <GtOp> */
void Rule_Opcode47(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <GeOp> */
void Rule_Opcode48(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LtOp> */
void Rule_Opcode49(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LeOp> */
void Rule_Opcode50(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <JumpOp> */
void Rule_Opcode51(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <BrfalseOp> */
void Rule_Opcode52(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <BrtrueOp> */
void Rule_Opcode53(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <LdfuncOp> */
void Rule_Opcode54(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <CallOp> */
void Rule_Opcode55(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <EnterOp> */
void Rule_Opcode56(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/* <Opcode> ::= <RetOp> */
void Rule_Opcode57(struct TokenStruct *Token, AilCompiler *compiler) {
	RuleTemplate(Token,compiler);
};




/***** Rule jumptable *******************************************************/




void (*RuleJumpTable[])(struct TokenStruct *Token, AilCompiler *compiler) = {

	/* 0. <Program> ::= <SourceElements> */
	Rule_Program,

	/* 1. <SourceElements> ::= <SourceElement> */
	Rule_SourceElements,

	/* 2. <SourceElements> ::= <SourceElement> <SourceElements> */
	Rule_SourceElements2,

	/* 3. <SourceElement> ::= <FunctionDeclaration> */
	Rule_SourceElement,

	/* 4. <SourceElement> ::= <Statement> */
	Rule_SourceElement2,

	/* 5. <FunctionDeclaration> ::= function IdentifierName <Block> */
	Rule_FunctionDeclaration_function_IdentifierName,

	/* 6. <StatementList> ::= <Statement> */
	Rule_StatementList,

	/* 7. <StatementList> ::= <Statement> <StatementList> */
	Rule_StatementList2,

	/* 8. <Statement> ::= <Opcodes> */
	Rule_Statement,

	/* 9. <Statement> ::= <Label> */
	Rule_Statement2,

	/* 10. <Opcodes> ::= <Prefix> <Opcode> */
	Rule_Opcodes,

	/* 11. <Opcodes> ::= <Opcode> */
	Rule_Opcodes2,

	/* 12. <Label> ::= IdentifierName ':' */
	Rule_Label_IdentifierName_Colon,

	/* 13. <Block> ::= '{' '}' */
	Rule_Block_LBrace_RBrace,

	/* 14. <Block> ::= '{' <StatementList> '}' */
	Rule_Block_LBrace_RBrace2,

	/* 15. <RefPrefix> ::= ref */
	Rule_RefPrefix_ref,

	/* 16. <Prefix> ::= <RefPrefix> */
	Rule_Prefix,

	/* 17. <LocalsInit> ::= '.initlocals' DecLiteralTerminal */
	Rule_LocalsInit_Dotinitlocals_DecLiteralTerminal,

	/* 18. <ArgsInit> ::= '.initargs' DecLiteralTerminal */
	Rule_ArgsInit_Dotinitargs_DecLiteralTerminal,

	/* 19. <PopOp> ::= pop */
	Rule_PopOp_pop,

	/* 20. <Stloc0Op> ::= 'stloc.0' */
	Rule_Stloc0Op_stlocDot0,

	/* 21. <Stloc1Op> ::= 'stloc.1' */
	Rule_Stloc1Op_stlocDot1,

	/* 22. <Stloc2Op> ::= 'stloc.2' */
	Rule_Stloc2Op_stlocDot2,

	/* 23. <Stloc3Op> ::= 'stloc.3' */
	Rule_Stloc3Op_stlocDot3,

	/* 24. <StlocSOp> ::= 'stloc.s' DecLiteralTerminal */
	Rule_StlocSOp_stlocDots_DecLiteralTerminal,

	/* 25. <Ldloc0Op> ::= 'ldloc.0' */
	Rule_Ldloc0Op_ldlocDot0,

	/* 26. <Ldloc1Op> ::= 'ldloc.1' */
	Rule_Ldloc1Op_ldlocDot1,

	/* 27. <Ldloc2Op> ::= 'ldloc.2' */
	Rule_Ldloc2Op_ldlocDot2,

	/* 28. <Ldloc3Op> ::= 'ldloc.3' */
	Rule_Ldloc3Op_ldlocDot3,

	/* 29. <LdlocSOp> ::= 'ldloc.s' DecLiteralTerminal */
	Rule_LdlocSOp_ldlocDots_DecLiteralTerminal,

	/* 30. <Ldarg0Op> ::= 'ldarg.0' */
	Rule_Ldarg0Op_ldargDot0,

	/* 31. <Ldarg1Op> ::= 'ldarg.1' */
	Rule_Ldarg1Op_ldargDot1,

	/* 32. <Ldarg2Op> ::= 'ldarg.2' */
	Rule_Ldarg2Op_ldargDot2,

	/* 33. <Ldarg3Op> ::= 'ldarg.3' */
	Rule_Ldarg3Op_ldargDot3,

	/* 34. <LdargSOp> ::= 'ldarg.s' DecLiteralTerminal */
	Rule_LdargSOp_ldargDots_DecLiteralTerminal,

	/* 35. <LdnullOp> ::= ldnull */
	Rule_LdnullOp_ldnull,

	/* 36. <Ldci4Op> ::= 'ldc.i4' DecLiteralTerminal */
	Rule_Ldci4Op_ldcDoti4_DecLiteralTerminal,

	/* 37. <Ldci4M1Op> ::= 'ldc.i4.m1' */
	Rule_Ldci4M1Op_ldcDoti4Dotm1,

	/* 38. <Ldci40Op> ::= 'ldc.i4.0' */
	Rule_Ldci40Op_ldcDoti4Dot0,

	/* 39. <Ldci41Op> ::= 'ldc.i4.1' */
	Rule_Ldci41Op_ldcDoti4Dot1,

	/* 40. <LdstrOp> ::= ldstr StringLiteralTerminalWithQuote */
	Rule_LdstrOp_ldstr_StringLiteralTerminalWithQuote,

	/* 41. <NewarrOp> ::= newarr */
	Rule_NewarrOp_newarr,

	/* 42. <StelemOp> ::= stelem */
	Rule_StelemOp_stelem,

	/* 43. <LdelemOp> ::= ldelem */
	Rule_LdelemOp_ldelem,

	/* 44. <RmelemOp> ::= rmelem */
	Rule_RmelemOp_rmelem,

	/* 45. <LdlenOp> ::= ldlen */
	Rule_LdlenOp_ldlen,

	/* 46. <AddOp> ::= add */
	Rule_AddOp_add,

	/* 47. <SubOp> ::= sub */
	Rule_SubOp_sub,

	/* 48. <MulOp> ::= mul */
	Rule_MulOp_mul,

	/* 49. <DivOp> ::= div */
	Rule_DivOp_div,

	/* 50. <RemOp> ::= rem */
	Rule_RemOp_rem,

	/* 51. <NegOp> ::= neg */
	Rule_NegOp_neg,

	/* 52. <OrOp> ::= or */
	Rule_OrOp_or,

	/* 53. <AndOp> ::= and */
	Rule_AndOp_and,

	/* 54. <XorOp> ::= xor */
	Rule_XorOp_xor,

	/* 55. <NotOp> ::= not */
	Rule_NotOp_not,

	/* 56. <ShlOp> ::= shl */
	Rule_ShlOp_shl,

	/* 57. <ShrOp> ::= shr */
	Rule_ShrOp_shr,

	/* 58. <LOrOp> ::= lor */
	Rule_LOrOp_lor,

	/* 59. <LAndOp> ::= land */
	Rule_LAndOp_land,

	/* 60. <LNotOp> ::= lnot */
	Rule_LNotOp_lnot,

	/* 61. <EqOp> ::= eq */
	Rule_EqOp_eq,

	/* 62. <NeOp> ::= ne */
	Rule_NeOp_ne,

	/* 63. <GtOp> ::= gt */
	Rule_GtOp_gt,

	/* 64. <GeOp> ::= ge */
	Rule_GeOp_ge,

	/* 65. <LtOp> ::= lt */
	Rule_LtOp_lt,

	/* 66. <LeOp> ::= le */
	Rule_LeOp_le,

	/* 67. <JumpOp> ::= 'jump ' IdentifierName */
	Rule_JumpOp_jump_IdentifierName,

	/* 68. <BrtrueOp> ::= brtrue */
	Rule_BrtrueOp_brtrue,

	/* 69. <BrfalseOp> ::= brfalse */
	Rule_BrfalseOp_brfalse,

	/* 70. <LdfuncOp> ::= ldfunc StringLiteralTerminalWithQuote */
	Rule_LdfuncOp_ldfunc_StringLiteralTerminalWithQuote,

	/* 71. <CallOp> ::= call */
	Rule_CallOp_call,

	/* 72. <EnterOp> ::= enter */
	Rule_EnterOp_enter,

	/* 73. <RetOp> ::= ret */
	Rule_RetOp_ret,

	/* 74. <Opcode> ::= <LocalsInit> */
	Rule_Opcode,

	/* 75. <Opcode> ::= <ArgsInit> */
	Rule_Opcode2,

	/* 76. <Opcode> ::= <PopOp> */
	Rule_Opcode3,

	/* 77. <Opcode> ::= <Stloc0Op> */
	Rule_Opcode4,

	/* 78. <Opcode> ::= <Stloc1Op> */
	Rule_Opcode5,

	/* 79. <Opcode> ::= <Stloc2Op> */
	Rule_Opcode6,

	/* 80. <Opcode> ::= <Stloc3Op> */
	Rule_Opcode7,

	/* 81. <Opcode> ::= <StlocSOp> */
	Rule_Opcode8,

	/* 82. <Opcode> ::= <Ldloc0Op> */
	Rule_Opcode9,

	/* 83. <Opcode> ::= <Ldloc1Op> */
	Rule_Opcode10,

	/* 84. <Opcode> ::= <Ldloc2Op> */
	Rule_Opcode11,

	/* 85. <Opcode> ::= <Ldloc3Op> */
	Rule_Opcode12,

	/* 86. <Opcode> ::= <LdlocSOp> */
	Rule_Opcode13,

	/* 87. <Opcode> ::= <Ldarg0Op> */
	Rule_Opcode14,

	/* 88. <Opcode> ::= <Ldarg1Op> */
	Rule_Opcode15,

	/* 89. <Opcode> ::= <Ldarg2Op> */
	Rule_Opcode16,

	/* 90. <Opcode> ::= <Ldarg3Op> */
	Rule_Opcode17,

	/* 91. <Opcode> ::= <LdargSOp> */
	Rule_Opcode18,

	/* 92. <Opcode> ::= <LdnullOp> */
	Rule_Opcode19,

	/* 93. <Opcode> ::= <Ldci4Op> */
	Rule_Opcode20,

	/* 94. <Opcode> ::= <Ldci4M1Op> */
	Rule_Opcode21,

	/* 95. <Opcode> ::= <Ldci40Op> */
	Rule_Opcode22,

	/* 96. <Opcode> ::= <Ldci41Op> */
	Rule_Opcode23,

	/* 97. <Opcode> ::= <LdstrOp> */
	Rule_Opcode24,

	/* 98. <Opcode> ::= <NewarrOp> */
	Rule_Opcode25,

	/* 99. <Opcode> ::= <StelemOp> */
	Rule_Opcode26,

	/* 100. <Opcode> ::= <LdelemOp> */
	Rule_Opcode27,

	/* 101. <Opcode> ::= <RmelemOp> */
	Rule_Opcode28,

	/* 102. <Opcode> ::= <LdlenOp> */
	Rule_Opcode29,

	/* 103. <Opcode> ::= <AddOp> */
	Rule_Opcode30,

	/* 104. <Opcode> ::= <SubOp> */
	Rule_Opcode31,

	/* 105. <Opcode> ::= <MulOp> */
	Rule_Opcode32,

	/* 106. <Opcode> ::= <DivOp> */
	Rule_Opcode33,

	/* 107. <Opcode> ::= <RemOp> */
	Rule_Opcode34,

	/* 108. <Opcode> ::= <NegOp> */
	Rule_Opcode35,

	/* 109. <Opcode> ::= <OrOp> */
	Rule_Opcode36,

	/* 110. <Opcode> ::= <AndOp> */
	Rule_Opcode37,

	/* 111. <Opcode> ::= <XorOp> */
	Rule_Opcode38,

	/* 112. <Opcode> ::= <NotOp> */
	Rule_Opcode39,

	/* 113. <Opcode> ::= <ShlOp> */
	Rule_Opcode40,

	/* 114. <Opcode> ::= <ShrOp> */
	Rule_Opcode41,

	/* 115. <Opcode> ::= <LOrOp> */
	Rule_Opcode42,

	/* 116. <Opcode> ::= <LAndOp> */
	Rule_Opcode43,

	/* 117. <Opcode> ::= <LNotOp> */
	Rule_Opcode44,

	/* 118. <Opcode> ::= <EqOp> */
	Rule_Opcode45,

	/* 119. <Opcode> ::= <NeOp> */
	Rule_Opcode46,

	/* 120. <Opcode> ::= <GtOp> */
	Rule_Opcode47,

	/* 121. <Opcode> ::= <GeOp> */
	Rule_Opcode48,

	/* 122. <Opcode> ::= <LtOp> */
	Rule_Opcode49,

	/* 123. <Opcode> ::= <LeOp> */
	Rule_Opcode50,

	/* 124. <Opcode> ::= <JumpOp> */
	Rule_Opcode51,

	/* 125. <Opcode> ::= <BrfalseOp> */
	Rule_Opcode52,

	/* 126. <Opcode> ::= <BrtrueOp> */
	Rule_Opcode53,

	/* 127. <Opcode> ::= <LdfuncOp> */
	Rule_Opcode54,

	/* 128. <Opcode> ::= <CallOp> */
	Rule_Opcode55,

	/* 129. <Opcode> ::= <EnterOp> */
	Rule_Opcode56,

	/* 130. <Opcode> ::= <RetOp> */
	Rule_Opcode57 
};
