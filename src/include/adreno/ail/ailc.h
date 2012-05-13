#ifndef AILC_H
#define AILC_H

#include <adreno/vm/vm.h>
#include <stdio.h>

typedef struct
{
	char *Data;
	unsigned int DataPosition, DataSize;
	AdrenoScript *Script;
	AdrenoFunction *CurrentFunction;
	AdrenoOpcodePrefixes Prefix;
} AilCompiler;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AilCompiler_Initialize(AilCompiler *compiler, char *data);
	extern AdrenoScript *AilCompiler_Compile(AilCompiler *compiler);
	extern void AilCompiler_Free(AilCompiler *compiler);

#ifdef __cplusplus
}
#endif

#endif
