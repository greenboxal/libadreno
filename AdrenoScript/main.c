#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include <Windows.h>

#include "Adreno/AdrenoVM/AdrenoVM.h"
#include "Adreno/AdrenoVM/AdrenoEmit.h"

char *readFile(char *file)
{
	FILE *fp = fopen(file, "r");
	char *buff = NULL;
	int total = 0, read = 0;

	do
	{
		total += read;
		buff = (char *)realloc(buff, total + 4096);
	}
	while ((read = fread(&buff[total], 1, 4096, fp)) > 0);

	buff = (char *)realloc(buff, total + 1);
	buff[total] = 0;

	return buff;
}

long double GetTime()
{
	LARGE_INTEGER frequency;
	LARGE_INTEGER time;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);

	return (long double)time.QuadPart / (long double)frequency.QuadPart;
}

int main(int argc, char **argv)
{
	/*adreno_compiler compiler;
	adrenovm vm;
	char *source = readFile("input.txt");
	int size = 0;
	char *data;

	adreno_compiler_init(&compiler, source);
	adreno_compiler_compile(&compiler);
	data = adreno_compiler_save(&compiler, &size);
	adreno_compiler_free(&compiler);

	adreno_init(&vm);
	adreno_load_script(&vm, data, size);
	adreno_run(&vm);
	adreno_free(&vm);*/

	AdrenoVM vm;
	AdrenoContext ctx;
	AdrenoScript script;
	AdrenoFunction *function;
	long double start;
	int j;

	AdrenoVM_Initialize(&vm);
	AdrenoContext_Initialize(&ctx);
	AdrenoScript_Initialize(&script);

	function = AdrenoEmit_CreateFunction(&script, L"<default>");
	function->LocalsCount = 1;
	{
		AdrenoEmit_EmitOp(function, OP_ENTER);
		AdrenoEmit_EmitOp2_I4(function, OP_LDC_I4, 3);
		AdrenoEmit_EmitOp2_I4(function, OP_LDC_I4, 5);
		AdrenoEmit_EmitOp(function, OP_ADD);
		AdrenoEmit_EmitOp2_I4(function, OP_LDC_I4, 2);
		AdrenoEmit_EmitOp(function, OP_DIV);
		//AdrenoEmit_EmitOp2_I4(function, OP_LDC_I4, 3);
		//AdrenoEmit_EmitOp(function, OP_REM);
		AdrenoEmit_EmitOp(function, OP_STLOC_0);
		AdrenoEmit_EmitOp(function, OP_LDNULL);
		AdrenoEmit_EmitOp(function, OP_RET);
	}

	AdrenoVM_AttachScript(&vm, &script);
	AdrenoContext_AttachScript(&ctx, &script);

	start = GetTime();
	for (j = 0; j < 100000; j++)
	{
		AdrenoContext_SetFunction(&ctx, function);
 		AdrenoVM_Run(&vm, &ctx);
	}
	start = GetTime() - start;
	printf("Time: %Lf", start);

	AdrenoScript_Free(&script);
	AdrenoContext_Free(&ctx);
	AdrenoVM_Free(&vm);

	getchar();
	return 0;
}
