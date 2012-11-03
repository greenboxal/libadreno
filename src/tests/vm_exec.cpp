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
#include <adreno/vm/emit.h>

using namespace Adreno;

SUITE(VMExecution)
{
	TEST(SimpleReturn)
	{
		VMContext context;
		context.MakeCurrent();

		AssemblyBuilder *ab = new AssemblyBuilder();
	
		FunctionEmitter *fe = new FunctionEmitter("main");
		fe->SetLocalCount(0);
		fe->SetStackSize(32);
		fe->EmitOp2(Opcode::Ldnum, 1337);
		fe->EmitOp(Opcode::Return);
		fe->Finish();

		ab->SetFunction(fe);

		ExecutionContext *ec = context.CreateExecutionContext(ab);
	
		Value ret;
		ec->Run("main", Arguments(), ret);

		CHECK_EQUAL(1337, ret.AsNumber());
	
		delete ec;
		delete ab;
	}
}
