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

#include <stddef.h>

// TODO: Move somewhere
#if defined( _MSC_VER ) && !defined( ssize_t )
typedef SSIZE_T ssize_t;
#endif

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
		explicit Value(ssize_t value);
		explicit Value(bool value);
		Value(double value);
		Value(const String &string); // not explicit for const char* conversion
		Value(Object *object);
		~Value();
		
		void SetNull();
		void SetValue(ssize_t value);
		void SetValue(double value);
		void SetValue(bool value);
		void SetValue(const String &value);
		void SetValue(Object *object);

		ssize_t AsNumber() const;
		double AsFloatingNumber() const;
		bool AsBoolean() const;
		String AsString() const;
		Reference<Object> AsObject() const;
		
		DEFPROP_RO_C(public, int, Type);

	private:
		union
		{
			ssize_t number;
			double floatingNumber;
			bool boolean;
			Object *object;
			String::SharedImpl *stringImpl;
		} _Values;

		void DereferenceMe();
	};
}

#endif
