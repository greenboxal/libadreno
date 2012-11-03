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

#include <adreno/config.h>
#include <adreno/vm/stack.h>

#include <stdlib.h>

using namespace Adreno;

void InitValues(Value *value, size_t count)
{
	for (size_t i = 0; i < count; i++)
		new (&value[i]) Value();
}

Stack::Stack(size_t stackSize)
{
	_Pointer = 0;
	_Size = stackSize;

	if (stackSize > 0)
		_Stack = (Value *)malloc(sizeof(Value) * stackSize);
	else
		_Stack = nullptr;

	InitValues(_Stack, _Size);
}

Stack::~Stack()
{
	if (_Stack)
	{
		Clear();
		free(_Stack);
	}
}

bool Stack::Push(const Value &value)
{
	if (_Pointer >= _Size)
	{
		size_t oldSize = _Size;

		_Size += ADRENOSTACK_EXPANSION_FACTOR;
		_Stack = (Value *)realloc(_Stack, sizeof(Value) * _Size);

		InitValues(&_Stack[oldSize], _Size - oldSize);
	}

	_Stack[_Pointer++] = value;

	return true;
}

bool Stack::Take(Value **address, size_t count)
{
	if (_Pointer + count - 1 >= _Size)
	{
		size_t oldSize = _Size;

		_Size += ((count + ADRENOSTACK_EXPANSION_FACTOR - 1) / ADRENOSTACK_EXPANSION_FACTOR) * ADRENOSTACK_EXPANSION_FACTOR;
		_Stack = (Value *)realloc(_Stack, sizeof(Value) * _Size);

		InitValues(&_Stack[oldSize], _Size - oldSize);
	}

	*address = &_Stack[_Pointer];
	_Pointer += count;

	return true;
}

bool Stack::Pop(Value &value)
{
	if (_Pointer == 0)
		return false;

	value = _Stack[_Pointer];
	_Stack[_Pointer].~Value();

	return true;
}

bool Stack::Pop(size_t count)
{
	if (count > _Pointer)
		return false;

	for (size_t i = 0; i < count; i++)
		_Stack[_Pointer - i - 1].~Value();

	_Pointer -= count;
	return true;
}

void Stack::Clear()
{
	while (_Pointer > 0)
		_Stack[--_Pointer].~Value();
}
