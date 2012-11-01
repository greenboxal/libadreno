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

#include <cstdint>
#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/string.h>

namespace Adreno
{
	namespace ValueType
	{
		enum
		{
			Null,
			Number,
			FloatingNumber,
			Boolean,
			String,
			Object
		};
	}

	class Object;
	class Value
	{
	public:

		Value();
		Value(std::int_fast32_t value);
		Value(double value);
		Value(bool value);
		Value(const String &string);
		Value(Object *object);
		~Value();
		
		void SetNull();
		void SetValue(std::int_fast32_t value);
		void SetValue(double value);
		void SetValue(bool value);
		void SetValue(const String &value);
		void SetValue(Object *object);

		std::int_fast32_t AsNumber() const;
		double AsFloatingNumber() const;
		bool AsBoolean() const;
		String AsString() const;
		Reference<Object> AsObject() const;
		
		DEFPROP_RO_C(public, int, Type);

	private:
		union
		{
			std::int_fast32_t number;
			double floatingNumber;
			bool boolean;
			Object *object;
			String::SharedImpl *stringImpl;
		} _Values;

		void DereferenceMe();
	};
}

#endif
