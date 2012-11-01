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

#include <adreno/vm/value.h>
#include <adreno/vm/object.h>

#include <string.h>

using namespace Adreno;

Value::Value()
{
	Type(ValueType::Null);
}

Value::Value(intptr_t value)
{
	Type(ValueType::Number);
	_Values.number = value;
}

Value::Value(double value)
{
	Type(ValueType::FloatingNumber);
	_Values.floatingNumber = value;
}

Value::Value(bool value)
{
	Type(ValueType::Boolean);
	_Values.boolean = value;
}

Value::Value(const String &string)
{
	Type(ValueType::String);
	_Values.stringImpl = string._Impl;
	_Values.stringImpl->IncRef();
}

Value::Value(Object *object)
{
	Type(ValueType::Object);

	_Values.object = object;
	if (_Values.object)
		_Values.object->GCState()->Reference(ReferenceType::Strong);
}

Value::~Value()
{
	DereferenceMe();
}

void Value::SetNull()
{
	Type(ValueType::Null);
	DereferenceMe();
}

void Value::SetValue(intptr_t value)
{
	DereferenceMe();
	Type(ValueType::Number);
	_Values.number = value;
}

void Value::SetValue(double value)
{
	DereferenceMe();
	Type(ValueType::FloatingNumber);
	_Values.floatingNumber = value;
}

void Value::SetValue(bool value)
{
	DereferenceMe();
	Type(ValueType::Boolean);
	_Values.boolean = value;
}

void Value::SetValue(const String &string)
{
	DereferenceMe();
	Type(ValueType::String);
	_Values.stringImpl = string._Impl;
	_Values.stringImpl->IncRef();
}

void Value::SetValue(Object *object)
{
	DereferenceMe();
	Type(ValueType::Object);

	_Values.object = object;
	if (_Values.object)
		_Values.object->GCState()->Reference(ReferenceType::Strong);
}

void Value::DereferenceMe()
{
	if (Type() == ValueType::Object && _Values.object)
		_Values.object->GCState()->Dereference(ReferenceType::Strong);
	else if (Type() == ValueType::String && _Values.stringImpl)
		_Values.stringImpl->DecRef();
}

intptr_t Value::AsNumber() const
{
	switch (Type())
	{
	case ValueType::Number: return _Values.number;
	case ValueType::FloatingNumber: return (intptr_t)_Values.floatingNumber;
	case ValueType::Boolean: return _Values.boolean ? 1 : 0;
	case ValueType::Object: return _Values.object->AsNumber();
	case ValueType::String:
		{
			return 0;
		}
	}

	return 0;
}

double Value::AsFloatingNumber() const
{
	switch (Type())
	{
	case ValueType::Number: return _Values.number;
	case ValueType::FloatingNumber: return _Values.floatingNumber;
	case ValueType::Boolean: return _Values.boolean ? 1 : 0;
	case ValueType::Object: return _Values.object->AsFloatingNumber();
	case ValueType::String:
		{
			return 0;
		}
	}

	return 0;
}

bool Value::AsBoolean() const
{
	switch (Type())
	{
	case ValueType::Number: return _Values.number != 0;
	case ValueType::FloatingNumber: return _Values.floatingNumber != 0;
	case ValueType::Boolean: return _Values.boolean;
	case ValueType::Object: return _Values.object->AsBoolean();
	case ValueType::String: return _Values.stringImpl->Compare(String::Static("true")._Impl, StringCompare::CaseInsensitive);
	}

	return 0;
}

String Value::AsString() const
{
	switch (Type())
	{
	case ValueType::Number:
		{
			return String::Convert(_Values.number);
		}
	case ValueType::FloatingNumber:
		{
			return String::Convert(_Values.floatingNumber);
		}
	case ValueType::Boolean:
		{
			return _Values.boolean ? "true" : "false";
		}
	case ValueType::Object: return _Values.object->AsString();
	case ValueType::String: return _Values.stringImpl;
	}

	return "undefined";
}

Reference<Object> Value::AsObject() const
{
	if (Type() == ValueType::Object)
		return _Values.object;
	else
		return Object::CreateFromValue(*this);
}
