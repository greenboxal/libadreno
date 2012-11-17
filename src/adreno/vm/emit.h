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

#ifndef ADRENOEMIT_H
#define ADRENOEMIT_H

#include <cstdint>
#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/vm.h>
#include <adreno/vm/assembly.h>
#include <adreno/utils/memorystream.h>

namespace Adreno
{
	struct Label;
	class FunctionEmitter
	{
	public:
		FunctionEmitter(const String &name)
			: _Name(name)
		{
			StackSize(0);
			LocalCount(0);
		}

		Label *CreateLabel();

		void EmitOp(unsigned char op, unsigned char prefix = 0);
		void EmitOp2(unsigned char op, Label *target, unsigned char prefix = 0);
		void EmitOp2(unsigned char op, std::uint32_t p1, unsigned char prefix = 0);
		void EmitDOp2(unsigned char op, double p1, unsigned char prefix = 0);

		void Finish();

		DEFPROP_RO_R(public, MemoryStream, Stream);
		DEFPROP_RW(public, size_t, StackSize);
		DEFPROP_RW(public, size_t, LocalCount);

	private:
		String _Name;

		struct RelocationEntry
		{
			size_t IP;
			Label *label;
		};

		std::list<RelocationEntry> _Relocs;
		std::list<Label> _Labels;

		friend class AssemblyBuilder;
	};

	struct Label
	{
	public:
		Label(const Label &label)
		{
			_IP = label._IP;
			_Owner = label._Owner;
		}

		void Bind()
		{
			_IP = _Owner->Stream().Tell();
		}

	private:
		Label(FunctionEmitter *owner)
		{
			_Owner = owner;
			_IP = 0xFFFFFFFF;
		}

		size_t _IP;
		FunctionEmitter *_Owner;

		friend class FunctionEmitter;
	};

	class AssemblyBuilder
	{
	public:
		~AssemblyBuilder()
		{
			std::unordered_map<String, FunctionEmitter *>::iterator it;
			for (it = _Functions.begin(); it != _Functions.end(); it++)
				delete it->second;
		}

		void AddToStringPool(const String &str)
		{
			_Strings[str.Hash()] = str;
		}

		void AddFunction(FunctionEmitter *fe)
		{
			_Functions[fe->_Name] = fe;
		}

		void *Save(size_t *size);
		Reference<Assembly> ToAssembly();

	private:
		std::unordered_map<std::uint32_t, String> _Strings;
		std::unordered_map<String, FunctionEmitter *> _Functions;
	};
}

#endif
