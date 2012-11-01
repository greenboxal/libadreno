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
#include <adreno/config.h>
#include <adreno/vm/vm.h>
#include <adreno/vm/stack.h>

using namespace Adreno;

TEST(Stack)
{
	VMContext context;
	context.MakeCurrent();

	Stack stack;
	Value *test = nullptr;

	CHECK(stack.Take(&test, ADRENOSTACK_DEFAULT_STACK));
	CHECK(stack.Pop(ADRENOSTACK_DEFAULT_STACK));
}
