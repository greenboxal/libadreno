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

#include <adreno/config.h>
#include <adreno/utils/bitarray.h>

#include <stddef.h> // size_t

typedef struct
{
	char *Address;
#ifdef ADRENOMP_USE_LINKED_LIST
	size_t UseCount;
#endif
} AdrenoMemoryPoolPage;

typedef struct
{
	size_t PageSize;
	size_t ObjectSize;
	size_t ExpansionFactor;

	size_t TotalCount;
	size_t TotalMaxCount;
	
	AdrenoMemoryPoolPage *Pages;
	size_t PageCount;

#ifdef ADRENOMP_USE_LINKED_LIST
	size_t *Reuse;
#else
	AdrenoBitArray FreeList;
#endif

	size_t Index;
	size_t DestroyLock;
} AdrenoMemoryPool;

#ifdef __cplusplus
extern "C"
{
#endif
	
	extern AdrenoMemoryPool *AdrenoMemoryPool_New(size_t objectSize, size_t expansionFactor);
	extern void AdrenoMemoryPool_Initialize(AdrenoMemoryPool *mp, size_t objectSize, size_t expansionFactor);
	extern void *AdrenoMemoryPool_Alloc(AdrenoMemoryPool *mp);
	extern void AdrenoMemoryPool_Free(AdrenoMemoryPool *mp, void *ptr);
	extern void AdrenoMemoryPool_Destroy(AdrenoMemoryPool *mp);

#ifdef __cplusplus
}
#endif

#endif
