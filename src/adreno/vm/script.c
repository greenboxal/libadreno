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

#include <adreno/vm/vm.h>
#include <adreno/utils/memorystream.h>

#include <string.h>

#define AdrenoBinHeaderMagic	0xF00D
#define AdrenoBinHeaderVersion	0x1000

typedef struct
{
	unsigned short Magic;
	unsigned short Version;

	size_t FunctionCount;
	size_t StringCount;
} AdrenoBinHeader;

typedef struct
{
	size_t Size;
} AdrenoBinString;

typedef struct
{
	size_t NameIndex;
	size_t LocalsCount;
	size_t BytecodeSize;
} AdrenoBinFunction;

void AdrenoScript_Initialize(AdrenoScript *script)
{
	AdrenoHashtable_Initialize(&script->Functions, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);
	AdrenoArray_Initialize(&script->Strings);

	script->Functions.ExpansionFactor = 2;
}

char *AdrenoScript_Save(AdrenoScript *script, size_t *size)
{
	size_t i;
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

	size_t i, position = 0;
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
	size_t i;
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
