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

#ifndef ADRENOGC_H
#define ADRENOGC_H

#include <queue>
#include <adreno/helpers.h>
#include <adreno/utils/memorypool.h>

namespace Adreno
{
	enum class GCObjectState
	{
		Alive,
		Dead,
		Collected,
	};

	enum class ReferenceType
	{
		Weak,
		Strong
	};
	
	class GCObject;
	class GC
	{
	public:
		static void AddToFinalizerQueue(GCObject *object);
		static void SurpressFinalize(GCObject *object);
		static void Collect();

	private:
		struct FinalizerQueueEntry
		{
			GCObject *Object;
			bool Valid;
		};

		static std::queue<FinalizerQueueEntry *> _FinalizerQueue;
		static MemoryPool<FinalizerQueueEntry> _FinalizerPool;

		friend class GCObject;
	};

	class Object;
	class GCObject
	{
	public:
		GCObject(Object *owner)
		{
			Value(owner);
			State(GCObjectState::Alive);
			FinalizerEntry(nullptr);

			_Strong = 0;
			_Weak = 0;
		}

		void Reference(ReferenceType type = ReferenceType::Strong)
		{
			if (type == ReferenceType::Strong)
			{
				if (_Strong == 0 && State() == GCObjectState::Dead)
				{
					GC::SurpressFinalize(this);
					State(GCObjectState::Alive);
					FinalizerEntry(nullptr);
				}

				_Strong++;
			}
			else if (type == ReferenceType::Weak)
			{
				_Weak++;
			}
		}

		void Dereference(ReferenceType type = ReferenceType::Strong)
		{
			if (type == ReferenceType::Strong)
			{
				_Strong--;
			}
			else if (type == ReferenceType::Weak)
			{
				_Weak--;
			}

			TryFinalize();
		}

		size_t Strong() const
		{
			return _Strong;
		}

		size_t Weak() const
		{
			return _Weak;
		}

		DEFPROP_RO_C(public, GCObjectState, State);
		DEFPROP_RO_P(public, Object, Value);

	private:
		DEFPROP_RO_P(private, GC::FinalizerQueueEntry, FinalizerEntry);

		void TryFinalize()
		{
			if (_Strong == 0 && FinalizerEntry() == nullptr)
			{
				State(GCObjectState::Dead);
				GC::AddToFinalizerQueue(this);
			}

			if (_Strong == 0 && _Weak == 0)
				delete this;
		}

		size_t _Weak, _Strong;
		
		friend class GC;
	};

	template<typename _Ty = Object>
	class Reference
	{
	public:
		Reference()
		{
			Value(nullptr);
		}
		
		Reference(const Reference<_Ty> &other)
		{
			Value(nullptr);
			Reset(other.Value());
		}

		Reference(_Ty *pointer)
		{
			Value(nullptr);
			Reset(pointer);
		}

		~Reference()
		{
			if (Value() != nullptr)
				Value()->GCState()->Dereference(ReferenceType::Strong);
		}

		void Reset(_Ty *pointer)
		{
			if (Value() != nullptr)
				Value()->GCState()->Dereference(ReferenceType::Strong);

			Value(pointer);

			if (Value())
				Value()->GCState()->Reference(ReferenceType::Strong);
		}

		_Ty &operator *() const
		{
			return *Value();
		}

		_Ty *operator ->() const
		{
			return Value();
		}

		bool operator ==(_Ty *pointer) const
		{
			return Value() == pointer;
		}

		bool operator !=(_Ty *pointer) const
		{
			return Value() != pointer;
		}

		bool operator ==(const Reference<_Ty> &other) const
		{
			return Value() == other.Value();
		}

		DEFPROP_RO_P(public, _Ty, Value);
	};

	template<typename _Ty = Object>
	class WeakReference
	{
	public:
		WeakReference()
		{
			_RefCounter = nullptr;
		}
		
		WeakReference(const Reference<_Ty> &other)
		{
			_RefCounter = nullptr;
			Reset(other.Value());
		}
		
		WeakReference(const WeakReference<_Ty> &other)
		{
			_RefCounter = nullptr;
			Reset(other.Value());
		}
		
		WeakReference(_Ty *other)
		{
			_RefCounter = nullptr;
			Reset(other);
		}

		~WeakReference()
		{
			if (_RefCounter != nullptr)
				_RefCounter->Dereference(ReferenceType::Weak);
		}

		void Reset(_Ty *pointer)
		{
			if (_RefCounter != nullptr)
				_RefCounter->Dereference(ReferenceType::Weak);

			if (pointer)
				_RefCounter = pointer->GCState();
			else
				_RefCounter = nullptr;

			if (_RefCounter)
				_RefCounter->Reference(ReferenceType::Weak);
		}

		bool IsGone() const
		{
			return _RefCounter->State() == GCObjectState::Collected;
		}

		Reference<_Ty> Get() const
		{
			if (IsGone())
			{
				return nullptr;
			}
			else
			{
				return Reference<_Ty>(_RefCounter->Value());
			}
		}

	private:
		GCObject *_RefCounter;
	};
};

#endif