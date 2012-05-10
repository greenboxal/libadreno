#include "AdrenoVM.h"

AdrenoValue *AdrenoValue_GetValue(AdrenoValue *ref)
{
	if (ref->Type == AT_REFERENCE)
		return AdrenoValue_GetValue(ref->Value.Reference);

	return ref;
}

void AdrenoValue_CreateReference(AdrenoValue *ref, AdrenoValue *value)
{
	value->ReferenceCounter++;

	ref->Type = AT_REFERENCE;
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
			if (value->Value.String->Flags & SF_FREE)
				AdrenoFree(value->Value.String->Value);

			AdrenoFree(value->Value.String);
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

	value->Value.I4 = 0;

	if (value->GCFlags & GC_FREE)
	{
		AdrenoFree(value);
	}
}
