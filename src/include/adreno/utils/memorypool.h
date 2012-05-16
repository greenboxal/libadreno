#ifndef ADRENOMEMORYPOOL_H
#define ADRENOMEMORYPOOL_H

#include <adreno/utils/bitarray.h>

typedef struct
{
	char *Address;
	unsigned int CommitedCount;
} AdrenoMemoryPoolPage;

typedef struct
{
	unsigned int ObjectSize;
	unsigned int ExpansionFactor;

	unsigned int TotalCount;
	unsigned int TotalMaxCount;
	
	AdrenoMemoryPoolPage *Pages;
	unsigned int PageCount;

	AdrenoBitArray FreeList;

	unsigned int Index;
	unsigned int DestroyLock;
} AdrenoMemoryPool;

#ifdef __cplusplus
extern "C"
{
#endif
	
	extern AdrenoMemoryPool *AdrenoMemoryPool_New(unsigned int objectSize, unsigned int expansionFactor);
	extern void AdrenoMemoryPool_Initialize(AdrenoMemoryPool *mp, unsigned int objectSize, unsigned int expansionFactor);
	extern void *AdrenoMemoryPool_Alloc(AdrenoMemoryPool *mp);
	extern void AdrenoMemoryPool_Free(AdrenoMemoryPool *mp, void *ptr);
	extern void AdrenoMemoryPool_Destroy(AdrenoMemoryPool *mp);

#ifdef __cplusplus
}
#endif

#endif
