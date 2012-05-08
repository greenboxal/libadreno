#ifndef ADRENOSCRIPT_H
#define ADRENOSCRIPT_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

struct adrenofunction
{
	unsigned int Index;
	unsigned int NameIndex;
	unsigned int ArgumentCount;
	unsigned int LocalsCount;

	AdrenoScript *Owner;

	unsigned char *Bytecode;
	unsigned int BytecodeSize;
};

struct adrenoscript
{
	AdrenoHashtable Functions;
	AdrenoHashtable Strings;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoScript_Initialize(AdrenoScript *script);
	extern void AdrenoScript_Free(AdrenoScript *script);

#ifdef __cplusplus
}
#endif

#endif
