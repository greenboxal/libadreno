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

			size_t _Index;
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
			return (_Ty *)_Impl->Alloc();
		}

		void Free(_Ty *obj)
		{
			_Impl->Free(obj);
		}

	private:
		Detail::MemoryPoolImpl *_Impl;
	};
}

#endif
