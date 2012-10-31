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

Value::Value(std::int_fast32_t value)
{
	Type(ValueType::Number);
	_Values.Number = value;
}

Value::Value(double value)
{
	Type(ValueType::FloatingNumber);
	_Values.FloatingNumber = value;
}

Value::Value(bool value)
{
	Type(ValueType::Boolean);
	_Values.Boolean = value;
}

Value::Value(const String &string)
{
	Type(ValueType::String);
	_Values.StringImpl = string._Impl;
	_Values.StringImpl->IncRef();
}

Value::Value(Object *object)
{
	Type(ValueType::Object);

	_Values.Object = object;
	if (_Values.Object)
		_Values.Object->GCState()->Reference(ReferenceType::Strong);
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

void Value::SetValue(std::int_fast32_t value)
{
	DereferenceMe();
	Type(ValueType::Number);
	_Values.Number = value;
}

void Value::SetValue(double value)
{
	DereferenceMe();
	Type(ValueType::FloatingNumber);
	_Values.FloatingNumber = value;
}

void Value::SetValue(bool value)
{
	DereferenceMe();
	Type(ValueType::Boolean);
	_Values.Boolean = value;
}

void Value::SetValue(const String &string)
{
	DereferenceMe();
	Type(ValueType::String);
	_Values.StringImpl = string._Impl;
	_Values.StringImpl->IncRef();
}

void Value::SetValue(Object *object)
{
	DereferenceMe();
	Type(ValueType::Object);

	_Values.Object = object;
	if (_Values.Object)
		_Values.Object->GCState()->Reference(ReferenceType::Strong);
}

void Value::DereferenceMe()
{
	if (Type() == ValueType::Object && _Values.Object)
		_Values.Object->GCState()->Dereference(ReferenceType::Strong);
	else if (Type() == ValueType::String && _Values.StringImpl)
		_Values.StringImpl->DecRef();
}

std::int_fast32_t Value::AsNumber() const
{
	switch (Type())
	{
	case ValueType::Number: return _Values.Number;
	case ValueType::FloatingNumber: return (std::int_fast32_t)_Values.FloatingNumber;
	case ValueType::Boolean: return _Values.Boolean ? 1 : 0;
	case ValueType::Object: return _Values.Object->AsNumber();
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
	case ValueType::Number: return _Values.Number;
	case ValueType::FloatingNumber: return _Values.FloatingNumber;
	case ValueType::Boolean: return _Values.Boolean ? 1 : 0;
	case ValueType::Object: return _Values.Object->AsFloatingNumber();
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
	case ValueType::Number: return _Values.Number != 0;
	case ValueType::FloatingNumber: return _Values.FloatingNumber != 0;
	case ValueType::Boolean: return _Values.Boolean;
	case ValueType::Object: return _Values.Object->AsBoolean();
	case ValueType::String: return _Values.StringImpl->Compare(String::Static("true")._Impl, StringCompare::CaseInsensitive);
	}

	return 0;
}

String Value::AsString() const
{
	switch (Type())
	{
	case ValueType::Number:
		{
			static char buffer[32];

			sprintf(buffer, "%d", _Values.Number);

			return buffer;
		}
	case ValueType::FloatingNumber:
		{
			static char buffer[32];

			sprintf(buffer, "%f", _Values.FloatingNumber);

			return buffer;
		}
	case ValueType::Boolean:
		{
			return _Values.Boolean ? String::Static("true") : String::Static("false");
		}
	case ValueType::Object: return _Values.Object->AsString();
	case ValueType::String: return _Values.StringImpl;
	}

	return String::Static("undefined");
}

Reference<Object> Value::AsObject() const
{
	if (Type() == ValueType::Object)
		return _Values.Object;
	else
		return Object::CreateFromValue(*this);
}
