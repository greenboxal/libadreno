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
#include <adreno/vm/object.h>
#include <string>

SUITE(GC)
{
	TEST(Reference)
	{
		Adreno::Object *obj = new Adreno::Object();
		Adreno::GCObject *gcobj = obj->GCState();
		CHECK_EQUAL(1, gcobj->Weak());

		{
			Adreno::WeakReference<Adreno::Object> weak1(obj);
			CHECK_EQUAL(2, gcobj->Weak());

			{
				Adreno::Reference<Adreno::Object> ref1(obj);
				CHECK_EQUAL(1, gcobj->Strong());

				{
					Adreno::Reference<Adreno::Object> ref2(obj);
					CHECK_EQUAL(2, gcobj->Strong());
				}

				CHECK_EQUAL(1, gcobj->Strong());
			}

			{
				Adreno::WeakReference<Adreno::Object> weak2(obj);
				CHECK_EQUAL(3, gcobj->Weak());
			}
			CHECK_EQUAL(2, gcobj->Weak());
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			{
				Adreno::Reference<Adreno::Object> ref = weak1.Get();
				CHECK_EQUAL(1, gcobj->Strong());
				CHECK(gcobj->State() == Adreno::GCObjectState::Alive);
			}
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			Adreno::GC::Collect();
	
			CHECK_EQUAL(true, weak1.IsGone());
			CHECK(gcobj->State() == Adreno::GCObjectState::Collected);
		}
	}


	class PersistentObject : public Adreno::Object
	{
	public:
		PersistentObject()
		{
			_PersistenceCounter = 0;
		}

		~PersistentObject()
		{

		}

		virtual bool Finalize()
		{
			_PersistenceCounter++;

			if (_PersistenceCounter > 4)
				return Adreno::Object::Finalize();

			return false;
		}
	
	private:
		int _PersistenceCounter;
	};

	TEST(Persistency)
	{
		Adreno::Object *obj = new PersistentObject();
		Adreno::GCObject *gcobj = obj->GCState();
		CHECK_EQUAL(1, gcobj->Weak());

		{
			Adreno::WeakReference<Adreno::Object> weak1(obj);
			CHECK_EQUAL(2, gcobj->Weak());

			{
				Adreno::Reference<Adreno::Object> ref1(obj);
				CHECK_EQUAL(1, gcobj->Strong());

				{
					Adreno::Reference<Adreno::Object> ref2(obj);
					CHECK_EQUAL(2, gcobj->Strong());
				}

				CHECK_EQUAL(1, gcobj->Strong());
			}

			{
				Adreno::WeakReference<Adreno::Object> weak2(obj);
				CHECK_EQUAL(3, gcobj->Weak());
			}
			CHECK_EQUAL(2, gcobj->Weak());
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			{
				Adreno::Reference<Adreno::Object> ref = weak1.Get();
				CHECK_EQUAL(1, gcobj->Strong());
				CHECK(gcobj->State() == Adreno::GCObjectState::Alive);
			}
	
			CHECK_EQUAL(0, gcobj->Strong());
			CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
			CHECK_EQUAL(false, weak1.IsGone());

			for (int i = 0; i < 4; i++)
			{
				Adreno::GC::Collect();

				CHECK_EQUAL(0, gcobj->Strong());
				CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
				CHECK_EQUAL(false, weak1.IsGone());

				{
					Adreno::Reference<Adreno::Object> ref = weak1.Get();
					CHECK_EQUAL(1, gcobj->Strong());
					CHECK(gcobj->State() == Adreno::GCObjectState::Alive);
				}
	
				CHECK_EQUAL(0, gcobj->Strong());
				CHECK(gcobj->State() == Adreno::GCObjectState::Dead);
				CHECK_EQUAL(false, weak1.IsGone());
			}
			
			Adreno::GC::Collect();
	
			CHECK_EQUAL(true, weak1.IsGone());
			CHECK(gcobj->State() == Adreno::GCObjectState::Collected);
		}
	}
}
