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

#ifndef ADRENOASSEMBLY_H
#define ADRENOASSEMBLY_H

#include <list>
#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/string.h>
#include <adreno/vm/object.h>

namespace Adreno
{
	class Assembly;
	class Function : public Object
	{
	public:
		virtual ~Function();
		virtual Value Call(const Arguments &args);

		DEFPROP_RO_R(public, String, Name);
		DEFPROP_RO_P(public, unsigned char, Bytecode);
		DEFPROP_RO_C(public, size_t, BytecodeSize);
		DEFPROP_RO_C(public, size_t, LocalCount);
		DEFPROP_RO_C(public, size_t, StackSize);
		DEFPROP_RO_R(public, Reference<Assembly>, Owner);

	private:
		Function(const Reference<Assembly> &owner, const String &name, unsigned char *bytecode, size_t bytecodeSize, size_t localcount, size_t stacksize);

		friend class Assembly;
		friend class AssemblyBuilder;
	};

	class Assembly : public Object
	{
	public:
		bool Load(void *memory, size_t size);

		Reference<Function> GetFunction(const String &name)
		{
			const Value &value = GetField(name);

			if (value.Type() != ValueType::Object)
				return nullptr;

			return (Function *)value.AsObject().Value();
		}

		String GetString(std::uint32_t hash)
		{
			std::unordered_map<std::uint32_t, String>::iterator it = _Strings.find(hash);

			if (it == _Strings.end())
				return String::Sealed(0, 0);

			return it->second;
		}

	private:
		std::unordered_map<std::uint32_t, String> _Strings;
		
		friend class AssemblyBuilder;
	};
}

#endif
