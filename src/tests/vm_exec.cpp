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
		Context ctx;
		ctx.MakeCurrent();

		FunctionEmitter *fe = new FunctionEmitter("main");
		fe->LocalCount(0);
		fe->StackSize(32);
		fe->EmitOp2(Opcode::Ldnum, 1337);
		fe->EmitOp(Opcode::Return);
		
		AssemblyBuilder ab;
		ab.AddFunction(fe);
		Reference<Assembly> assembly = ab.ToAssembly();
	
		Value ret;
		ctx.Run(assembly->GetFunction("main"), Arguments(), ret);

		CHECK_EQUAL(1337, ret.AsNumber());
	}

	TEST(ExternFuncCall)
	{
		Context ctx;
		ctx.MakeCurrent();

		String check = "check";
		String check_equal = "check_equal";
		String add_five = "add_five";

		ctx.SetGlobal(check, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(1, args.Count());
			CHECK(args[0].AsBoolean());
			return Value();
		}).Value());

		ctx.SetGlobal(check_equal, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(2, args.Count());
			CHECK(args[0].AsObject()->EqualOp(args[1]).AsBoolean());
			return Value();
		}).Value());
		
		ctx.SetGlobal(add_five, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(1, args.Count());
			return Value(args[0].AsNumber() + 5);
		}).Value());
	
		// def main(arg1)
		FunctionEmitter *fe = new FunctionEmitter("main");
		fe->LocalCount(0);
		fe->StackSize(32);
		
		// check true
		fe->EmitOp(Opcode::Ldtrue);
		fe->EmitOp2(Opcode::Ldhash, check.Hash());
		fe->EmitOp(Opcode::Ldglob);
		fe->EmitOp2(Opcode::Call, 1);
		fe->EmitOp(Opcode::Pop);
		
		// check_equal arg1, 5 + 5
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp(Opcode::Add);
		
		fe->EmitOp(Opcode::Ldarg_0);
		fe->EmitOp2(Opcode::Ldhash, check_equal.Hash());
		fe->EmitOp(Opcode::Ldglob);
		fe->EmitOp2(Opcode::Call, 2);
		fe->EmitOp(Opcode::Pop);

		// check_equal arg1, add_five(5)
		fe->EmitOp2(Opcode::Ldnum, 5);
		fe->EmitOp2(Opcode::Ldhash, add_five.Hash());
		fe->EmitOp(Opcode::Ldglob);
		fe->EmitOp2(Opcode::Call, 1);

		fe->EmitOp(Opcode::Ldarg_0);
		fe->EmitOp2(Opcode::Ldhash, check_equal.Hash());
		fe->EmitOp(Opcode::Ldglob);
		fe->EmitOp2(Opcode::Call, 2);
		fe->EmitOp(Opcode::Pop);

		// return
		fe->EmitOp(Opcode::Ldnull);
		fe->EmitOp(Opcode::Return);
		
		AssemblyBuilder ab;
		ab.AddFunction(fe);
		Reference<Assembly> assembly = ab.ToAssembly();
	
		Value ret;
		ctx.Run(assembly->GetFunction("main"), Arguments(Value((intptr_t)10)), ret);
	}

	TEST(InternalFunctionCall)
	{
		Context ctx;
		ctx.MakeCurrent();

		String check_equal = "check_equal";
		String test = "test";

		ctx.SetGlobal(check_equal, FunctionObject::New([&](const Arguments &args) -> Value
		{
			CHECK_EQUAL(2, args.Count());
			CHECK(args[0].AsObject()->EqualOp(args[1]).AsBoolean());
			return Value();
		}).Value());

		FunctionEmitter *feTest = new FunctionEmitter(test);
		feTest->LocalCount(0);
		feTest->StackSize(32);
		feTest->EmitOp2(Opcode::Ldnum, 100);
		feTest->EmitOp(Opcode::Ldarg_0);
		feTest->EmitOp(Opcode::Mul);
		feTest->EmitOp(Opcode::Return);

		FunctionEmitter *feMain = new FunctionEmitter("main");
		feMain->LocalCount(0);
		feMain->StackSize(32);
		feMain->EmitOp2(Opcode::Ldnum, 5);
		feMain->EmitOp2(Opcode::Ldhash, test.Hash());
		feMain->EmitOp(Opcode::Ldglob);
		feMain->EmitOp2(Opcode::Call, 1);
		feMain->EmitOp2(Opcode::Ldnum, 500);
		feMain->EmitOp2(Opcode::Ldhash, check_equal.Hash());
		feMain->EmitOp(Opcode::Ldglob);
		feMain->EmitOp2(Opcode::Call, 2);
		feMain->EmitOp(Opcode::Pop);
		feMain->EmitOp(Opcode::Return);

		AssemblyBuilder ab;
		ab.AddFunction(feTest);
		ab.AddFunction(feMain);
		Reference<Assembly> assembly = ab.ToAssembly();
	
		Value ret;
		ctx.Run(assembly->GetFunction("main"), Arguments(), ret);
	}
}
