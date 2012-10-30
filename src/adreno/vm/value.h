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

#ifndef ADRENOVALUE_H
#define ADRENOVALUE_H

#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/gc.h>

namespace Adreno
{
	enum class ValueType
	{
		Null,
		Number,
		FloatingNumber,
		Boolean,
		String,
		Object
	};

	class Object;
	class Value
	{
	public:
		union ValueUnion
		{
			size_t Number;
			double FloatingNumber;
			bool Boolean;
			Object *Object;
			struct
			{
				char *Data;
				size_t Size;
			} String;
		};

		Value();
		Value(size_t value);
		Value(double value);
		Value(bool value);
		Value(char *data, size_t size);
		Value(Object *object);
		~Value();
		
		void SetNull();
		void SetValue(size_t value);
		void SetValue(double value);
		void SetValue(bool value);
		void SetValue(char *data, size_t size);
		void SetValue(Object *object);

		DEFPROP_RO_RC(public, ValueUnion, Values);
		DEFPROP_RO_C(public, ValueType, Type);

	private:
		void DereferenceMe();
	};
}

#endif
