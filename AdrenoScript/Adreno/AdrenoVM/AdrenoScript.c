#include "AdrenoVM.h"

void AdrenoScript_Initialize(AdrenoScript *script)
{
	AdrenoHashtable_Init(&script->Functions, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_WString);
	AdrenoHashtable_Init(&script->Strings, NULL, NULL);

	script->Functions.ExpansionFactor = 2;
	script->Strings.ExpansionFactor = 2;
}

void AdrenoScript_Free(AdrenoScript *script)
{	
	AdrenoHashtable_Destroy(&script->Functions);
	AdrenoHashtable_Destroy(&script->Strings);
}
