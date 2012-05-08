#ifndef ADRENOVALUE_H
#define ADRENOVALUE_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

#include <wchar.h>

typedef enum
{
    GC_NONE = 0x00000000,
    GC_COLLECT = 0x00000001,
    GC_FREE = 0x00000002,
} AdrenoGCFlags;

typedef enum
{
    AT_NULL,
    AT_INTEGER,
    AT_STRING,
    AT_ARRAY,
    AT_REFERENCE,
    AT_FUNCTION,
    AT_RETURNINFO,
} AdrenoType;

struct adrenostring
{
	wchar_t *Value;
	unsigned int Size;
};

struct adrenoarray
{
	AdrenoType Type;
	AdrenoHashtable Array;
};

struct adrenoretvalue
{
	AdrenoScript *Script;
	AdrenoFunction *Function;
	unsigned int InstructionPointer;
	AdrenoValue *Locals;
};

struct adrenovalue
{
	AdrenoType Type;

	AdrenoGCFlags GCFlags;
	unsigned int ReferenceCounter;

	union
	{
		unsigned int I4;
		AdrenoString String;
		AdrenoArray *Array;
		AdrenoValue *Reference;
		AdrenoFunction *Function;
		AdrenoReturnInfo *ReturnInfo;
	} Value;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern AdrenoValue *AdrenoValue_GetValue(AdrenoValue *ref);
	extern void AdrenoValue_CreateReference(AdrenoValue *ref, AdrenoValue *value);
	extern void AdrenoValue_Dereference(AdrenoValue *value);
	extern void AdrenoValue_Free(AdrenoValue *value);

#ifdef __cplusplus
}
#endif

#endif
