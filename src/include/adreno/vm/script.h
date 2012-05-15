#ifndef ADRENOSCRIPT_H
#define ADRENOSCRIPT_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

typedef enum
{
	AF_SCRIPT,
	AF_API,
} AdrenoFunctionType;

struct adrenofunction
{
	AdrenoFunctionType Type;
	unsigned int Index;
	unsigned int NameIndex;
	unsigned int LocalsCount;

	AdrenoScript *Owner;

	AdrenoGCFlags GCFlags;
	unsigned char *Bytecode;
	unsigned int BytecodeSize;

	AdrenoAPIFunction APIFunction;
};

struct adrenoscript
{
	AdrenoGCFlags GCFlags;

	AdrenoHashtable Functions;
	AdrenoHashtable Strings;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoScript_Initialize(AdrenoScript *script);
	extern char *AdrenoScript_Save(AdrenoScript *script, unsigned int *size);
	extern AdrenoScript *AdrenoScript_Load(char *data);
	extern void AdrenoScript_Free(AdrenoScript *script);

#ifdef __cplusplus
}
#endif

#endif
