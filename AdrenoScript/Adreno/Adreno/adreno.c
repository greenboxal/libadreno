#include "adreno.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <Windows.h>

#include "engine.h"
#include "adrenoscript.h"
/*#include "adrenovm.h"

void (*RuleJumpTable[])(struct TokenStruct *Token, adreno_compiler *compiler);

#pragma region Code Gen

void emit_ensure(adreno_compiler *compiler, int size)
{
	if (compiler->bytecodeLen + size > compiler->bytecodeMax)
	{
		compiler->bytecodeMax = compiler->bytecodeLen + size;
		compiler->bytecode = (char *)realloc(compiler->bytecode, compiler->bytecodeMax);
	}
}

void emit_op1(adreno_compiler *compiler, AdrenoOp op)
{
	emit_ensure(compiler, 1);

	compiler->bytecode[compiler->bytecodeLen] = (unsigned char)op;

	compiler->bytecodeLen += 1;
}

void emit_op2_1(adreno_compiler *compiler, AdrenoOp op, unsigned char value)
{
	emit_ensure(compiler, 2);

	compiler->bytecode[compiler->bytecodeLen] = (unsigned char)op;
	compiler->bytecode[compiler->bytecodeLen + 1] = value;

	compiler->bytecodeLen += 2;
}

void emit_op2_4(adreno_compiler *compiler, AdrenoOp op, unsigned int value)
{
	emit_ensure(compiler, 5);

	compiler->bytecode[compiler->bytecodeLen] = (unsigned char)op;
	*((unsigned int *)&compiler->bytecode[compiler->bytecodeLen + 1]) = value;

	compiler->bytecodeLen += 5;
}

unsigned int adreno_compiler_addname(adreno_compiler *compiler, wchar_t *name)
{
	unsigned int i;

	for (i = 0; i < compiler->nameCount; i++)
		if (wcscmp(compiler->names[i], name) == 0)
			return i;

	compiler->names = (wchar_t **)realloc(compiler->names, (compiler->nameCount + 1) * sizeof(wchar_t *));
	compiler->names[compiler->nameCount] = _wcsdup(name);

	return compiler->nameCount++;
}

adreno_emit_function *emit_startfunc(adreno_compiler *compiler, wchar_t *name, unsigned int startIp)
{
	adreno_emit_function *func = (adreno_emit_function *)malloc(sizeof(adreno_emit_function));

	func->nameIndex = adreno_compiler_addname(compiler, name);
	func->startIp = startIp;
	func->argCount = 0;

	AdrenoHashtable_Init(&func->locals, AdrenoHashtable_Hash_Fnv, (LenFunction)wcslen);
	AdrenoHashtable_Set(&compiler->functions, compiler->names[func->nameIndex], func);

	return func;
}

#pragma endregion

#pragma region Parser Code

void RuleTemplate(struct TokenStruct *Token, adreno_compiler *compiler)
{
	int i;

	if (Token->ReductionRule >= 0)
	{
		for (i = 0; i < Grammar.RuleArray[Token->ReductionRule].SymbolsCount; i++)
		{
			if (Token->Tokens[i]->ReductionRule >= 0)
				RuleJumpTable[Token->Tokens[i]->ReductionRule](Token->Tokens[i], compiler);
		}
	}
}

void Rule_DecLiteral_DecLiteralTerminal(struct TokenStruct *Token, adreno_compiler *compiler)
{
	emit_op2_4(compiler, OP_LDINT_I4, _wtoi(Token->Tokens[0]->Data));

	RuleTemplate(Token, compiler);
}

void Rule_StringLiteral_StringLiteralTerminalWithQuote(struct TokenStruct *Token, adreno_compiler *compiler)
{
	Token->Tokens[0]->Data[wcslen(Token->Tokens[0]->Data) - 1] = 0;

	emit_op2_4(compiler, OP_LDINT_I4, adreno_compiler_addname(compiler, (wchar_t *)&Token->Tokens[0]->Data[1]));

	RuleTemplate(Token, compiler);
}

void Rule_StringLiteral_StringLiteralTerminalWithApostroph(struct TokenStruct *Token, adreno_compiler *compiler)
{
	Token->Tokens[0]->Data[wcslen(Token->Tokens[0]->Data) - 1] = 0;

	emit_op2_4(compiler, OP_LDINT_I4, adreno_compiler_addname(compiler, (wchar_t *)&Token->Tokens[0]->Data[1]));

	RuleTemplate(Token, compiler);
}

void Rule_Identifier_IdentifierName(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Literal(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Literal2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Program(struct TokenStruct *Token, adreno_compiler *compiler)
{
	compiler->mainFunction = compiler->currentFunction = emit_startfunc(compiler, L"<default>", compiler->bytecodeLen);

	RuleTemplate(Token, compiler);
}

void Rule_SourceElements(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SourceElements2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SourceElement(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SourceElement2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_FunctionDeclaration_FunctionWord_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	compiler->currentFunction = emit_startfunc(compiler, Token->Tokens[1]->Data, compiler->bytecodeLen);

	RuleTemplate(Token, compiler);
}

void Rule_FunctionParameterList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_FunctionParameterList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NonEmptyFunctionParameterList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NonEmptyFunctionParameterList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultipleFunctionParameterList_Comma(struct TokenStruct *Token, adreno_compiler *compiler)
{
	compiler->currentFunction->argCount++;

	RuleTemplate(Token, compiler);
}

void Rule_SingleFunctionParameterList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	compiler->currentFunction->argCount++;

	RuleTemplate(Token, compiler);
}

void Rule_EmptyFunctionParameterList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_StatementList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_StatementList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement6(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement7(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Statement8(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThenStatement(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThenStatement2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThenStatement3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThenStatement4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThenStatement5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NormalStatement6(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_EmptyStatement_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ExpressionStatement_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Block_LBrace_RBrace(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Block_LBrace_RBrace2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_WithStatement_WithWord_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ReturnStatement_ReturnWord_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ReturnStatement_ReturnWord_Semi2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_BreakStatement_BreakWord_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ContinueStatement_ContinueWord_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_IfStatement_if_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_IfElseStatement_if_LParan_RParan_ElseWord(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_GotoStatement_GotoWord_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Label_Colon(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SwitchStatement_SwitchWord_LParan_RParan_LBrace_RBrace(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CaseLabelList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CaseLabelList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CaseLabelList3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_EmptyCaseLabelList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NotEmptyCaseLabelList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CaseLabel_CaseWord_Colon(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CaseLabel_CaseWord_Colon2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_DefaultLabel_DefaultWord_Colon(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ForStatement_ForWord_LParan_Semi_Semi_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_FirstForExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SecondForExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ThirdForExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ForExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ForExpression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ForInStatement_ForWord_LParan_in_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_WhileStatement_WhileWord_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_DoWhileStatement_DoWord_WhileWord_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableStatement_VarWord_Semi(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableDeclarationList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableDeclarationList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultipleVariableDeclarationList_Comma(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleVariableDeclarationList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Expression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Expression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableExpression_VarWord(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableDeclaration(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_VariableDeclaration2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UsualVariableDeclaration_Eq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleVariableDeclaration(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_AssignmentExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_AssignmentExpression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression6(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression7(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression8(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpression9(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionEquality_Eq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	emit_op1(compiler, OP_EQ);

	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionPlus_PlusEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionMinus_MinusEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionMultiply_TimesEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionDivide_DivEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionXor_CaretEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionBinaryAnd_AmpEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionBinaryOr_PipeEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleAssignmentExpressionRemainder_PercentEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_IfOperation_Question_Colon(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_IfOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_LeftIfOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_RightIfOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OrOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OrOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorOr_PipePipe(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_LOR);
}

void Rule_AndOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_AndOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorAnd_AmpAmp(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_LAND);
}

void Rule_BinOrOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_BinOrOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorBinaryOr_Pipe(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_OR);
}

void Rule_BinXorOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_BinXorOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorBinaryXor_Caret(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_XOR);
}

void Rule_BinAndOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_BinAndOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorBinaryAnd_Amp(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_AND);
}

void Rule_EquateOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_EquateOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_EquateOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorEquality_EqEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_EQ);
}

void Rule_OperatorInequality_ExclamEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_NE);
}

void Rule_CompareOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CompareOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CompareOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CompareOperation4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CompareOperation5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorLess_Lt(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_LT);
}

void Rule_OperatorMore_Gt(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_GT);
}

void Rule_OperatorLessOrEqual_LtEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_LE);
}

void Rule_OperatorMoreOrEqual_GtEq(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_GE);
}

void Rule_ShiftOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ShiftOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ShiftOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorShiftLeft_LtLt(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_LSHIFT);
}

void Rule_OperatorShiftRight_GtGt(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_RSHIFT);
}

void Rule_AddOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_AddOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_AddOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorPlus_Plus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_ADD);
}

void Rule_OperatorMinus_Minus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_SUB);
}

void Rule_MultiplicationOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultiplicationOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultiplicationOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultiplicationOperation4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OperatorMultiply_Times(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_MUL);
}

void Rule_OperatorDivide_Div(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_DIV);
}

void Rule_OperatorRemainder_Percent(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[0], compiler);
	RuleJumpTable[Token->Tokens[2]->ReductionRule](Token->Tokens[2], compiler);

	emit_op1(compiler, OP_MOD);
}

void Rule_OtherOperation(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation5(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation6(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation7(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_OtherOperation8(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UnaryOperatorNot_Exclam(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[1], compiler);

	emit_op1(compiler, OP_LNOT);
}

void Rule_UnaryOperatorBinaryNot_Tilde(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[1], compiler);

	emit_op1(compiler, OP_NOT);
}

void Rule_UnaryOperatorPrefixInc_PlusPlus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UnaryOperatorPostfixInc_PlusPlus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UnaryOperatorPrefixDec_MinusMinus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UnaryOperatorPostfixDec_MinusMinus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_UnaryOperatorMinus_Minus(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleJumpTable[Token->Tokens[0]->ReductionRule](Token->Tokens[1], compiler);

	emit_op1(compiler, OP_NEG);
}

void Rule_RightValue(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_RightValue2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_RightValue3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_RightValue4(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_LeftValue(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_LeftValue2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_PrimaryExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_PrimaryExpression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_PrimaryExpression3(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_BracedExpression_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_CallExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_IndexExpression_LBracket_RBracket(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MemberExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MemberExpression2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_MultipleMemberExpression_Dot(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_SingleMemberExpression(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_Arguments_LParan_RParan(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ArgumentList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_ArgumentList2(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NonEmptyArgumentList_Comma(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_NonEmptyArgumentList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void Rule_EmptyArgumentList(struct TokenStruct *Token, adreno_compiler *compiler)
{
	RuleTemplate(Token, compiler);
}

void (*RuleJumpTable[])(struct TokenStruct *Token, adreno_compiler *compiler) =
{
	Rule_DecLiteral_DecLiteralTerminal,
	Rule_StringLiteral_StringLiteralTerminalWithQuote,
	Rule_StringLiteral_StringLiteralTerminalWithApostroph,
	Rule_Literal,
	Rule_Literal2,
	Rule_Identifier_IdentifierName,
	Rule_Program,
	Rule_SourceElements,
	Rule_SourceElements2,
	Rule_SourceElement,
	Rule_SourceElement2,
	Rule_FunctionDeclaration_FunctionWord_LParan_RParan,
	Rule_FunctionParameterList,
	Rule_FunctionParameterList2,
	Rule_NonEmptyFunctionParameterList,
	Rule_NonEmptyFunctionParameterList2,
	Rule_MultipleFunctionParameterList_Comma,
	Rule_SingleFunctionParameterList,
	Rule_EmptyFunctionParameterList,
	Rule_StatementList,
	Rule_StatementList2,
	Rule_Statement,
	Rule_Statement2,
	Rule_Statement3,
	Rule_Statement4,
	Rule_Statement5,
	Rule_Statement6,
	Rule_Statement7,
	Rule_Statement8,
	Rule_ThenStatement,
	Rule_ThenStatement2,
	Rule_ThenStatement3,
	Rule_ThenStatement4,
	Rule_ThenStatement5,
	Rule_NormalStatement,
	Rule_NormalStatement2,
	Rule_NormalStatement3,
	Rule_NormalStatement4,
	Rule_NormalStatement5,
	Rule_NormalStatement6,
	Rule_EmptyStatement_Semi,
	Rule_ExpressionStatement_Semi,
	Rule_Block_LBrace_RBrace,
	Rule_Block_LBrace_RBrace2,
	Rule_WithStatement_WithWord_LParan_RParan,
	Rule_ReturnStatement_ReturnWord_Semi,
	Rule_ReturnStatement_ReturnWord_Semi2,
	Rule_BreakStatement_BreakWord_Semi,
	Rule_ContinueStatement_ContinueWord_Semi,
	Rule_IfStatement_if_LParan_RParan,
	Rule_IfElseStatement_if_LParan_RParan_ElseWord,
	Rule_GotoStatement_GotoWord_Semi,
	Rule_Label_Colon,
	Rule_SwitchStatement_SwitchWord_LParan_RParan_LBrace_RBrace,
	Rule_CaseLabelList,
	Rule_CaseLabelList2,
	Rule_CaseLabelList3,
	Rule_EmptyCaseLabelList,
	Rule_NotEmptyCaseLabelList,
	Rule_CaseLabel_CaseWord_Colon,
	Rule_CaseLabel_CaseWord_Colon2,
	Rule_DefaultLabel_DefaultWord_Colon,
	Rule_ForStatement_ForWord_LParan_Semi_Semi_RParan,
	Rule_FirstForExpression,
	Rule_SecondForExpression,
	Rule_ThirdForExpression,
	Rule_ForExpression,
	Rule_ForExpression2,
	Rule_ForInStatement_ForWord_LParan_in_RParan,
	Rule_WhileStatement_WhileWord_LParan_RParan,
	Rule_DoWhileStatement_DoWord_WhileWord_LParan_RParan,
	Rule_VariableStatement_VarWord_Semi,
	Rule_VariableDeclarationList,
	Rule_VariableDeclarationList2,
	Rule_MultipleVariableDeclarationList_Comma,
	Rule_SingleVariableDeclarationList,
	Rule_Expression,
	Rule_Expression2,
	Rule_VariableExpression_VarWord,
	Rule_VariableDeclaration,
	Rule_VariableDeclaration2,
	Rule_UsualVariableDeclaration_Eq,
	Rule_SingleVariableDeclaration,
	Rule_AssignmentExpression,
	Rule_AssignmentExpression2,
	Rule_SingleAssignmentExpression,
	Rule_SingleAssignmentExpression2,
	Rule_SingleAssignmentExpression3,
	Rule_SingleAssignmentExpression4,
	Rule_SingleAssignmentExpression5,
	Rule_SingleAssignmentExpression6,
	Rule_SingleAssignmentExpression7,
	Rule_SingleAssignmentExpression8,
	Rule_SingleAssignmentExpression9,
	Rule_SingleAssignmentExpressionEquality_Eq,
	Rule_SingleAssignmentExpressionPlus_PlusEq,
	Rule_SingleAssignmentExpressionMinus_MinusEq,
	Rule_SingleAssignmentExpressionMultiply_TimesEq,
	Rule_SingleAssignmentExpressionDivide_DivEq,
	Rule_SingleAssignmentExpressionXor_CaretEq,
	Rule_SingleAssignmentExpressionBinaryAnd_AmpEq,
	Rule_SingleAssignmentExpressionBinaryOr_PipeEq,
	Rule_SingleAssignmentExpressionRemainder_PercentEq,
	Rule_IfOperation_Question_Colon,
	Rule_IfOperation,
	Rule_LeftIfOperation,
	Rule_RightIfOperation,
	Rule_OrOperation,
	Rule_OrOperation2,
	Rule_OperatorOr_PipePipe,
	Rule_AndOperation,
	Rule_AndOperation2,
	Rule_OperatorAnd_AmpAmp,
	Rule_BinOrOperation,
	Rule_BinOrOperation2,
	Rule_OperatorBinaryOr_Pipe,
	Rule_BinXorOperation,
	Rule_BinXorOperation2,
	Rule_OperatorBinaryXor_Caret,
	Rule_BinAndOperation,
	Rule_BinAndOperation2,
	Rule_OperatorBinaryAnd_Amp,
	Rule_EquateOperation,
	Rule_EquateOperation2,
	Rule_EquateOperation3,
	Rule_OperatorEquality_EqEq,
	Rule_OperatorInequality_ExclamEq,
	Rule_CompareOperation,
	Rule_CompareOperation2,
	Rule_CompareOperation3,
	Rule_CompareOperation4,
	Rule_CompareOperation5,
	Rule_OperatorLess_Lt,
	Rule_OperatorMore_Gt,
	Rule_OperatorLessOrEqual_LtEq,
	Rule_OperatorMoreOrEqual_GtEq,
	Rule_ShiftOperation,
	Rule_ShiftOperation2,
	Rule_ShiftOperation3,
	Rule_OperatorShiftLeft_LtLt,
	Rule_OperatorShiftRight_GtGt,
	Rule_AddOperation,
	Rule_AddOperation2,
	Rule_AddOperation3,
	Rule_OperatorPlus_Plus,
	Rule_OperatorMinus_Minus,
	Rule_MultiplicationOperation,
	Rule_MultiplicationOperation2,
	Rule_MultiplicationOperation3,
	Rule_MultiplicationOperation4,
	Rule_OperatorMultiply_Times,
	Rule_OperatorDivide_Div,
	Rule_OperatorRemainder_Percent,
	Rule_OtherOperation,
	Rule_OtherOperation2,
	Rule_OtherOperation3,
	Rule_OtherOperation4,
	Rule_OtherOperation5,
	Rule_OtherOperation6,
	Rule_OtherOperation7,
	Rule_OtherOperation8,
	Rule_UnaryOperatorNot_Exclam,
	Rule_UnaryOperatorBinaryNot_Tilde,
	Rule_UnaryOperatorPrefixInc_PlusPlus,
	Rule_UnaryOperatorPostfixInc_PlusPlus,
	Rule_UnaryOperatorPrefixDec_MinusMinus,
	Rule_UnaryOperatorPostfixDec_MinusMinus,
	Rule_UnaryOperatorMinus_Minus,
	Rule_RightValue,
	Rule_RightValue2,
	Rule_RightValue3,
	Rule_RightValue4,
	Rule_LeftValue,
	Rule_LeftValue2,
	Rule_PrimaryExpression,
	Rule_PrimaryExpression2,
	Rule_PrimaryExpression3,
	Rule_BracedExpression_LParan_RParan,
	Rule_CallExpression,
	Rule_IndexExpression_LBracket_RBracket,
	Rule_MemberExpression,
	Rule_MemberExpression2,
	Rule_MultipleMemberExpression_Dot,
	Rule_SingleMemberExpression,
	Rule_Arguments_LParan_RParan,
	Rule_ArgumentList,
	Rule_ArgumentList2,
	Rule_NonEmptyArgumentList_Comma,
	Rule_NonEmptyArgumentList,
	Rule_EmptyArgumentList
};

#pragma endregion

void adreno_compiler_init(adreno_compiler *compiler, char *source)
{
	int size;

	if (!compiler)
		return;

	memset(compiler, 0, sizeof(adreno_compiler));

	size = strlen(source) * 2 + 2;
	compiler->source = (wchar_t *)malloc(size);
	mbstowcs(compiler->source, source, size);

	AdrenoHashtable_Init(&compiler->functions, AdrenoHashtable_Hash_Fnv, (LenFunction)wcslen);
}

void adreno_compiler_free(adreno_compiler *compiler)
{
	if (!compiler)
		return;

	if (compiler->source)
		free(compiler->source);

	if (compiler->bytecode)
		free(compiler->bytecode);

	if (compiler->names)
	{
		unsigned int i;

		for (i = 0; i < compiler->nameCount; i++)
			free(compiler->names[i]);

		free(compiler->names);
	}

	AdrenoHashtable_Destroy(&compiler->functions);
}

void adreno_compiler_compile(adreno_compiler *compiler)
{
	struct TokenStruct *token;
	int result;
	int run = 1;
	AdrenoRules lastRule = (AdrenoRules)-1;
	AdrenoSymbols lastSymbol = (AdrenoSymbols)-1;

	result = Parse(compiler->source, wcslen(compiler->source), 1, 0, &token);

	if (result == PARSEACCEPT)
		RuleJumpTable[token->ReductionRule](token, compiler);

	DeleteTokens(token);
}

char *adreno_compiler_save(adreno_compiler *compiler, int *tSize)
{
	// h = short = 0xF00D
	//              h          bytecode                             functions
	int totalSize = 2 + compiler->bytecodeLen + (AdrenoHashtable_Count(&compiler->functions) * 12);
	int i, pos = 0;
	char *data;

	for (i = 0; i < compiler->nameCount; i++)
		totalSize += wcslen(compiler->names[i]) + 1;

	if (tSize)
		*tSize = totalSize;

	data = (char *)malloc(totalSize);

	*((unsigned short *)&data[pos]) = 0xF00D; pos += 2;

	*((unsigned int *)&data[pos]) = compiler->nameCount; pos += 4;
	for (i = 0; i < compiler->nameCount; i++)
	{
		wcscpy((wchar_t *)&data[pos], compiler->names[i]);
		pos += wcslen(compiler->names[i]);
		data[pos++] = 0;
	}

	for (i = 0; i < compiler->functions.NodeCount; i++)
	{
		adreno_emit_function *func = (adreno_emit_function *)compiler->functions.NodeHeap[i].Value.Value;

		*((unsigned int *)&data[pos]) = func->nameIndex; pos += 4;
		*((unsigned int *)&data[pos]) = func->startIp; pos += 4;
		*((unsigned int *)&data[pos]) = func->argCount; pos += 4;
	}

	memcpy(&data[pos], compiler->bytecode, compiler->bytecodeLen);

	return data;
}
*/