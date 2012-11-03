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

	TEST(ExternFuncCall)
	{
		VMContext context;
		context.MakeCurrent();

		String check = "check";
		String check_equal = "check_equal";
		String add_five = "add_five";

		context.SetGlobal(check, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(1, args.Count());
			CHECK(args[0].AsBoolean());
			return Value();
		}).Value());

		context.SetGlobal(check_equal, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(2, args.Count());
			CHECK(args[0].AsObject()->EqualOp(args[1]).AsBoolean());
			return Value();
		}).Value());
		
		context.SetGlobal(add_five, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(1, args.Count());
			return Value(args[0].AsNumber() + 5);
		}).Value());

		AssemblyBuilder *ab = new AssemblyBuilder();
	
		// def main(arg1)
		FunctionEmitter *fe = new FunctionEmitter("main");
		fe->SetLocalCount(0);
		fe->SetStackSize(32);
		
		// check true
		fe->EmitOp(Opcode::Ldtrue);
		fe->EmitOp2(Opcode::Ldglob, check.Hash());
		fe->EmitOp2(Opcode::Call, 1);
		fe->EmitOp(Opcode::Pop);
		
		// check_equal arg1, 5 + 5
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp(Opcode::Add);
		
		fe->EmitOp(Opcode::Ldarg_0);
		fe->EmitOp2(Opcode::Ldglob, check_equal.Hash());
		fe->EmitOp2(Opcode::Call, 2);
		fe->EmitOp(Opcode::Pop);

		// check_equal arg1, add_five(5)
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp2(Opcode::Ldglob, add_five.Hash());
		fe->EmitOp2(Opcode::Call, 1);

		fe->EmitOp(Opcode::Ldarg_0);
		fe->EmitOp2(Opcode::Ldglob, check_equal.Hash());
		fe->EmitOp2(Opcode::Call, 2);
		fe->EmitOp(Opcode::Pop);

		// return
		fe->EmitOp(Opcode::Ldnull);
		fe->EmitOp(Opcode::Return);

		// end
		fe->Finish();

		ab->SetFunction(fe);

		ExecutionContext *ec = context.CreateExecutionContext(ab);
	
		Value ret;
		ec->Run("main", Arguments(Value((intptr_t)10)), ret);
	
		delete ec;
		delete ab;
	}
}
