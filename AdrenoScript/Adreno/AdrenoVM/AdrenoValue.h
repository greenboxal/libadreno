#ifndef ADRENOVALUE_H
#define ADRENOVALUE_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

#include <wchar.h>

typedef enum
{
	/* None action should be done */
    GC_NONE = 0x00000000,

	/* The GC should free the referenced memory in the value field */
    GC_COLLECT = 0x00000001,

	/* The GC should free the pointer to the AdrenoValue struct */
    GC_FREE = 0x00000002,
} AdrenoGCFlags;

typedef enum
{
	/* Null Object */
    AT_NULL,

	/* Double word */
    AT_INTEGER,
	
	/* String */
    AT_STRING,

	/* Array */
    AT_ARRAY,
	
	/* Reference */
    AT_REFERENCE,
	
	/* Reference to a function */
    AT_FUNCTION,

	/* Function call return info */
    AT_RETURNINFO,
} AdrenoType;

typedef enum
{
	/* None flag associated with this string, probably a string from the string pool */
	SF_NONE = 0x00000000,

	/* The string should be freed by the GC */
	SF_FREE = 0x00000001,
} AdrenoStringFlags;

struct adrenostring
{
	AdrenoStringFlags Flags;
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
		AdrenoString *String;
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
