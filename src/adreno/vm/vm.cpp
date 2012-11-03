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

#include <adreno/vm/vm.h>

using namespace Adreno;

THREAD_LOCAL VMContext *VMContext::_CurrentVM;

VMContext::VMContext()
{
	GC(new GarbageCollector());
}

VMContext::~VMContext()
{
	if (_CurrentVM == this)
		_CurrentVM = nullptr;

	delete GC();
}

void VMContext::MakeCurrent()
{
	_CurrentVM = this;
}

Class *VMContext::GetClass(const String &name) const
{
	Assembly::ClassMap::const_iterator it = _Classes.find(name);

	if (it == _Classes.end())
		return nullptr;

	return it->second;
}

void VMContext::SetClass(const String &name, Class *function)
{
	_Classes[name] = function;
}

Value VMContext::GetGlobal(const String &name)
{
	Object::FieldMap::iterator it = _Globals.find(name);

	if (it == _Globals.end())
		return Value();

	return it->second;
}

void VMContext::SetGlobal(const String &name, const Value &value)
{
	_Globals[name] = value;
}

ExecutionContext *VMContext::CreateExecutionContext(Assembly *assembly)
{
	return new ExecutionContext(this, assembly);
}

ExecutionContext::ExecutionContext(VMContext *owner, Assembly *unit)
{
	Owner(owner);
	Unit(unit);
	State(ExecutionState::Stopped);
}

ExecutionContext::~ExecutionContext()
{

}

