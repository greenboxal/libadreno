#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>

#include <adreno/vm/vm.h>
#include <adreno/vm/emit.h>
#include <adreno/ail/ailc.h>

#include <Windows.h>

char *LoadInputFile(char *FileName) 
{
	FILE *Fin;
	char *Buf1;
	struct stat statbuf;
	size_t BytesRead;
	unsigned long i;

	/* Sanity check. */
	if ((FileName == NULL) || (*FileName == '\0')) return(NULL);

	/* Open the file. */
	Fin = fopen(FileName,"rb");
	if (Fin == NULL) 
	{
		fprintf(stdout,"Could not open input file: %s\n",FileName);
		return(NULL);
	}

	/* Get the size of the file. */
	if (fstat(_fileno(Fin),&statbuf) != 0) 
	{
		fprintf(stdout,"Could not stat() the input file: %s\n",FileName);
		fclose(Fin);
		return(NULL);
	}

	/* Allocate memory for the input. */
	Buf1 = (char *)AdrenoAlloc(statbuf.st_size + 1);
	if ((Buf1 == NULL)) 
	{
		fprintf(stdout,"Not enough memory to load the file: %s\n",FileName);
		fclose(Fin);
		if (Buf1 != NULL) free(Buf1);
		return(NULL);
	}

	/* Load the file into memory. */
	BytesRead = fread(Buf1,1,statbuf.st_size,Fin);
	Buf1[BytesRead] = '\0';

	/* Close the file. */
	fclose(Fin);

	/* Exit if there was an error while reading the file. */
	if (BytesRead != statbuf.st_size) 
	{
		fprintf(stdout,"Error while reading input file: %s\n",FileName);
		free(Buf1);
		return(NULL);
	}

	return Buf1;
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
	AdrenoScript *script;
	AilCompiler c;
	long double start;
	int j;
	
#ifdef USE_DEBUG_MALLOC
	malloc_init();
#endif

	AdrenoVM_Initialize(&vm);
	AdrenoContext_Initialize(&ctx);

	AilCompiler_Initialize(&c, LoadInputFile("input.txt"));
	script = AilCompiler_Compile(&c);
	AilCompiler_Free(&c);
	AdrenoFree(c.Data);
getchar();
	AdrenoContext_AttachScript(&ctx, script);

	start = GetTime();
	for (j = 0; j < 1000; j++)
	{
		AdrenoContext_SetFunction(&ctx, (AdrenoFunction *)script->Functions.NodeHeap[0].Value.Value);
 		AdrenoVM_Run(&vm, &ctx);
	}
	start = GetTime() - start;
	printf("Time: %Lf\n", start);

	AdrenoScript_Free(script);
	AdrenoContext_Free(&ctx);
	AdrenoVM_Free(&vm);

#ifdef USE_DEBUG_MALLOC
	malloc_final();
#endif

	getchar();
	return 0;
}
