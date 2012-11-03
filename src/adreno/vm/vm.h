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

#ifndef ADRENOVM_H
#define ADRENOVM_H

#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/value.h>
#include <adreno/vm/stack.h>
#include <adreno/vm/object.h>
#include <adreno/vm/assembly.h>

namespace Adreno
{
	namespace Prefix
	{
		enum Prefixes : unsigned char
		{
			// 
			None = 0x00,

			// ref : Not implemented yet, placeholder
			Reference = 0x40,

			// Internal Use Only
			Extra = 0x80,

			// Prefixes outside this mask encode the op byte with Extra prefix and the real prefix is saved in the second byte
			Mask = 0xC0,
		};
	}

	namespace Opcode
	{
		enum Opcodes : unsigned char
		{
			// nop
			Nop,

			// pop, pop.s
			Pop,
			Pop_S,
			
			// ldnull
			// ldnum <4byte : signed int>
			// ldnum.m1
			// ldnum.0
			// ldnum.1
			// ldfloat <8bytes : double>
			// ldstr <4bytes : str sensitive hash>
			// ldhash <4bytes : str insensitive hash>
			// ldcls
			Ldnull,
			Ldnum,
			Ldnum_M1,
			Ldnum_0,
			Ldnum_1,
			Ldfloat,
			Ldstr,
			Ldhash,
			Ldglob,
			Ldcls,
			Ldtrue,
			Ldfalse,

			// ldarg.0
			// ldarg.1
			// ldarg.2
			// ldarg.3
			// ldarg.s <4bytes : index>
			// ldargs <4bytes : starting index>
			Ldarg_0,
			Ldarg_1,
			Ldarg_2,
			Ldarg_3,
			Ldarg_S,
			Ldargs,

			// ldloc.0
			// ldloc.1
			// ldloc.2
			// ldloc.3
			// ldloc.s <4bytes : index>
			Ldloc_0,
			Ldloc_1,
			Ldloc_2,
			Ldloc_3,
			Ldloc_S,

			// stloc.0
			// stloc.1
			// stloc.2
			// stloc.3
			// stloc.s <4bytes : index>
			Stloc_0,
			Stloc_1,
			Stloc_2,
			Stloc_3,
			Stloc_S,

			// new <4bytes : arg count>
			New,

			// add, sub, mul, div, rem, neg
			Add,
			Sub,
			Mul,
			Div,
			Rem,
			Neg,

			// and, or, xor, not, shr, shl
			And,
			Or,
			Xor,
			Not,
			RightShift,
			LeftShift,

			// land, lor, lnot
			LogAnd,
			LogOr,
			LogNot,

			// eq, neq, gt, ge, lt, le
			Equal,
			NotEqual,
			Greater,
			GreaterEq,
			Lesser,
			LesserEq,

			// call <4bytes : arg count>
			Call,

			// ret
			Return,
			
			// jmp <4bytes : offset>
			Jump,

			// brtrue <4bytes : offseT>
			Brtrue,

			// brfalse <4bytes : offseT>
			Brfalse,

			Mask = 0x3F,
		};
	}

	class ExecutionContext;
	class VMContext
	{
	public:
		static VMContext *CurrentVM()
		{
			return _CurrentVM;
		}

		VMContext();
		~VMContext();

		void MakeCurrent();

		ExecutionContext *CreateExecutionContext(Assembly *assembly);
		
		const Object::FieldMap GetGlobals() const
		{
			return _Globals;
		}

		Class *GetClass(const String &name) const;
		void SetClass(const String &name, Class *function);

		const Assembly::ClassMap GetClasses() const
		{
			return _Classes;
		}

		Value GetGlobal(const String &name);
		void SetGlobal(const String &name, const Value &value);

		DEFPROP_RO_P(public, GarbageCollector, GC);

	private:
		Object::FieldMap _Globals;
		Assembly::ClassMap _Classes;

		static THREAD_LOCAL VMContext *_CurrentVM;
	};

	namespace ExecutionState
	{
		enum
		{
			Running,
			Suspended,
			Stopped
		};
	}

	class ExecutionContext
	{
	public:
		ExecutionContext(VMContext *owner, Assembly *unit);
		~ExecutionContext();

		bool Run(const String &name, const Arguments &args, Value &retValue);

		DEFPROP_RO_P(public, VMContext, Owner);
		DEFPROP_RO_P(public, Assembly, Unit);
		DEFPROP_RO_C(public, int, State);
		DEFPROP_RO_C(public, int, Error);

	private:
		size_t _IP;
		Stack *_Stack;
		std::vector<Value> _Locals;
	};
}

#endif
