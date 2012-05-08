#include "AdrenoEmit.h"

unsigned int AdrenoEmit_AddString(AdrenoScript *script, wchar_t *string)
{
	unsigned int i;

	for (i = 0; i < script->Strings.NodeCount; i++)
		if (wcscmp((wchar_t *)script->Strings.NodeHeap[i].Value.Value, string) == 0)
			return i;

	AdrenoHashtable_Set(&script->Strings, (void *)i, string);

	return i;
}

AdrenoFunction *AdrenoEmit_CreateFunction(AdrenoScript *script, wchar_t *name)
{
	AdrenoFunction *f = (AdrenoFunction *)AdrenoAlloc(sizeof(AdrenoFunction));

	f->Index = script->Functions.NodeCount;
	f->NameIndex = AdrenoEmit_AddString(script, name);
	f->Bytecode = NULL;
	f->BytecodeSize = 0;
	f->ArgumentCount = 0;
	f->LocalsCount = 0;
	f->Owner = script;

	AdrenoHashtable_Set(&script->Functions, script->Strings.NodeHeap[f->NameIndex].Value.Value, f);

	return f;
}

void AdrenoEmit_Expand(AdrenoFunction *function, int size)
{
	function->BytecodeSize += size;
	function->Bytecode = (char *)AdrenoRealloc(function->Bytecode, function->BytecodeSize);
}

void AdrenoEmit_EmitOp(AdrenoFunction *function, AdrenoOpcodes op)
{
	AdrenoEmit_Expand(function, 1);
	function->Bytecode[function->BytecodeSize - 1] = (char)op;
}

void AdrenoEmit_EmitOp2_I1(AdrenoFunction *function, AdrenoOpcodes op, unsigned char val)
{
	AdrenoEmit_Expand(function, 2);
	function->Bytecode[function->BytecodeSize - 2] = (char)op;
	function->Bytecode[function->BytecodeSize - 1] = (char)val;
}

void AdrenoEmit_EmitOp2_I2(AdrenoFunction *function, AdrenoOpcodes op, unsigned short val)
{
	AdrenoEmit_Expand(function, 3);
	function->Bytecode[function->BytecodeSize - 3] = (char)op;
	*((unsigned short *)&function->Bytecode[function->BytecodeSize - 2]) = val;
}

void AdrenoEmit_EmitOp2_I4(AdrenoFunction *function, AdrenoOpcodes op, unsigned int val)
{
	AdrenoEmit_Expand(function, 5);
	function->Bytecode[function->BytecodeSize - 5] = (char)op;
	*((unsigned int *)&function->Bytecode[function->BytecodeSize - 4]) = val;
}
