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

#ifndef ADRENOOBJECT_H
#define ADRENOOBJECT_H

#include <string>
#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/value.h>
#include <adreno/utils/memorypool.h>

namespace Adreno
{
	class Object
	{
	public:
		typedef std::unordered_map<std::string, Reference<Object>, std::hash<std::string>, std::equal_to<std::string>,  MemoryPoolAllocator<std::pair<std::string, Reference<Object> > > > FieldMap;

		Object();
		virtual ~Object();

		virtual bool Finalize();

#define DummyOp(name) virtual Value name##Op() { return Value(nullptr); }
#define DummyOp2(name) virtual Value name##Op(const Value &) { return Value(nullptr); }

		DummyOp2(Add)
		DummyOp2(Sub)
		DummyOp2(Mult)
		DummyOp2(Div)
		DummyOp2(Rem)
		DummyOp(Neg)

		DummyOp2(And)
		DummyOp2(Or)
		DummyOp2(Xor)
		DummyOp(Not)
		DummyOp2(LeftShift)
		DummyOp2(RightShift)

		DummyOp2(LogAnd)
		DummyOp2(LogOr)
		DummyOp2(LogNot)

		DummyOp2(Equal)
		DummyOp2(NotEqual)
		DummyOp2(Greater)
		DummyOp2(GreaterEq)
		DummyOp2(Lesser)
		DummyOp2(LesserEq)
		
#undef DummyOp2
#undef DummyOp

		static Adreno::Reference<Object> CreateFromValue(const Value &value);

		DEFPROP_P_RO_R(public, FieldMap, Fields);
		DEFPROP_P_RO_P(public, GCObject, GCState);
	};
}

#endif
