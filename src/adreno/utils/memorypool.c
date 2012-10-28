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
#include <string.h>

static AdrenoMemoryPool **MPools;
static unsigned int MPoolsCount = 0;

#if defined(_WIN32) && !defined(FORCE_MEMMGR_ALLOC)
#include <Windows.h>

void *AP_ReservePage(int count)
{
	int size = count * ADRENOMP_PAGE_SIZE;

	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

void *AP_CommitPage(void *addr, int count)
{
	int size = count * ADRENOMP_PAGE_SIZE;
	
	return VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
}

void AP_FreePage(void *addr, int count)
{
	int size = count * ADRENOMP_PAGE_SIZE;
	
	VirtualAlloc(addr, size, MEM_FREE, PAGE_READWRITE);
}
#else
void *AP_ReservePage(int count)
{
	int size = count * ADRENOMP_PAGE_SIZE;

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

	// For now commit the pages too
	AP_CommitPage(mp->Pages[nIdx].Address, mp->ExpansionFactor);

#ifdef ADRENOMP_USE_LINKED_LIST
	*((unsigned int *)mp->Pages[nIdx].Address) = 0;
	mp->Pages[nIdx].UseCount = 0;
#endif

	mp->TotalMaxCount += mp->ExpansionFactor * mp->PageSize / mp->ObjectSize;

#ifndef ADRENOMP_USE_LINKED_LIST
	AdrenoBitArray_Resize(&mp->FreeList, mp->TotalMaxCount);
#endif
}

AdrenoMemoryPool *AdrenoMemoryPool_New(unsigned int objectSize, unsigned int expansionFactor)
{
	unsigned int i;
	AdrenoMemoryPool *addr;

	if (objectSize % 2)
		objectSize++;

#ifdef ADRENOMP_USE_LINKED_LIST
	objectSize += 4;
#endif

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
	AdrenoMemoryPool_Initialize(addr, objectSize - 4, expansionFactor);
	
	addr->Index = MPoolsCount - 1;
	MPools[MPoolsCount - 1] = addr;

	return addr;
}

void AdrenoMemoryPool_Initialize(AdrenoMemoryPool *mp, unsigned int objectSize, unsigned int expansionFactor)
{
	if (objectSize % 2)
		objectSize++;
	
#ifdef ADRENOMP_USE_LINKED_LIST
	objectSize += 4;
#endif

	mp->PageSize = (ADRENOMP_PAGE_SIZE / objectSize) * objectSize;
	mp->ObjectSize = objectSize;
	mp->ExpansionFactor = 0;

	while (mp->ExpansionFactor * mp->PageSize < objectSize)
		mp->ExpansionFactor++;

	mp->ExpansionFactor *= expansionFactor;

	mp->Pages = NULL;
	mp->PageCount = 0;

	mp->TotalCount = 0;
	mp->TotalMaxCount = 0;

	mp->Index = -1;
	mp->DestroyLock = 1;

#ifdef ADRENOMP_USE_LINKED_LIST
	mp->Reuse = 0;
#else
	AdrenoBitArray_Initialize(&mp->FreeList, 0);
#endif
	AdrenoMemoryPool_Expand(mp);
}

void *AdrenoMemoryPool_Alloc(AdrenoMemoryPool *mp)
{
#ifdef ADRENOMP_USE_LINKED_LIST
	if (mp->Reuse)
	{
		void *ret = (void *)(mp->Reuse + 1);

		mp->Reuse = (unsigned int *) (size_t)*mp->Reuse;

		return ret;
	}
	else if (mp->PageCount && mp->Pages[mp->PageCount - 1].UseCount < mp->ExpansionFactor * mp->PageSize / mp->ObjectSize)
	{
		return (void *)(&mp->Pages[mp->PageCount - 1].Address[mp->Pages[mp->PageCount - 1].UseCount++ * mp->ObjectSize] + sizeof(unsigned int));
	}
	else
	{
		AdrenoMemoryPool_Expand(mp);

		return AdrenoMemoryPool_Alloc(mp);
	}
#else
	int freeIdx;
	int total, total2;
	int pageIndex, pageOffset;

	if (mp->TotalCount >= mp->TotalMaxCount)
	{
		AdrenoMemoryPool_Expand(mp);

		return AdrenoMemoryPool_Alloc(mp);
	}


	freeIdx = AdrenoBitArray_Search(&mp->FreeList);
	total = mp->ExpansionFactor * mp->PageSize;
	total2 = mp->ObjectSize * freeIdx;

	pageIndex = total2 / total;
	pageOffset = total2 % total;

	mp->TotalCount++;
	AdrenoBitArray_Set(&mp->FreeList, freeIdx);

	return (void *)&mp->Pages[pageIndex].Address[pageOffset];
#endif
}

void AdrenoMemoryPool_Free(AdrenoMemoryPool *mp, void *ptr)
{
#ifdef ADRENOMP_USE_LINKED_LIST
	unsigned int *reuse = (unsigned int *)((unsigned int)ptr - 4);
	*reuse = (unsigned int)mp->Reuse;
	mp->Reuse = reuse;
#else
	unsigned int i = 0;

	for (i = 0; i < mp->PageCount; i++)
	{
		if ((unsigned int)ptr >= (unsigned int)mp->Pages[i].Address &&
			(unsigned int)ptr < (unsigned int)(mp->Pages[i].Address + mp->ExpansionFactor * mp->PageSize))
		{
			int pageOffset, freeIdx;

			pageOffset = ((unsigned int)ptr - (unsigned int)mp->Pages[i].Address);
			freeIdx = ((i * mp->ExpansionFactor * mp->PageSize) / mp->ObjectSize) + (pageOffset / mp->ObjectSize);

			mp->TotalCount--;
			AdrenoBitArray_Unset(&mp->FreeList, freeIdx);

			break;
		}
	}
#endif
}

void AdrenoMemoryPool_Destroy(AdrenoMemoryPool *mp)
{
	unsigned int i = 0, freeIt = 0;

	if (--mp->DestroyLock > 0)
		return;

	for (i = 0; i < mp->PageCount; i++)
	{
		AP_FreePage((void *)mp->Pages[i].Address, mp->ExpansionFactor);
	}
	
	if (mp->Pages)
		AdrenoFree(mp->Pages);

#ifndef ADRENOMP_USE_LINKED_LIST
	AdrenoBitArray_Free(&mp->FreeList);
#endif

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
