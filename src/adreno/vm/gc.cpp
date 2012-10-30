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

#include <adreno/vm/gc.h>
#include <adreno/vm/object.h>

using namespace Adreno;

std::queue<GC::FinalizerQueueEntry *> GC::_FinalizerQueue;
MemoryPool<GC::FinalizerQueueEntry> GC::_FinalizerPool;

void GC::AddToFinalizerQueue(GCObject *object)
{
	FinalizerQueueEntry *entry = _FinalizerPool.Alloc();
	entry->Object = object;
	entry->Valid = true;
	object->FinalizerEntry(entry);
	_FinalizerQueue.push(entry);
}

void GC::SurpressFinalize(GCObject *object)
{
	if (object->FinalizerEntry() != nullptr)
		object->FinalizerEntry()->Valid = false;
}

void GC::Collect()
{
	while (_FinalizerQueue.size() > 0)
	{
		FinalizerQueueEntry *entry = _FinalizerQueue.front();
		_FinalizerQueue.pop();

		entry->Object->Reference(ReferenceType::Weak);

		if (entry->Valid)
		{
			if (entry->Object->Value()->Finalize())
			{
				delete entry->Object->Value();
				entry->Object->State(GCObjectState::Collected);
			}
		}

		entry->Object->Dereference(ReferenceType::Weak);

		_FinalizerPool.Free(entry);
	}
}
