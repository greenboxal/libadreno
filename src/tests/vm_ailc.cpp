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
/*#include <adreno/ail/ail.h>

using namespace Adreno;

extern int AILC_debug;

TEST(AIL)
{
	VMContext context;
	context.MakeCurrent();

	std::stringstream ss;
#define x(s) ss.write(s"\n", strlen(s"\n"))
	x("// Test program 1");
	x("def main");
	x("ldarg0");
	x("ldnum 2");
	x("mul");
	x("ret");
	x("end");
#undef x

	AilParserContext apc(ss);

	//AILC_debug = 1;
	CHECK(AILC_parse(&apc) == 0);

	AssemblyBuilder *ab = apc.GetAssembly();
	ExecutionContext *ec = context.CreateExecutionContext(ab);
	
	Value ret;
	CHECK(ec->Run("main", Arguments(Value((intptr_t)1337)), ret));
	CHECK_EQUAL(1337 * 2, ret.AsNumber());

	delete ec;
	delete ab;
}*/
