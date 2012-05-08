#include "AdrenoEmit.h"

typedef struct
{
	AdrenoFunction Function;
	AdrenoHashtable Labels;
	AdrenoHashtable RLabels;
} EmitFunction;

unsigned int AdrenoEmit_SetLabel(AdrenoFunction *fnc, wchar_t *name)
{
	EmitFunction *e = (EmitFunction *)fnc;
	
	AdrenoHashtable_Set(&e->Labels, name, (void *)e->Function.BytecodeSize);

	return e->Labels.NodeCount;
}

void AdrenoEmit_EmitJump(AdrenoFunction *function, wchar_t *name)
{
	EmitFunction *e = (EmitFunction *)function;
	
	AdrenoHashtable_Set(&e->RLabels, (void *)(e->Function.BytecodeSize + 1), name);
	AdrenoEmit_EmitOp2_I4(function, OP_JUMP, 0);
}

int AdrenoEmit_ResolveJumps(AdrenoFunction *function)
{
	EmitFunction *e = (EmitFunction *)function;
	int i;

	for (i = 0; i < e->RLabels.NodeCount; i++)
	{
		unsigned int tAddr = 0;
		unsigned int offset = (unsigned int)e->RLabels.NodeHeap[i].Value.Key;
		unsigned int *addr = (unsigned int *)(&e->Function.Bytecode[offset]);
		wchar_t *name = (wchar_t *)e->RLabels.NodeHeap[i].Value.Value;

		if (!AdrenoHashtable_Get(&e->Labels, name, (void **)&tAddr))
			return 0;

		*addr = tAddr - (offset + 4);
	}

	return 1;
}

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
	EmitFunction *f = (EmitFunction *)AdrenoAlloc(sizeof(EmitFunction));

	f->Function.Index = script->Functions.NodeCount;
	f->Function.NameIndex = AdrenoEmit_AddString(script, name);
	f->Function.Bytecode = NULL;
	f->Function.BytecodeSize = 0;
	f->Function.ArgumentCount = 0;
	f->Function.LocalsCount = 0;
	f->Function.Owner = script;

	AdrenoHashtable_Init(&f->Labels, AdrenoHashtable_Hash_Fnv, (AdrenoHashtable_LenFunction)wcslen);
	AdrenoHashtable_Init(&f->RLabels, NULL, NULL);

	AdrenoHashtable_Set(&script->Functions, script->Strings.NodeHeap[f->Function.NameIndex].Value.Value, f);

	return (AdrenoFunction *)f;
}

void AdrenoEmit_Expand(AdrenoFunction *function, int size)
{
	function->BytecodeSize += size;
	function->Bytecode = (unsigned char *)AdrenoRealloc(function->Bytecode, function->BytecodeSize);
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
