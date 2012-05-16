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
	unsigned int FakeObjectSize;
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
