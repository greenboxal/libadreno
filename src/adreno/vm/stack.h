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

#ifndef ADRENOSTACK_H
#define ADRENOSTACK_H

#include <stack>
#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/value.h>
#include <adreno/vm/object.h>

namespace Adreno
{
	class Stack
	{
	public:
		Stack();
		~Stack();

		bool Push(const Value &value);
		bool Take(Value **address, size_t count);
		bool Pop(Value &value);
		bool Pop(size_t count);
		void Clear();

	private:
		Value *_Stack;
		size_t _Pointer, _Size;
	};
}

#endif
