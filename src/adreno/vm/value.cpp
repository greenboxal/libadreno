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

Value::Value(size_t value)
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

Value::Value(char *data, size_t size)
{
	Type(ValueType::String);
	_Values.String.Data = data;
	_Values.String.Size = size;
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
	if (Type() == ValueType::Object && _Values.Object)
		_Values.Object->GCState()->Dereference(ReferenceType::Strong);
}

void Value::SetNull()
{
	Type(ValueType::Null);
	DereferenceMe();
}

void Value::SetValue(size_t value)
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

void Value::SetValue(char *data, size_t size)
{
	DereferenceMe();
	Type(ValueType::String);
	_Values.String.Data = data;
	_Values.String.Size = size;
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
	if (_Values.Object)
		_Values.Object->GCState()->Dereference(ReferenceType::Strong);
}
