#include <adreno/vm/emit.h>
#include <adreno/utils/memorystream.h>

#include <string.h>

typedef struct
{
	unsigned int Offset;
	char *Name;
} EmitLabelRef;

typedef struct
{
	AdrenoFunction Function;
	AdrenoMS Stream;
	AdrenoHashtable Labels;
	AdrenoArray RLabels;
} EmitFunction;

unsigned int AdrenoEmit_AddString(AdrenoScript *script, char *string, unsigned int len)
{
	unsigned int i;
	AdrenoValue *value;

	for (i = 0; i < script->Strings.Count; i++)
	{
		if (((AdrenoValue *)script->Strings.Data[i])->Value.String->Size == len && memcmp(((AdrenoValue *)script->Strings.Data[i])->Value.String->Value, string, len) == 0)
			return i;
	}
	
	i = script->Strings.Count;

	value = (AdrenoValue *)AdrenoMemoryPool_Alloc(AdrenoVM_ValuePool);
	value->Type = AT_STRING;
	value->GCFlags = GC_FINAL_FREE;
	value->ReferenceCounter = 0;
	value->Value.String = (AdrenoString *)AdrenoAlloc(sizeof(AdrenoString));
	value->Value.String->Value = (char *)AdrenoAlloc(len + 1);
	memcpy(value->Value.String->Value, string, len);
	value->Value.String->Value[len] = 0;
	value->Value.String->Size = len;
	value->Value.String->Flags = SF_FREE;

	AdrenoArray_Add(&script->Strings, value);

	return i;
}

AdrenoFunction *AdrenoEmit_CreateFunction(AdrenoScript *script, char *name)
{
	EmitFunction *f = (EmitFunction *)AdrenoAlloc(sizeof(EmitFunction));

	f->Function.Type = AF_SCRIPT;
	f->Function.Index = script->Functions.NodeCount;
	f->Function.NameIndex = AdrenoEmit_AddString(script, name, (unsigned int)strlen(name));
	f->Function.Bytecode = NULL;
	f->Function.BytecodeSize = 0;
	f->Function.LocalsCount = 0;
	f->Function.Owner = script;
	f->Function.GCFlags = (AdrenoGCFlags)(GC_FREE | GC_COLLECT);

	AdrenoHashtable_Initialize(&f->Labels, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);
	AdrenoArray_Initialize(&f->RLabels);

	AdrenoHashtable_Set(&script->Functions, ((AdrenoValue *)script->Strings.Data[f->Function.NameIndex])->Value.String->Value, f);
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
	EmitLabelRef *ref = (EmitLabelRef *)AdrenoAlloc(sizeof(EmitLabelRef));

	ref->Offset = e->Stream.bufferPosition + 1;
	ref->Name = AdrenoStrdup(name);
	
	AdrenoArray_Add(&e->RLabels, ref);
	AdrenoEmit_EmitOp2_I4(function, op, 0);
}

unsigned int AdrenoEmit_SetLabel(AdrenoFunction *fnc, char *name)
{
	EmitFunction *e = (EmitFunction *)fnc;
	
	AdrenoHashtable_Set(&e->Labels, name, (void *)e->Stream.bufferPosition);

	return e->Labels.NodeCount - 1;
}

int AdrenoEmit_Finalize(AdrenoFunction *function)
{
	EmitFunction *e = (EmitFunction *)function;
	unsigned int i;
	
	e->Function.Bytecode = AdrenoMS_Clone(&e->Stream);
	e->Function.BytecodeSize = e->Stream.bufferSize;
	AdrenoMS_Close(&e->Stream);

	for (i = 0; i < e->RLabels.Count; i++)
	{
		unsigned int tAddr = 0;
		unsigned int offset = ((EmitLabelRef *)e->RLabels.Data[i])->Offset;
		unsigned int *addr = (unsigned int *)(&e->Function.Bytecode[offset]);
		char *name = ((EmitLabelRef *)e->RLabels.Data[i])->Name;

		if (!AdrenoHashtable_Get(&e->Labels, name, (void **)&tAddr))
			return 0;

		*addr = tAddr - (offset + 4);

		AdrenoFree(name);
		AdrenoFree((EmitLabelRef *)e->RLabels.Data[i]);
	}

	AdrenoHashtable_Destroy(&e->Labels);
	AdrenoArray_Free(&e->RLabels);

	return 1;
}
