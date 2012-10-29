#include "testrunner.h"

#include <adreno/memory.h>
#include <adreno/utils/array.h>
#include <adreno/utils/hashtable.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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


TEST( HashTableUsage )
{
	AdrenoHashtable ht;
	AdrenoArray arr;
	int i;
	char tmp[30];
	const int ITERATIONS = 10000;

#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

	AdrenoHashtable_Initialize(&ht, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);
	AdrenoArray_Initialize(&arr);

	for (i = 0; i < ITERATIONS; i++)
	{
		rand_str(tmp, sizeof(tmp));
		AdrenoArray_Add(&arr, AdrenoStrdup(tmp));
	}

	for (i = 0; i < ITERATIONS; i++)
	{
		AdrenoHashtable_Set(&ht, arr.Data[i], arr.Data[i]);
	}

	for (i = 0; i < ITERATIONS; i++)
	{
		AdrenoHashtable_Remove(&ht, arr.Data[i]);
	}

	while(arr.Count)
	{
		AdrenoFree(arr.Data[0]);
		AdrenoArray_Remove(&arr, 0);
	}

	AdrenoHashtable_Destroy(&ht);
	AdrenoArray_Free(&arr);

#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif
}
