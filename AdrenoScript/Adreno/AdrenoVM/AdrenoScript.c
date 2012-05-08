#include "AdrenoVM.h"

void AdrenoScript_Initialize(AdrenoScript *script)
{
	AdrenoHashtable_Init(&script->Functions, AdrenoHashtable_Hash_Fnv, (AdrenoHashtable_LenFunction)wcslen);
	AdrenoHashtable_Init(&script->Strings, NULL, NULL);
}

void AdrenoScript_Free(AdrenoScript *script)
{	
	AdrenoHashtable_Destroy(&script->Functions);
	AdrenoHashtable_Destroy(&script->Strings);
}
