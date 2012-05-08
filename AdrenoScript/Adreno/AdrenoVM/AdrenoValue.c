#include "AdrenoVM.h"

AdrenoValue *AdrenoValue_GetValue(AdrenoValue *ref)
{
	if (ref->Type == AT_REFERENCE)
		return AdrenoValue_GetValue(ref->Value.Reference);

	return ref;
}

void AdrenoValue_CreateReference(AdrenoValue *ref, AdrenoValue *value)
{
	ref->GCFlags = GC_NONE;
	ref->Type = AT_REFERENCE;
	ref->ReferenceCounter = 1;
	ref->Value.Reference = value;
}

void AdrenoValue_Dereference(AdrenoValue *value)
{
	value->ReferenceCounter--;

	if (value->Type == AT_REFERENCE)
		AdrenoValue_Dereference(value->Value.Reference);

	if (value->ReferenceCounter <= 0)
		AdrenoValue_Free(value);
}

void AdrenoValue_Free(AdrenoValue *value)
{
	if (value->GCFlags & GC_COLLECT)
	{
		if (value->Type == AT_STRING)
		{
			AdrenoFree(value->Value.String.Value);
		}
		else if (value->Type == AT_ARRAY)
		{
			AdrenoHashtable_Destroy(&value->Value.Array->Array);
			AdrenoFree(value->Value.Array);
		}
		else if (value->Type == AT_RETURNINFO)
		{
			AdrenoFree(value->Value.ReturnInfo);
		}
	}

	if (value->GCFlags & GC_FREE)
	{
		AdrenoFree(value);
	}
}
