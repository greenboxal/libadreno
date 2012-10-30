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
#include <adreno/utils/memorypool.h>
#include <string>

struct TestStruct
{
	int Foo;
	int Bar;
	std::string String;
};

TEST(MemoryPoolTest)
{
	Adreno::MemoryPool<TestStruct> mp;

	TestStruct *t1 = mp.Alloc();

	t1->Bar = 1337;
	t1->Foo = 242424666;
	t1->String = "test tes testes";

	mp.Free(t1);

	// t2 should be the same as t1
	TestStruct *t2 = mp.Alloc();

	CHECK_EQUAL(1337, t2->Bar);
	CHECK_EQUAL(242424666, t2->Foo);
	CHECK_EQUAL("test tes testes", t2->String);
}
