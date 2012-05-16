#include <adreno/vm/vm.h>
#include <adreno/utils/memorystream.h>

#define AdrenoBinHeaderMagic	0xF00D
#define AdrenoBinHeaderVersion	0x1000

typedef struct
{
	unsigned short Magic;
	unsigned short Version;

	unsigned int FunctionCount;
	unsigned int StringCount;
} AdrenoBinHeader;

typedef struct
{
	unsigned int Size;
} AdrenoBinString;

typedef struct
{
	unsigned int NameIndex;
	unsigned int LocalsCount;
	unsigned int BytecodeSize;
} AdrenoBinFunction;

void AdrenoScript_Initialize(AdrenoScript *script)
{
	AdrenoHashtable_Initialize(&script->Functions, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);
	AdrenoArray_Initialize(&script->Strings);

	script->Functions.ExpansionFactor = 2;
}

char *AdrenoScript_Save(AdrenoScript *script, unsigned int *size)
{
	unsigned int i;
	char *result;
	AdrenoHashtableIterator *it;

	AdrenoMS ms;
	AdrenoMS_Open(&ms);

	{
		AdrenoBinHeader header;

		header.Magic = AdrenoBinHeaderMagic;
		header.Version = AdrenoBinHeaderVersion;
		header.FunctionCount = script->Functions.NodeCount;
		header.StringCount = script->Strings.Count;

		AdrenoMS_Write(&ms, (unsigned char *)&header, 0, sizeof(AdrenoBinHeader));
	}

	for (i = 0; i < script->Strings.Count; i++)
	{
		AdrenoValue *value = (AdrenoValue *)script->Strings.Data[i];
		AdrenoBinString bin;

		bin.Size = value->Value.String->Size;

		AdrenoMS_Write(&ms, (unsigned char *)&bin, 0, sizeof(AdrenoBinString));
		AdrenoMS_Write(&ms, (unsigned char *)value->Value.String->Value, 0, value->Value.String->Size);
	}

	for (it = AdrenoHashtable_CreateIterator(&script->Functions); it->CurrentNode; AdrenoHashtableIterator_Next(it))
	{
		AdrenoFunction *func = (AdrenoFunction *)it->CurrentNode->Value.Value;
		AdrenoBinFunction bin;

		bin.NameIndex = func->NameIndex;
		bin.LocalsCount = func->LocalsCount;
		bin.BytecodeSize = func->BytecodeSize;

		AdrenoMS_Write(&ms, (unsigned char *)&bin, 0, sizeof(AdrenoBinFunction));
		AdrenoMS_Write(&ms, (unsigned char *)func->Bytecode, 0, func->BytecodeSize);
	}
	AdrenoHashtableIterator_Free(it);
	
	*size = ms.bufferSize;
	result = (char *)AdrenoMS_Clone(&ms);
	AdrenoMS_Close(&ms);

	return result;
}

AdrenoScript *AdrenoScript_Load(char *data)
{
#define TAKE_DATA_PTR(var, type) var = (type *)&data[position]; position += sizeof(type)

	unsigned int i, position = 0;
	AdrenoBinHeader *header;

	AdrenoScript *as = (AdrenoScript *)AdrenoAlloc(sizeof(AdrenoScript));
	AdrenoScript_Initialize(as);
	as->GCFlags = GC_FREE;

	TAKE_DATA_PTR(header, AdrenoBinHeader);

	if (header->Magic != AdrenoBinHeaderMagic)
		return NULL;

	if (header->Version > AdrenoBinHeaderVersion)
		return NULL;

	for (i = 0; i < header->StringCount; i++)
	{
		AdrenoBinString *string;
		AdrenoValue value;

		TAKE_DATA_PTR(string, AdrenoBinString);

		AdrenoValue_LoadString(&value, &data[position], string->Size, 1);
		value.Value.Reference->GCFlags = GC_FINAL_FREE;
		
		AdrenoArray_Add(&as->Strings, value.Value.Reference);

		position += string->Size;
	}

	for (i = 0; i < header->FunctionCount; i++)
	{
		AdrenoBinFunction *function;
		AdrenoFunction *f = (AdrenoFunction *)AdrenoAlloc(sizeof(AdrenoFunction));

		TAKE_DATA_PTR(function, AdrenoBinFunction);
		
		f->Type = AF_SCRIPT;
		f->GCFlags = (AdrenoGCFlags)(GC_FREE | GC_COLLECT);
		f->NameIndex = function->NameIndex;
		f->Bytecode = (unsigned char *)AdrenoAlloc(function->BytecodeSize);
		f->BytecodeSize = function->BytecodeSize;
		f->LocalsCount = function->LocalsCount;
		f->Owner = as;
		f->Index = i;
		f->APIFunction = NULL;

		AdrenoHashtable_Set(&as->Functions, ((AdrenoValue *)as->Strings.Data[f->NameIndex])->Value.String->Value, f);

		memcpy(f->Bytecode, (unsigned char *)(data + position), f->BytecodeSize);

		position += function->BytecodeSize;
	}

	return as;
}

void AdrenoScript_Free(AdrenoScript *script)
{	
	unsigned int i;
	AdrenoHashtableIterator *it;
	
	for (it = AdrenoHashtable_CreateIterator(&script->Functions); it->CurrentNode; AdrenoHashtableIterator_Next(it))
	{
		AdrenoFunction *fnc = (AdrenoFunction *)it->CurrentNode->Value.Value;
		
		if (fnc->Type == AF_SCRIPT && fnc->GCFlags & GC_COLLECT && fnc->Bytecode)
			AdrenoFree(fnc->Bytecode);

		if (fnc->GCFlags & GC_FREE)
			AdrenoFree(fnc);
	}
	AdrenoHashtableIterator_Free(it);

	for (i = 0; i < script->Strings.Count; i++)
	{
		AdrenoValue *val = (AdrenoValue *)script->Strings.Data[i];
		
		if (val->GCFlags & GC_FINAL_FREE)
			val->GCFlags = (AdrenoGCFlags)(val->GCFlags | GC_FREE | GC_COLLECT);

		AdrenoValue_Free(val);
	}

	AdrenoHashtable_Destroy(&script->Functions);
	AdrenoArray_Free(&script->Strings);

	if (script->GCFlags & GC_FREE)
		AdrenoFree(script);
}
