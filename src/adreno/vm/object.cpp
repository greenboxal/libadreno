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

#include <adreno/vm/object.h>

#include <string.h>

using namespace Adreno;

Object::Object()
{
	GCState(new GCObject(this));
	GCState()->Reference(ReferenceType::Weak);
}

Object::~Object()
{
	GCState()->Dereference(ReferenceType::Weak);
}

bool Object::Finalize()
{
	Fields().clear();

	return true;
}

Adreno::Reference<Object> Object::CreateFromValue(const Value &)
{
	return Reference<Object>(new Object());
}

Value Object::GetField(String name)
{
	FieldMap::iterator it = Fields().find(name);

	if (it == Fields().end())
		return Value();

	return it->second;
}

void Object::SetField(String name, const Value &value)
{
	Fields()[name] = value;
}

#define DEF_OP(name) \
Value Object::name##Op() \
{ \
	Value field = GetField("op_"#name); \
	if (field.Type() != ValueType::Null) \
		return field.AsObject()->Call(Arguments()); \
	return Value(); \
}

#define DEF_OP2(name) \
Value Object::name##Op(const Value &value) \
{ \
	Value field = GetField("op_"#name); \
	if (field.Type() != ValueType::Null) \
		return field.AsObject()->Call(Arguments(&value, 1)); \
	return Value(); \
}

DEF_OP2(Add)
DEF_OP2(Sub)
DEF_OP2(Mult)
DEF_OP2(Div)
DEF_OP2(Rem)
DEF_OP(Neg)

DEF_OP2(And)
DEF_OP2(Or)
DEF_OP2(Xor)
DEF_OP(Not)
DEF_OP2(LeftShift)
DEF_OP2(RightShift)

DEF_OP2(LogAnd)
DEF_OP2(LogOr)
DEF_OP(LogNot)

DEF_OP2(Equal)
DEF_OP2(NotEqual)
DEF_OP2(Greater)
DEF_OP2(GreaterEq)
DEF_OP2(Lesser)
DEF_OP2(LesserEq)

#undef DEF_OP
#undef DEF_OP2

Reference<Object> Object::New()
{
	return new Object();
}

Reference<FunctionObject> FunctionObject::New(std::function<Value (const Arguments &)> function)
{
	return new FunctionObject(function);
}

Reference<NumeralObject> NumeralObject::New(std::int_fast32_t value)
{
	return new NumeralObject(value);
}

Reference<FloatingNumeralObject> FloatingNumeralObject::New(double value)
{
	return new FloatingNumeralObject(value);
}

Reference<StringObject> StringObject::New(const String &string)
{
	return new StringObject(string);
}
