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

THREAD_LOCAL Context *Context::_Current;

Context::Context()
{
	GC(new GarbageCollector());
	State(ExecutionState::Stopped);
	Error(0);
}

Context::~Context()
{
	if (_Current == this)
		_Current = nullptr;

	delete GC();
}

bool Context::Run(const Reference<Function> &function, const Arguments &args, Value &retValue)
{
	MakeCurrent();
	Error(0);

	if (State() == ExecutionState::Stopped)
	{
		_CurrentFrame = new CallFrame(function, args);
	}
	else if (State() == ExecutionState::Running)
	{
		_CallStack.push(_CurrentFrame);
		_CurrentFrame = new CallFrame(function, args);
	}

	Opcode::Opcodes op;
	Prefix::Prefixes prefix;
	std::uint32_t intVal = 0;
	double floatVal = 0;
	Value value, value2;
		
	State(ExecutionState::Running);
	while (State() == ExecutionState::Running)
	{
#define HasBytes(count) { if (_CurrentFrame->IP + (count) > _CurrentFrame->F->BytecodeSize()) { Error(1); State(ExecutionState::Stopped); break; } }
#define ReadChar(var) { HasBytes(sizeof(unsigned char)); var = _CurrentFrame->F->Bytecode()[_CurrentFrame->IP]; _CurrentFrame->IP += sizeof(unsigned char); }
#define ReadOP(var) { HasBytes(sizeof(unsigned char)); var = (Opcode::Opcodes)_CurrentFrame->F->Bytecode()[_CurrentFrame->IP]; _CurrentFrame->IP += sizeof(unsigned char); }
#define ReadPrefix(var) { HasBytes(sizeof(unsigned char)); var = (Prefix::Prefixes)_CurrentFrame->F->Bytecode()[_CurrentFrame->IP]; _CurrentFrame->IP += sizeof(unsigned char); }
#define ReadInt(var) { HasBytes(sizeof(std::uint32_t)); var = *((std::uint32_t *)&_CurrentFrame->F->Bytecode()[_CurrentFrame->IP]); _CurrentFrame->IP += sizeof(std::uint32_t); }
#define ReadDouble(var) { HasBytes(sizeof(double)); var = *((double *)&_CurrentFrame->F->Bytecode()[_CurrentFrame->IP]); _CurrentFrame->IP += sizeof(double); }
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
			Check(1, _CurrentFrame->S.Pop(1))
			break;
		case Opcode::Pop_S:
			ReadInt(intVal);
			Check(1, _CurrentFrame->S.Pop(intVal))
			break;

			// Value Loading
		case Opcode::Ldnull:
			Check(1, _CurrentFrame->S.Push(Value()));
			break;
		case Opcode::Ldnum:
			ReadInt(intVal);
			Check(1, _CurrentFrame->S.Push(Value((intptr_t)intVal)));
			break;
		case Opcode::Ldnum_M1:
			Check(1, _CurrentFrame->S.Push(Value((intptr_t)-1)));
			break;
		case Opcode::Ldnum_0:
			Check(1, _CurrentFrame->S.Push(Value((intptr_t)0)));
			break;
		case Opcode::Ldnum_1:
			Check(1, _CurrentFrame->S.Push(Value((intptr_t)1)));
			break;
		case Opcode::Ldfloat:
			ReadDouble(floatVal);
			Check(1, _CurrentFrame->S.Push(floatVal));
			break;
		case Opcode::Ldstr:
			{
				ReadInt(intVal);

				String str = _CurrentFrame->F->Owner()->GetString(intVal);
				Check(1, str.Data() != nullptr);

				Check(1, _CurrentFrame->S.Push(str));
			}
			break;
		case Opcode::Ldhash:
			ReadInt(intVal);
			Check(1, _CurrentFrame->S.Push(String::Sealed(intVal, intVal)));
			break;
		case Opcode::Ldglob:
			{
				Check(1, _CurrentFrame->S.Pop(value));
				String name = value.AsString();
				if (_CurrentFrame->F->Owner()->HasField(name))
					value = _CurrentFrame->F->Owner()->GetField(name);
				else
					value = GetGlobal(name);
				Check(1, _CurrentFrame->S.Push(value));
			}
			break;
		case Opcode::Ldtrue:
			Check(1, _CurrentFrame->S.Push(true));
			break;
		case Opcode::Ldfalse:
			Check(1, _CurrentFrame->S.Push(true));
			break;

			// Arguments
		case Opcode::Ldarg_0:
		case Opcode::Ldarg_1:
		case Opcode::Ldarg_2:
		case Opcode::Ldarg_3:
			intVal = op - Opcode::Ldarg_0;
			Check(1, intVal < args.Count());
			Check(1, _CurrentFrame->S.Push(args[intVal]));
			break;
		case Opcode::Ldarg_S:
			ReadInt(intVal);
			Check(1, intVal < args.Count());
			Check(1, _CurrentFrame->S.Push(args[intVal]));
			break;

			// Locals
		case Opcode::Ldloc_0:
		case Opcode::Ldloc_1:
		case Opcode::Ldloc_2:
		case Opcode::Ldloc_3:
			intVal = op - Opcode::Ldloc_0;
			Check(1, intVal < _CurrentFrame->L.size());
			_CurrentFrame->S.Push(_CurrentFrame->L[intVal]);
			break;
		case Opcode::Ldloc_S:
			ReadInt(intVal);
			Check(1, intVal < _CurrentFrame->L.size());
			Check(1, _CurrentFrame->S.Push(_CurrentFrame->L[intVal]));
			break;

		case Opcode::Stloc_0:
		case Opcode::Stloc_1:
		case Opcode::Stloc_2:
		case Opcode::Stloc_3:
			intVal = op - Opcode::Stloc_0;
			Check(1, intVal < _CurrentFrame->L.size());
			Check(1, _CurrentFrame->S.Pop(value));
			_CurrentFrame->L[intVal] = value;
			break;
		case Opcode::Stloc_S:
			ReadInt(intVal);
			Check(1, intVal < _CurrentFrame->L.size());
			Check(1, _CurrentFrame->S.Pop(value));
			_CurrentFrame->L[intVal] = value;
			break;

			// New
		case Opcode::New:
			{
				ReadInt(intVal);
				std::vector<Value> callargs(intVal);

				// For now just ignore the object type
				Check(1, _CurrentFrame->S.Pop(1));
				
				for (size_t i = 0; i < intVal; i++)
					Check(1, _CurrentFrame->S.Pop(callargs[i]));

				Reference<Object> obj = Object::New();
				obj->Construct(callargs);

				Check(1, _CurrentFrame->S.Push(obj.Value()));
			}
			break;

			// Math
#define MathOp(name) \
		case Opcode::name: \
			Check(1, _CurrentFrame->S.Pop(value)); \
			Check(1, _CurrentFrame->S.Push(value.AsObject()->name##Op())); \
			break;
#define MathOp2(name) \
		case Opcode::name: \
			Check(1, _CurrentFrame->S.Pop(value2)); \
			Check(1, _CurrentFrame->S.Pop(value)); \
			Check(1, _CurrentFrame->S.Push(value.AsObject()->name##Op(value2))); \
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
				
				Check(1, _CurrentFrame->S.Pop(value));

				for (size_t i = 0; i < intVal; i++)
					Check(1, _CurrentFrame->S.Pop(callargs[i]));

				Check(1, _CurrentFrame->S.Push(value.AsObject()->Call(callargs)));
			}
			break;
		case Opcode::Return:
			Check(1, _CurrentFrame->S.Pop(retValue));
			State(ExecutionState::Stopped);
			break;

			// Jumps
		case Opcode::Jump:
			ReadInt(intVal);
			Check(1, intVal < _CurrentFrame->F->BytecodeSize());
			_CurrentFrame->IP = intVal;
			break;
		case Opcode::Brtrue:
			ReadInt(intVal);
			Check(1, intVal < _CurrentFrame->F->BytecodeSize());
			Check(1, _CurrentFrame->S.Pop(value));
			if (value.AsBoolean() == true)
				_CurrentFrame->IP = intVal;
			break;
		case Opcode::Brfalse:
			ReadInt(intVal);
			Check(1, intVal < _CurrentFrame->F->BytecodeSize());
			Check(1, _CurrentFrame->S.Pop(value));
			if (value.AsBoolean() == false)
				_CurrentFrame->IP = intVal;
			break;
		}

#undef HasBytes
#undef Check
#undef Read4Byte
#undef Read1Byte
	}

	if (Error() == 0)
	{
		delete _CurrentFrame;
		_CurrentFrame = nullptr;

		if (_CallStack.size() > 0)
		{
			State(ExecutionState::Running);
			_CurrentFrame = _CallStack.top();
			_CallStack.pop();
		}
		else
		{
			State(ExecutionState::Stopped);
		}
	}

	return true;
}
