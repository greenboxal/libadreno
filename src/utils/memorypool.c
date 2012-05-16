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

#include <adreno/utils/memorypool.h>
#include <adreno/memory.h>

#include <stdlib.h>

#define PAGE_SIZE 4096

// This will force the use of libadreno memory manager for allocate memory for the pool
//#define FORCE_MEMMGR_ALLOC

static AdrenoMemoryPool **MPools;
static unsigned int MPoolsCount = 0;

#if defined(_WIN32) && !defined(FORCE_MEMMGR_ALLOC)
#include <Windows.h>

void *AP_ReservePage(int count)
{
	int size = count * PAGE_SIZE;

	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

void *AP_CommitPage(void *addr, int count)
{
	int size = count * PAGE_SIZE;
	
	return VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
}

void AP_FreePage(void *addr, int count)
{
	int size = count * PAGE_SIZE;
	
	VirtualAlloc(addr, size, MEM_FREE, PAGE_READWRITE);
}
#else
void *AP_ReservePage(int count)
{
	int size = count * PAGE_SIZE;

	return AdrenoAlloc(size);
}

void *AP_CommitPage(void *addr, int count)
{
	return addr;
}

void AP_FreePage(void *addr, int count)
{
	AdrenoFree(addr);
}
#endif

void AdrenoMemoryPool_Expand(AdrenoMemoryPool *mp)
{
	int nIdx = mp->PageCount;

	mp->PageCount++;
	mp->Pages = (AdrenoMemoryPoolPage *)AdrenoRealloc(mp->Pages, mp->PageCount * sizeof(AdrenoMemoryPoolPage));
		
	mp->Pages[nIdx].Address = (char *)AP_ReservePage(mp->ExpansionFactor);
	mp->Pages[nIdx].CommitedCount = 0;

	// For now commit the pages too
	AP_CommitPage(mp->Pages[nIdx].Address, mp->ExpansionFactor);

	mp->TotalMaxCount += mp->ExpansionFactor * PAGE_SIZE / mp->ObjectSize;

	AdrenoBitArray_Resize(&mp->FreeList, mp->TotalMaxCount);
}

AdrenoMemoryPool *AdrenoMemoryPool_New(unsigned int objectSize, unsigned int expansionFactor)
{
	unsigned int i;
	AdrenoMemoryPool *addr;

	if (objectSize % 2)
		objectSize++;

	for (i = 0; i < MPoolsCount; i++)
	{
		if (MPools[i] && MPools[i]->ObjectSize == objectSize && MPools[i]->ExpansionFactor == expansionFactor)
		{
			MPools[i]->DestroyLock++;

			return MPools[i];
		}
	}

	MPoolsCount++;
	MPools = (AdrenoMemoryPool **)AdrenoRealloc(MPools, MPoolsCount * sizeof(AdrenoMemoryPool *));

	addr = (AdrenoMemoryPool *)AdrenoAlloc(sizeof(AdrenoMemoryPool));
	AdrenoMemoryPool_Initialize(addr, objectSize, expansionFactor);
	
	addr->Index = MPoolsCount - 1;
	MPools[MPoolsCount - 1] = addr;

	return addr;
}

void AdrenoMemoryPool_Initialize(AdrenoMemoryPool *mp, unsigned int objectSize, unsigned int expansionFactor)
{
	if (objectSize % 2)
		objectSize++;

	mp->ObjectSize = objectSize;
	mp->ExpansionFactor = 0;

	mp->TotalCount = 0;
	mp->TotalMaxCount = 0;

	while (mp->ExpansionFactor * PAGE_SIZE < objectSize)
		mp->ExpansionFactor++;

	mp->ExpansionFactor *= expansionFactor;

	mp->Pages = NULL;
	mp->PageCount = 0;

	mp->Index = -1;
	mp->DestroyLock = 1;

	AdrenoBitArray_Initialize(&mp->FreeList, 0);
	AdrenoMemoryPool_Expand(mp);
}

void *AdrenoMemoryPool_Alloc(AdrenoMemoryPool *mp)
{
	int freeIdx;
	int total, total2;
	int pageIndex, pageOffset;

	if (mp->TotalCount >= mp->TotalMaxCount)
	{
		AdrenoMemoryPool_Expand(mp);

		return AdrenoMemoryPool_Alloc(mp);
	}
	
	freeIdx = AdrenoBitArray_Search(&mp->FreeList);
	total = mp->ExpansionFactor * PAGE_SIZE;
	total2 = mp->ObjectSize * freeIdx;

	pageIndex = total2 / total;
	pageOffset = total2 % total;

	mp->TotalCount++;
	AdrenoBitArray_Set(&mp->FreeList, freeIdx);

	return (void *)&mp->Pages[pageIndex].Address[pageOffset];
}

void AdrenoMemoryPool_Free(AdrenoMemoryPool *mp, void *ptr)
{
	unsigned int i = 0;

	for (i = 0; i < mp->PageCount; i++)
	{
		if ((unsigned int)ptr >= (unsigned int)mp->Pages[i].Address &&
			(unsigned int)ptr < (unsigned int)(mp->Pages[i].Address + mp->ExpansionFactor * PAGE_SIZE))
		{
			int pageOffset, freeIdx;

			pageOffset = ((unsigned int)ptr - (unsigned int)mp->Pages[i].Address);
			freeIdx = ((i * mp->ExpansionFactor * PAGE_SIZE) / mp->ObjectSize) + (pageOffset / mp->ObjectSize);

			mp->TotalCount--;
			AdrenoBitArray_Unset(&mp->FreeList, freeIdx);

			break;
		}
	}
}

void AdrenoMemoryPool_Destroy(AdrenoMemoryPool *mp)
{
	unsigned int i = 0, freeIt = 0;

	if (--mp->DestroyLock > 0)
		return;

	for (i = 0; i < mp->PageCount; i++)
	{
		AP_FreePage((void *)mp->Pages[i].Address, mp->ExpansionFactor * PAGE_SIZE);
	}
	
	if (mp->Pages)
		AdrenoFree(mp->Pages);

	AdrenoBitArray_Free(&mp->FreeList);

	if (mp->Index >= 0 && mp->DestroyLock == 0)
	{
		MPools[mp->Index] = NULL;
		MPoolsCount--;

		if (MPoolsCount == 0)
		{
			AdrenoFree(MPools);
			MPools = NULL;
		}

		AdrenoFree(mp);
	}
}
