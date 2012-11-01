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

#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/vm.h>
#include <adreno/vm/assembly.h>
#include <adreno/utils/memorystream.h>

namespace Adreno
{
	struct Label;
	class FunctionEmitter : BytecodeFunction
	{
	public:
		FunctionEmitter(const String &name)
			: BytecodeFunction(name)
		{

		}

		Label *CreateLabel();

		void SetLocalCount(size_t count)
		{
			LocalCount(count);
		}

		void EmitOp(unsigned char op, unsigned char prefix = 0);
		void EmitOp(unsigned char op, Label *target, int prefix = 0);
		void EmitOp(unsigned char op, std::uint32_t p1, int prefix = 0);
		void EmitOp(unsigned char op, std::uint32_t p1, std::uint32_t p2, int prefix = 0);

		void Finish();

		DEFPROP_RO_R(public, MemoryStream, Stream);

	private:
		struct RelocationEntry
		{
			size_t IP;
			Label *label;
		};

		std::list<RelocationEntry> _Relocs;
		std::list<Label> _Labels;
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

	class AssemblyBuilder : Assembly
	{
	public:
		typedef std::unordered_map<std::uint32_t, String> StringMap;

		void AddToStringPool(const String &str)
		{
			_Strings[str.Hash()] = str;
		}

		void *Save(size_t *size);

	private:
		StringMap _Strings;
	};
}

#endif
