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

using namespace Adreno;

TEST(Objects)
{
	Reference<Object> obj1 = Object::New();

	obj1->SetField("op_Call", FunctionObject::New([&](const Arguments &args)
	{
		return args[0];
	}).Value());

	CHECK_EQUAL(1337, obj1->Call(Arguments(1337)).AsNumber());
}
