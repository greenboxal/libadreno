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

#include <adreno/vm/vm.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/object.h>

using namespace Adreno;

void GarbageCollector::AddToFinalizerQueue(GCObject *object)
{
	FinalizerQueueEntry *entry = _FinalizerPool.Alloc();
	entry->Object = object;
	entry->Valid = true;
	object->FinalizerEntry(entry);
	_FinalizerQueue.push(entry);
}

void GarbageCollector::SurpressFinalize(GCObject *object)
{
	if (object->FinalizerEntry() != nullptr)
		object->FinalizerEntry()->Valid = false;
}

void GarbageCollector::Collect()
{
	while (_FinalizerQueue.size() > 0)
	{
		FinalizerQueueEntry *entry = _FinalizerQueue.front();
		_FinalizerQueue.pop();

		entry->Object->Reference(ReferenceType::Weak);

		if (entry->Valid)
		{
			int prev = entry->Object->State();
			entry->Object->State(GCObjectState::Destructing);

			if (entry->Object->Value()->Destruct())
			{
				delete entry->Object->Value();
				entry->Object->State(GCObjectState::Collected);
			}
			else
			{
				entry->Object->State(prev);
			}
		}

		entry->Object->Dereference(ReferenceType::Weak);

		_FinalizerPool.Free(entry);
	}
}

GCObject::GCObject(Object *owner)
{
	Value(owner);
	State(GCObjectState::Alive);
	FinalizerEntry(nullptr);

	_Strong = 0;
	_Weak = 0;
}

void GCObject::Reference(int type)
{
	if (type == ReferenceType::Strong)
	{
		if (_Strong == 0 && State() == GCObjectState::Dead)
		{
			Context::Current()->GC()->SurpressFinalize(this);
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

void GCObject::Dereference(int type)
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

void GCObject::TryFinalize()
{
	if (_Strong == 0 && FinalizerEntry() == nullptr && State() == GCObjectState::Alive)
	{
		State(GCObjectState::Dead);
		Context::Current()->GC()->AddToFinalizerQueue(this);
	}

	if (_Strong == 0 && _Weak == 0)
		delete this;
}
