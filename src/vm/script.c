#include <adreno/vm/vm.h>

void AdrenoScript_Initialize(AdrenoScript *script)
{
	AdrenoHashtable_Initialize(&script->Functions, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_WString);
	AdrenoHashtable_Initialize(&script->Strings, NULL, NULL);

	script->Functions.ExpansionFactor = 2;
	script->Strings.ExpansionFactor = 2;
}

void AdrenoScript_Free(AdrenoScript *script)
{	
	unsigned int i;

	for (i = 0; i < script->Functions.NodeCount; i++)
	{
		AdrenoFunction *fnc = (AdrenoFunction *)script->Functions.NodeHeap[i].Value.Value;
		
		if (fnc->GCFlags & GC_COLLECT && fnc->Bytecode)
			AdrenoFree(fnc->Bytecode);

		if (fnc->GCFlags & GC_FREE)
			AdrenoFree(fnc);
	}

	for (i = 0; i < script->Strings.NodeCount; i++)
	{
		AdrenoValue *val = (AdrenoValue *)script->Strings.NodeHeap[i].Value.Value;
		
		if (val->GCFlags & GC_FINAL_FREE)
			val->GCFlags = (AdrenoGCFlags)(val->GCFlags | GC_FREE | GC_COLLECT);

		AdrenoValue_Free(val);
	}

	AdrenoHashtable_Destroy(&script->Functions);
	AdrenoHashtable_Destroy(&script->Strings);

	if (script->GCFlags & GC_FREE)
		AdrenoFree(script);
}
