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

#include <UnitTest++.h>
#include <adreno/vm/vm.h>
#include <adreno/vm/object.h>
#include <string>

using namespace Adreno;

SUITE(GC)
{
	TEST(Reference)
	{
		VMContext Context;
		Context.MakeCurrent();

		Object *obj = new Object();
		GCObject *gcobj = obj->GCState();
		CHECK_EQUAL(1, gcobj->Weak());

		{
			WeakReference<Object> weak1(obj);
			CHECK_EQUAL(2, gcobj->Weak());

			{
				Reference<Object> ref1(obj);
				CHECK_EQUAL(1, gcobj->Strong());

				{
					Reference<Object> ref2(obj);
					CHECK_EQUAL(2, gcobj->Strong());
				}

				CHECK_EQUAL(1, gcobj->Strong());
			}

			{
				WeakReference<Object> weak2(obj);
				CHECK_EQUAL(3, gcobj->Weak());
			}
			CHECK_EQUAL(2, gcobj->Weak());
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			{
				Reference<Object> ref = weak1.Get();
				CHECK_EQUAL(1, gcobj->Strong());
				CHECK(gcobj->State() == GCObjectState::Alive);
			}
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			Context.GC()->Collect();
	
			CHECK_EQUAL(true, weak1.IsGone());
			CHECK(gcobj->State() == GCObjectState::Collected);
		}
	}


	class PersistentObject : public Object
	{
	public:
		PersistentObject()
		{
			_PersistenceCounter = 0;
		}

		~PersistentObject()
		{

		}

		virtual bool Destruct()
		{
			_PersistenceCounter++;

			if (_PersistenceCounter > 4)
				return Object::Destruct();

			return false;
		}
	
	private:
		int _PersistenceCounter;
	};

	TEST(Persistency)
	{
		VMContext Context;
		Context.MakeCurrent();

		Object *obj = new PersistentObject();
		GCObject *gcobj = obj->GCState();
		CHECK_EQUAL(1, gcobj->Weak());

		{
			WeakReference<Object> weak1(obj);
			CHECK_EQUAL(2, gcobj->Weak());

			{
				Reference<Object> ref1(obj);
				CHECK_EQUAL(1, gcobj->Strong());

				{
					Reference<Object> ref2(obj);
					CHECK_EQUAL(2, gcobj->Strong());
				}

				CHECK_EQUAL(1, gcobj->Strong());
			}

			{
				WeakReference<Object> weak2(obj);
				CHECK_EQUAL(3, gcobj->Weak());
			}
			CHECK_EQUAL(2, gcobj->Weak());
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			{
				Reference<Object> ref = weak1.Get();
				CHECK_EQUAL(1, gcobj->Strong());
				CHECK(gcobj->State() == GCObjectState::Alive);
			}
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			for (int i = 0; i < 4; i++)
			{
				Context.GC()->Collect();

				CHECK_EQUAL(0, gcobj->Strong());
				CHECK(gcobj->State() == GCObjectState::Dead);
				CHECK_EQUAL(false, weak1.IsGone());

				{
					Reference<Object> ref = weak1.Get();
					CHECK_EQUAL(1, gcobj->Strong());
					CHECK(gcobj->State() == GCObjectState::Alive);
				}
	
				CHECK_EQUAL(0, gcobj->Strong());
				CHECK(gcobj->State() == GCObjectState::Dead);
				CHECK_EQUAL(false, weak1.IsGone());
			}
			
			Context.GC()->Collect();
	
			CHECK_EQUAL(true, weak1.IsGone());
			CHECK(gcobj->State() == GCObjectState::Collected);
		}
	}
}