bool ExecutionContext::Run(const String &name, const Arguments &args, Value &retValue)
{
	BytecodeFunction *fn = Unit()->GetFunction(name);

	if (fn == nullptr)
		return false;
	
	Error(0);

	if (State() == ExecutionState::Stopped)
	{
		_IP = 0;
		_Stack = new Stack(fn->StackSize());
		_Locals.resize(fn->LocalCount());
	}
	else if (State() == ExecutionState::Running)
	{
		Error(1);
		return false;
	}

	Opcode::Opcodes op;
	Prefix::Prefixes prefix;
	std::uint32_t intVal = 0;
	double floatVal = 0;
	Value value, value2;
		
	State(ExecutionState::Running);
	while (State() == ExecutionState::Running)
	{
#define HasBytes(count) { if (_IP + (count) > fn->BytecodeSize()) { Error(1); State(ExecutionState::Stopped); break; } }
#define ReadChar(var) { HasBytes(sizeof(unsigned char)); var = fn->Bytecode()[_IP]; _IP += sizeof(unsigned char); }
#define ReadOP(var) { HasBytes(sizeof(unsigned char)); var = (Opcode::Opcodes)fn->Bytecode()[_IP]; _IP += sizeof(unsigned char); }
#define ReadPrefix(var) { HasBytes(sizeof(unsigned char)); var = (Prefix::Prefixes)fn->Bytecode()[_IP]; _IP += sizeof(unsigned char); }
#define ReadInt(var) { HasBytes(sizeof(std::uint32_t)); var = *((std::uint32_t *)&fn->Bytecode()[_IP]); _IP += sizeof(std::uint32_t); }
#define ReadDouble(var) { HasBytes(sizeof(double)); var = *((double *)&fn->Bytecode()[_IP]); _IP += sizeof(double); }
#define Check(err, inst) { if (!(inst)) { Error(err); State(ExecutionState::Stopped); break; } }

		ReadOP(op);
		prefix = (Prefix::Prefixes)(op & Prefix::Mask);

		if (prefix & Prefix::Extra)
			ReadPrefix(prefix);

		op = (Opcode::Opcodes)(op & Opcode::Mask);

		switch (op)
		{
		case Opcode::Nop:
			break;

			// Stack
		case Opcode::Pop:
			Check(1, _Stack->Pop(1))
			break;
		case Opcode::Pop_S:
			ReadInt(intVal);
			Check(1, _Stack->Pop(intVal))
			break;

			// Value Loading
		case Opcode::Ldnull:
			Check(1, _Stack->Push(Value()));
			break;
		case Opcode::Ldnum:
			ReadInt(intVal);
			Check(1, _Stack->Push(Value((intptr_t)intVal)));
			break;
		case Opcode::Ldnum_M1:
			Check(1, _Stack->Push(Value(-1)));
			break;
		case Opcode::Ldnum_0:
			Check(1, _Stack->Push(Value(0)));
			break;
		case Opcode::Ldnum_1:
			Check(1, _Stack->Push(Value(1)));
			break;
		case Opcode::Ldfloat:
			ReadDouble(floatVal);
			Check(1, _Stack->Push(floatVal));
			break;
		case Opcode::Ldstr:
			{
				ReadInt(intVal);

				String str = Unit()->GetString(intVal);
				Check(1, str.Data() != nullptr);

				Check(1, _Stack->Push(str));
			}
			break;
		case Opcode::Ldhash:
			ReadInt(intVal);
			Check(1, _Stack->Push(String::Sealed(intVal, intVal)));
			break;
		case Opcode::Ldglob:
			ReadInt(intVal);
			Check(1, _Stack->Push(_Owner->GetGlobal(String::Sealed(intVal, intVal))));
			break;
		case Opcode::Ldcls:
			ReadInt(intVal);
			Check(1, false);
			break;
		case Opcode::Ldtrue:
			Check(1, _Stack->Push(true));
			break;
		case Opcode::Ldfalse:
			Check(1, _Stack->Push(true));
			break;

			// Arguments
		case Opcode::Ldarg_0:
		case Opcode::Ldarg_1:
		case Opcode::Ldarg_2:
		case Opcode::Ldarg_3:
			intVal = op - Opcode::Ldarg_0;
			Check(1, intVal < args.Count());
			Check(1, _Stack->Push(args[intVal]));
			break;
		case Opcode::Ldarg_S:
			ReadInt(intVal);
			Check(1, intVal < args.Count());
			Check(1, _Stack->Push(args[intVal]));
			break;

			// Locals
		case Opcode::Ldloc_0:
		case Opcode::Ldloc_1:
		case Opcode::Ldloc_2:
		case Opcode::Ldloc_3:
			intVal = op - Opcode::Ldloc_0;
			Check(1, intVal < _Locals.size());
			_Stack->Push(_Locals[intVal]);
			break;
		case Opcode::Ldloc_S:
			ReadInt(intVal);
			Check(1, intVal < _Locals.size());
			Check(1, _Stack->Push(_Locals[intVal]));
			break;

		case Opcode::Stloc_0:
		case Opcode::Stloc_1:
		case Opcode::Stloc_2:
		case Opcode::Stloc_3:
			intVal = op - Opcode::Stloc_0;
			Check(1, intVal < _Locals.size());
			Check(1, _Stack->Pop(value));
			_Locals[intVal] = value;
			break;
		case Opcode::Stloc_S:
			ReadInt(intVal);
			Check(1, intVal < _Locals.size());
			Check(1, _Stack->Pop(value));
			_Locals[intVal] = value;
			break;

			// New
		case Opcode::New:
			{
				ReadInt(intVal);
				std::vector<Value> callargs(intVal);
				
				for (size_t i = 0; i < intVal; i++)
					Check(1, _Stack->Pop(callargs[i]));

				// For now just ignore the object type
				Check(1, _Stack->Pop(1));

				Reference<Object> obj = Object::New();
				obj->Construct(callargs);

				Check(1, _Stack->Push(obj.Value()));
			}
			break;

			// Math
#define MathOp(name) \
		case Opcode::name: \
			Check(1, _Stack->Pop(value)); \
			Check(1, _Stack->Push(value.AsObject()->name##Op())); \
			break;
#define MathOp2(name) \
		case Opcode::name: \
			Check(1, _Stack->Pop(value2)); \
			Check(1, _Stack->Pop(value)); \
			Check(1, _Stack->Push(value.AsObject()->name##Op(value2))); \
			break;

			MathOp2(Add)
			MathOp2(Sub)
			MathOp2(Mul)
			MathOp2(Div)
			MathOp2(Rem)
			MathOp(Neg)

			MathOp2(And)
			MathOp2(Or)
			MathOp2(Xor)
			MathOp(Not)
			MathOp2(LeftShift)
			MathOp2(RightShift)

			MathOp2(LogAnd)
			MathOp2(LogOr)
			MathOp(LogNot)
		
			MathOp2(Equal)
			MathOp2(NotEqual)
			MathOp2(Greater)
			MathOp2(GreaterEq)
			MathOp2(Lesser)
			MathOp2(LesserEq)
#undef MathOp2
#undef MathOp

			// Function Calling
		case Opcode::Call:
			{
				ReadInt(intVal);
				std::vector<Value> callargs(intVal);
				
				for (size_t i = 0; i < intVal; i++)
					Check(1, _Stack->Pop(callargs[i]));

				Check(1, _Stack->Pop(value));
				Check(1, _Stack->Push(value.AsObject()->Call(callargs)));
			}
			break;
		case Opcode::Return:
			Check(1, _Stack->Pop(retValue));
			State(ExecutionState::Stopped);
			break;

			// Jumps
		case Opcode::Jump:
			ReadInt(intVal);
			Check(1, intVal < fn->BytecodeSize());
			_IP = intVal;
			break;
		case Opcode::Brtrue:
			ReadInt(intVal);
			Check(1, intVal < fn->BytecodeSize());
			Check(1, _Stack->Pop(value));
			if (value.AsBoolean() == true)
				_IP = intVal;
			break;
		case Opcode::Brfalse:
			ReadInt(intVal);
			Check(1, intVal < fn->BytecodeSize());
			Check(1, _Stack->Pop(value));
			if (value.AsBoolean() == false)
				_IP = intVal;
			break;
		}

#undef HasBytes
#undef Check
#undef Read4Byte
#undef Read1Byte
	}

	if (State() == ExecutionState::Stopped)
	{
		_Locals.clear();
		delete _Stack;
	}

	return true;
}
