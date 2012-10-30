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

#include <list>

using namespace Adreno::Detail;

static std::list<MemoryPoolImpl *> _MPools;


#if defined(_WIN32) && !defined(ADRENOMP_FORCE_MEMMGR_ALLOC)
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


MemoryPoolImpl *MemoryPoolImpl::Find(size_t objectSize, int expansionFactor)
{
	std::list<MemoryPoolImpl *>::iterator it;

	if (objectSize % 2)
		objectSize++;

#ifdef ADRENOMP_USE_LINKED_LIST
	objectSize += sizeof(size_t);
#endif

	for (it = _MPools.begin(); it != _MPools.end(); it++)
		if ((*it)->_ObjectSize == objectSize)
			return *it;

	MemoryPoolImpl *impl = new MemoryPoolImpl(objectSize - sizeof(size_t), expansionFactor);
	_MPools.push_back(impl);

	return impl;
}

MemoryPoolImpl::MemoryPoolImpl(size_t objectSize, int expansionFactor)
{
	if (objectSize % 2)
		objectSize++;
	
#ifdef ADRENOMP_USE_LINKED_LIST
	objectSize += sizeof( size_t );
#endif

	_PageSize = (ADRENOMP_PAGE_SIZE / objectSize) * objectSize;
	_ObjectSize = objectSize;
	_ExpansionFactor = 0;

	while (_ExpansionFactor * _PageSize < objectSize)
		_ExpansionFactor++;

	_ExpansionFactor *= expansionFactor;

	_Pages = NULL;
	_PageCount = 0;

	_TotalCount = 0;
	_TotalMaxCount = 0;

	_Index = -1;
	_DestroyLock = 1;

#ifdef ADRENOMP_USE_LINKED_LIST
	_Reuse = 0;
#else
	AdrenoBitArray_Initialize(&_FreeList, 0);
#endif

	Expand();
}

MemoryPoolImpl::~MemoryPoolImpl()
{
	for (int i = 0; i < _PageCount; i++)
		AP_FreePage((void *)_Pages[i].Address, _ExpansionFactor);
	
	if (_Pages)
		AdrenoFree(_Pages);

#ifndef ADRENOMP_USE_LINKED_LIST
	AdrenoBitArray_Free(&_FreeList);
#endif

}

void MemoryPoolImpl::Reference()
{
	_DestroyLock++;
}

void MemoryPoolImpl::Dereference()
{
	if (--_DestroyLock > 0)
		return;
	
	std::list<MemoryPoolImpl *>::iterator it;
	for (it = _MPools.begin(); it != _MPools.end(); it++)
	{
		if (*it == this)
		{
			_MPools.erase(it);
			break;
		}
	}

	delete this;
}

void MemoryPoolImpl::Expand()
{
	int nIdx = _PageCount;

	_PageCount++;
	_Pages = (AdrenoMemoryPoolPage *)AdrenoRealloc(_Pages, _PageCount * sizeof(AdrenoMemoryPoolPage));
		
	_Pages[nIdx].Address = (char *)AP_ReservePage(_ExpansionFactor);

	// For now commit the pages too
	AP_CommitPage(_Pages[nIdx].Address, _ExpansionFactor);

#ifdef ADRENOMP_USE_LINKED_LIST
	*((size_t *)_Pages[nIdx].Address) = 0;
	_Pages[nIdx].UseCount = 0;
#endif

	_TotalMaxCount += _ExpansionFactor * _PageSize / _ObjectSize;

#ifndef ADRENOMP_USE_LINKED_LIST
	AdrenoBitArray_Resize(&_FreeList, _TotalMaxCount);
#endif
}

void *MemoryPoolImpl::Alloc()
{
#ifdef ADRENOMP_USE_LINKED_LIST
	if (_Reuse)
	{
		void *ret = (void *)(_Reuse + 1);

		_Reuse = (size_t *) *_Reuse;

		return ret;
	}
	else if (_PageCount && _Pages[_PageCount - 1].UseCount < _ExpansionFactor * _PageSize / _ObjectSize)
	{
		return (void *)(&_Pages[_PageCount -
		1].Address[_Pages[_PageCount - 1].UseCount++ * _ObjectSize] + sizeof(size_t));
	}
	else
	{
		Expand();

		return Alloc();
	}
#else
	int freeIdx;
	int total, total2;
	int pageIndex, pageOffset;

	if (_TotalCount >= _TotalMaxCount)
	{
		AdrenoMemoryPool_Expand(mp);

		return AdrenoMemoryPool_Alloc(mp);
	}


	freeIdx = AdrenoBitArray_Search(&_FreeList);
	total = _ExpansionFactor * _PageSize;
	total2 = _ObjectSize * freeIdx;

	pageIndex = total2 / total;
	pageOffset = total2 % total;

	_TotalCount++;
	AdrenoBitArray_Set(&_FreeList, freeIdx);

	return (void *)&_Pages[pageIndex].Address[pageOffset];
#endif
}

void MemoryPoolImpl::Free(void *obj)
{
#ifdef ADRENOMP_USE_LINKED_LIST
	size_t *reuse = (size_t *)obj - 1;
	*reuse = (size_t)_Reuse;
	_Reuse = reuse;
#else
	size_t i = 0;

	for (i = 0; i < _PageCount; i++)
	{
		if ((size_t)ptr >= (size_t)_Pages[i].Address &&
			(size_t)ptr < (size_t)(_Pages[i].Address + _ExpansionFactor * _PageSize))
		{
			int pageOffset, freeIdx;

			pageOffset = ((size_t)ptr - (size_t)_Pages[i].Address);
			freeIdx = ((i * _ExpansionFactor * _PageSize) / _ObjectSize) + (pageOffset / _ObjectSize);

			_TotalCount--;
			AdrenoBitArray_Unset(&_FreeList, freeIdx);

			break;
		}
	}
#endif
}
