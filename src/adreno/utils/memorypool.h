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

#include <exception>
#include <stddef.h> // size_t
#include <limits.h>

namespace Adreno
{
	namespace Detail
	{
		typedef struct
		{
			char *Address;
#ifdef ADRENOMP_USE_LINKED_LIST
			size_t UseCount;
#endif
		} AdrenoMemoryPoolPage;

		class MemoryPoolImpl
		{
		public:
			~MemoryPoolImpl();

			static MemoryPoolImpl *Find(size_t objectSize, int expansionFactor);

			void Reference();
			void Dereference();

			void *Alloc();
			void Free(void *obj);

		private:
			MemoryPoolImpl(size_t objectSize, int expansionFactor);
			void Expand();

			size_t _PageSize;
			size_t _ObjectSize;
			size_t _ExpansionFactor;

			size_t _TotalCount;
			size_t _TotalMaxCount;
	
			AdrenoMemoryPoolPage *_Pages;
			size_t _PageCount;

#ifdef ADRENOMP_USE_LINKED_LIST
			size_t *_Reuse;
#else
			AdrenoBitArray _FreeList;
#endif

			size_t _DestroyLock;
		};
	};

	template<typename _Ty>
	class MemoryPool
	{
	public:
		MemoryPool(int expansionFactor = 1)
		{
			_Impl = Detail::MemoryPoolImpl::Find(sizeof(_Ty), expansionFactor);
			_Impl->Reference();
		}

		~MemoryPool()
		{
			_Impl->Dereference();
		}

		_Ty *Alloc()
		{
			_Ty *object = (_Ty *)_Impl->Alloc();

			new (object) _Ty();

			return object;
		}

		void Free(_Ty *obj)
		{
			obj->~_Ty();
			_Impl->Free(obj);
		}

	private:
		Detail::MemoryPoolImpl *_Impl;
	};
	
	template<typename _Ty>
	class MemoryPoolAllocator
	{
	public:
		typedef _Ty        value_type;
		typedef size_t     size_type;
		typedef ptrdiff_t  difference_type;
 
		typedef _Ty*       pointer;
		typedef const _Ty* const_pointer;
 
		typedef _Ty&       reference;
		typedef const _Ty& const_reference;

		template <class U>
		struct rebind
		{
			typedef MemoryPoolAllocator<U> other;
		};

		MemoryPoolAllocator()
		{

		}

		MemoryPoolAllocator(const MemoryPoolAllocator<_Ty> &)
		{

		}
		
		template <class _Other>
		MemoryPoolAllocator(const MemoryPoolAllocator<_Other> &)
		{

		}

		~MemoryPoolAllocator()
		{

		}
 
		pointer address(reference r) const
		{
			return &r;
		}
 
		const_pointer address(const_reference r) const
		{
			return &r;
		}
 
		pointer allocate(size_type n, const void * = 0)
		{
			if (n != 1)
				throw std::exception("This allocator can only alloc one object per allocate call.");

			return _Pool.Alloc();
		}
 
		void deallocate(pointer p, size_type)
		{
			_Pool.Free(p);
		}
 
		void construct(pointer p, const _Ty &val)
		{
			new (p) _Ty(val);
		}
 
		void destroy(pointer p)
		{
			p->~_Ty();
		}
 
		size_type max_size() const
		{
			return ULONG_MAX / sizeof(_Ty);
		}

	private:
		void operator =(const MemoryPoolAllocator<_Ty> &);

		MemoryPool<_Ty> _Pool;
	};
}

#endif
