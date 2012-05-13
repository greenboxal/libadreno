#include <adreno/vm/emit.h>
#include <adreno/utils/memorystream.h>

#include <string.h>

typedef struct
{
	AdrenoFunction Function;
	AdrenoMS Stream;
	AdrenoHashtable Labels;
	AdrenoHashtable RLabels;
} EmitFunction;

unsigned int AdrenoEmit_AddString(AdrenoScript *script, char *string, unsigned int len)
{
	unsigned int i;
	AdrenoValue *value;

	for (i = 0; i < script->Strings.NodeCount; i++)
		if (((AdrenoString *)script->Strings.NodeHeap[i].Value.Value)->Size == len && memcmp(((AdrenoString *)script->Strings.NodeHeap[i].Value.Value)->Value, string, len) == 0)
			return i;
	
	i = script->Strings.NodeCount;

	value = (AdrenoValue *)AdrenoAlloc(sizeof(AdrenoValue));
	value->Type = AT_STRING;
	value->GCFlags = GC_FINAL_FREE;
	value->ReferenceCounter = 0;
	value->Value.String = (AdrenoString *)AdrenoAlloc(sizeof(AdrenoString));
	value->Value.String->Value = (char *)AdrenoAlloc(len + 1);
	memcpy(value->Value.String->Value, string, len);
	value->Value.String->Value[len] = 0;
	value->Value.String->Size = len;
	value->Value.String->Flags = SF_FREE;

	AdrenoHashtable_Set(&script->Strings, (void *)i, value);

	return i;
}

AdrenoFunction *AdrenoEmit_CreateFunction(AdrenoScript *script, char *name)
{
	EmitFunction *f = (EmitFunction *)AdrenoAlloc(sizeof(EmitFunction));

	f->Function.Index = script->Functions.NodeCount;
	f->Function.NameIndex = AdrenoEmit_AddString(script, name, (unsigned int)strlen(name));
	f->Function.Bytecode = NULL;
	f->Function.BytecodeSize = 0;
	f->Function.ArgumentCount = 0;
	f->Function.LocalsCount = 0;
	f->Function.Owner = script;
	f->Function.GCFlags = (AdrenoGCFlags)(GC_FREE | GC_COLLECT);

	AdrenoHashtable_Initialize(&f->Labels, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_WString);
	AdrenoHashtable_Initialize(&f->RLabels, NULL, NULL);

	AdrenoHashtable_Set(&script->Functions, script->Strings.NodeHeap[f->Function.NameIndex].Value.Value, f);
	AdrenoMS_Open(&f->Stream);

	return (AdrenoFunction *)f;
}

void AdrenoEmit_EmitOp(AdrenoFunction *function, unsigned char op)
{
	EmitFunction *e = (EmitFunction *)function;
	
	AdrenoMS_Write(&e->Stream, &op, 0, 1);
}

void AdrenoEmit_EmitOp2_I4(AdrenoFunction *function, unsigned char op, unsigned int val)
{
	EmitFunction *e = (EmitFunction *)function;
	
	AdrenoMS_Write(&e->Stream, &op, 0, 1);
	AdrenoMS_Write(&e->Stream, (unsigned char *)&val, 0, 4);
}

void AdrenoEmit_EmitJump(AdrenoFunction *function, unsigned char op, char *name)
{
	EmitFunction *e = (EmitFunction *)function;
	
	AdrenoHashtable_Set(&e->RLabels, (void *)(e->Function.BytecodeSize + 1), name);
	AdrenoEmit_EmitOp2_I4(function, op, 0);
}

unsigned int AdrenoEmit_SetLabel(AdrenoFunction *fnc, char *name)
{
	EmitFunction *e = (EmitFunction *)fnc;
	
	AdrenoHashtable_Set(&e->Labels, name, (void *)e->Function.BytecodeSize);

	return e->Labels.NodeCount - 1;
}

int AdrenoEmit_Finalize(AdrenoFunction *function)
{
	EmitFunction *e = (EmitFunction *)function;
	unsigned int i;

	for (i = 0; i < e->RLabels.NodeCount; i++)
	{
		unsigned int tAddr = 0;
		unsigned int offset = (unsigned int)e->RLabels.NodeHeap[i].Value.Key;
		unsigned int *addr = (unsigned int *)(&e->Function.Bytecode[offset]);
		char *name = (char *)e->RLabels.NodeHeap[i].Value.Value;

		if (!AdrenoHashtable_Get(&e->Labels, name, (void **)&tAddr))
			return 0;

		*addr = tAddr - (offset + 4);
	}

	AdrenoHashtable_Clear(&e->Labels);
	AdrenoHashtable_Clear(&e->RLabels);

	e->Function.Bytecode = AdrenoMS_Clone(&e->Stream);
	e->Function.BytecodeSize = e->Stream.bufferSize;
	AdrenoMS_Close(&e->Stream);

	return 1;
}
