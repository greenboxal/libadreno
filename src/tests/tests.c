/*	This file is part of libadreno.

    libadreno is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libadreno is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <adreno/vm/vm.h>
#include <adreno/vm/emit.h>
#include <adreno/ail/ailc.h>

#include <adreno/utils/array.h>
#include <adreno/utils/memorypool.h>
#include <adreno/utils/hashtable.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

// 0 = VM
// 1 = Memory Pool
// 2 = Hashtable + Array
#define TEST_TYPE 2

char *LoadInputFile(char *FileName) 
{
	FILE *Fin;
	char *Buf1;
	struct stat statbuf;
	size_t BytesRead;

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
	if (fstat(fileno(Fin),&statbuf) != 0) 
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
#ifdef _MSC_VER
	LARGE_INTEGER frequency;
	LARGE_INTEGER time;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);

	return (long double)time.QuadPart / (long double)frequency.QuadPart;
#else
	struct timeval tm;
	gettimeofday( &tm, 0 );

	return tm.tv_sec + (long double)tm.tv_usec / 1000000;
#endif
}

#if TEST_TYPE == 0

int main(int argc, char **argv)
{
	AdrenoVM vm;
	AdrenoContext ctx;
	AdrenoScript *script;
	AilCompiler c;
	long double start;
	int j;
	unsigned int size;
	char *data;
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

	AdrenoVM_StaticInit();

	AdrenoVM_Initialize(&vm);
	AdrenoContext_Initialize(&ctx);

	AdrenoVM_LoadStdlib(&vm);

// 0 = Read Binary
// 1 = Compile
#define S_TYPE 0

#if S_TYPE == 0
	data = LoadInputFile("test.bin");
	script = AdrenoScript_Load(data);
	AdrenoFree(data);
#elif S_TYPE == 1
	AilCompiler_Initialize(&c, LoadInputFile("input.txt"));
	script = AilCompiler_Compile(&c);
	AilCompiler_Free(&c);
	AdrenoFree(c.Data);
#endif

	AdrenoContext_AttachScript(&ctx, script);

	start = GetTime();
	for (j = 0; j < 1; j++)
	{
		AdrenoContext_SetFunctionByName(&ctx, "main");
 		AdrenoVM_Run(&vm, &ctx);
	}
	start = GetTime() - start;
	printf("Time: %Lf\n", start);

	AdrenoScript_Free(script);
	AdrenoContext_Free(&ctx);
	AdrenoVM_Free(&vm);

	AdrenoVM_StaticDestroy();

#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif

	getchar();
	return 0;
}

#elif TEST_TYPE == 1

int main(int argc, char **argv)
{
	AdrenoMemoryPool *pool;
	void *a;
	long double start;
	int i;
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

#define dosize 50
#define docount 10000
#define dofree

	pool = AdrenoMemoryPool_New(dosize, 1);
	
	start = GetTime();
	for (i = 0; i < docount; i++)
	{
		a = AdrenoAlloc(dosize);
#ifdef dofree
		AdrenoFree(a);
#endif
	}
	start = GetTime() - start;
	printf("memmgr: %Lf\n", start);

	start = GetTime();
	for (i = 0; i < docount; i++)
	{
		a = AdrenoMemoryPool_Alloc(pool);
#ifdef dofree
		AdrenoMemoryPool_Free(pool, a);
#endif
	}
	start = GetTime() - start;
	printf("AdrenoMemoryPool: %Lf\n", start);

	start = GetTime();
	for (i = 0; i < docount; i++)
	{
		a = malloc(dosize);
#ifdef dofree
		free(a);
#endif
	}
	start = GetTime() - start;
	printf("malloc: %Lf\n", start);

	AdrenoMemoryPool_Destroy(pool);

#ifndef dofree
	getchar();
#endif
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif

	getchar();
	return 0;
}

#elif TEST_TYPE == 2
char *rand_str(char *dst, int size)
{
   static const char text[] = "abcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							  "0123456789"
							  "!@#$%¨&*()_";
   int i, len = rand() % (size - 1);
   for ( i = 0; i < len; ++i )
   {
      dst[i] = text[rand() % (sizeof text - 1)];
   }
   dst[i] = '\0';
   return dst;
}

int main(int argc, char **argv)
{
	AdrenoHashtable ht;
	AdrenoArray arr;
	int i;
	char tmp[30];
	long double time;
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

	AdrenoHashtable_Initialize(&ht, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);
	AdrenoArray_Initialize(&arr);
	
#define docount 10000

	time = GetTime();
	for (i = 0; i < docount; i++)
	{
		rand_str(tmp, sizeof(tmp));
		AdrenoArray_Add(&arr, AdrenoStrdup(tmp));
	}
	time = GetTime() - time;
	printf("Array add with rand_str + strdup: %Lf\n", time);
	
	time = GetTime();
	for (i = 0; i < docount; i++)
	{
		AdrenoHashtable_Set(&ht, arr.Data[i], arr.Data[i]);
	}
	time = GetTime() - time;
	printf("Hashtable add: %Lf\n", time);
	
	time = GetTime();
	for (i = 0; i < docount; i++)
	{
		AdrenoHashtable_Remove(&ht, arr.Data[i]);
	}
	time = GetTime() - time;
	printf("Hashtable remove: %Lf\n", time);

	time = GetTime();
	while(arr.Count)
	{
		AdrenoFree(arr.Data[0]);
		AdrenoArray_Remove(&arr, 0);
	}
	time = GetTime() - time;
	printf("Array remove + free: %Lf\n", time);

	AdrenoHashtable_Destroy(&ht);
	AdrenoArray_Free(&arr);
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif

	getchar();
	return 0;
}
#endif
