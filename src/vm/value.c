#include <adreno/vm/vm.h>

unsigned int AdrenoValue_ValueHash(AdrenoValue *value, unsigned int size)
{
	AdrenoValue *v = AdrenoValue_GetValue(value);

	if (value->Type == AT_STRING)
		return AdrenoHashtable_Hash_Fnv(v->Value.String->Value, v->Value.String->Size);
	
	return v->Value.I4;
}

unsigned int AdrenoValue_GetValueLen(AdrenoValue *value)
{
	return 0;
}

AdrenoValue *AdrenoValue_GetValue(AdrenoValue *ref)
{
	if (ref->Type == AT_REFERENCE)
		return AdrenoValue_GetValue(ref->Value.Reference);

	return ref;
}

void AdrenoValue_CreateReference(AdrenoValue *ref, AdrenoValue *value)
{
	value = AdrenoValue_GetValue(value);
	value->ReferenceCounter++;

	ref->Type = AT_REFERENCE;
	ref->Value.Reference = value;
	ref->ReferenceCounter = 1;
}

void AdrenoValue_Dereference(AdrenoValue *value)
{
	if (value->Type == AT_REFERENCE)
	{
		AdrenoValue_Dereference(value->Value.Reference);
	}
	else
	{
		value->ReferenceCounter--;
	}

	if (value->ReferenceCounter == 0)
		AdrenoValue_Free(value);
}

int AdrenoValue_LoadNull(AdrenoValue *value)
{
	value->Type = AT_NULL;
	value->GCFlags = GC_NONE;
	value->ReferenceCounter = 1;
	value->Value.I4 = 0;

	return 1;
}

int AdrenoValue_LoadInteger(AdrenoValue *value, unsigned int i4)
{
	value->Type = AT_INTEGER;
	value->GCFlags = GC_NONE;
	value->ReferenceCounter = 1;
	value->Value.I4 = i4;

	return 1;
}

int AdrenoValue_LoadString(AdrenoValue *value, char *string, unsigned int len, int copy)
{
	AdrenoValue *rvalue;

	rvalue = (AdrenoValue *)AdrenoMemoryPool_Alloc(AdrenoVM_ValuePool);
	rvalue->Type = AT_STRING;
	rvalue->GCFlags = (AdrenoGCFlags)(GC_FREE | GC_COLLECT);
	rvalue->ReferenceCounter = 0;
	rvalue->Value.String = (AdrenoString *)AdrenoAlloc(sizeof(AdrenoString));
	rvalue->Value.String->Size = len;

	if (copy == 1)
	{
		rvalue->Value.String->Value = (char *)AdrenoAlloc(len + 1);
		rvalue->Value.String->Flags = SF_FREE;
		memcpy(rvalue->Value.String->Value, string, rvalue->Value.String->Size);
		rvalue->Value.String->Value[len] = 0;
	}
	else
	{
		rvalue->Value.String->Flags = copy == 2 ? SF_FREE : SF_NONE;
		rvalue->Value.String->Value = string;
	}
	
	AdrenoValue_CreateReference(value, rvalue);

	return 1;
}

int AdrenoValue_LoadArray(AdrenoValue *value)
{
	AdrenoValue *rvalue = (AdrenoValue *)AdrenoMemoryPool_Alloc(AdrenoVM_ValuePool);
	rvalue->Type = AT_ARRAY;
	rvalue->GCFlags = (AdrenoGCFlags)(GC_COLLECT | GC_FREE);
	rvalue->ReferenceCounter = 0;
	rvalue->Value.Array = (AdrenoVMArray *)AdrenoAlloc(sizeof(AdrenoVMArray));
	rvalue->Value.Array->Type = AT_NULL;
	AdrenoHashtable_Initialize(&rvalue->Value.Array->Array, (AdrenoHashtable_HashFunction)AdrenoValue_ValueHash, (AdrenoHashtable_LenFunction)AdrenoValue_GetValueLen);
				
	AdrenoValue_CreateReference(value, rvalue);

	return 1;
}

void AdrenoValue_Free(AdrenoValue *value)
{
	if (value->GCFlags & GC_COLLECT)
	{
		if (value->Type == AT_STRING)
		{
			if (value->Value.String->Flags & SF_FREE)
			{
				AdrenoFree(value->Value.String->Value);
				AdrenoFree(value->Value.String);
			}
		}
		else if (value->Type == AT_ARRAY)
		{
			AdrenoHashtableIterator *it;
			
			for (it = AdrenoHashtable_CreateIterator(&value->Value.Array->Array); it->CurrentNode; AdrenoHashtableIterator_Next(it))
			{
				AdrenoValue_Dereference((AdrenoValue *)it->CurrentNode->Value.Value);
			}
			AdrenoHashtableIterator_Free(it);

			AdrenoHashtable_Destroy(&value->Value.Array->Array);
			AdrenoFree(value->Value.Array);
		}
		else if (value->Type == AT_RETURNINFO)
		{
			AdrenoFree(value->Value.ReturnInfo);
		}

		value->Value.I4 = 0;
	}

	value->ReferenceCounter = 0;

	if (value->GCFlags & GC_FREE)
	{
		AdrenoMemoryPool_Free(AdrenoVM_ValuePool, value);
	}
	else if (value->GCFlags & GC_FREE_NP)
	{
		AdrenoFree(value);
	}
}
