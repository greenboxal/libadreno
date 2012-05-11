#ifndef AILC_H
#define AILC_H

#include <adreno/vm/vm.h>

typedef struct
{
	wchar_t *data;
	AdrenoScript *script;
	AdrenoFunction *currentFunction;

	AdrenoOpcodePrefixes currentPrefix;
} AilCompiler;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AilCompiler_Initialize(AilCompiler *compiler, wchar_t *data);
	extern AdrenoScript *AilCompiler_Compile(AilCompiler *compiler);
	extern void AilCompiler_Free(AilCompiler *compiler);

#ifdef __cplusplus
}
#endif

#endif
