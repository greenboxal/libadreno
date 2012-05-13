#include <adreno/ail/ailc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ailp.h"

void AilCompiler_Initialize(AilCompiler *compiler, char *data)
{
	compiler->Data = data;
	compiler->DataPosition = 0;
	compiler->DataSize = strlen(data);
}

AdrenoScript *AilCompiler_Compile(AilCompiler *compiler)
{
	// Initialize the script object
	compiler->Script = (AdrenoScript *)AdrenoAlloc(sizeof(AdrenoScript));
	AdrenoScript_Initialize(compiler->Script);
	compiler->Script->GCFlags = GC_FREE;

	ail_setup_buffer(compiler->Data);

	if (ail_parse(compiler) != 0)
	{
		ail_free_buffer();
		AdrenoScript_Free(compiler->Script);
		AdrenoFree(compiler->Script);
		return NULL;
	}

	ail_free_buffer();

	return compiler->Script;
}

void AilCompiler_Free(AilCompiler *compiler)
{
	
}
