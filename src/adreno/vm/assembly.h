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

#include <memory.h>

namespace Adreno
{
	class BytecodeFunction
	{
	public:
		virtual ~BytecodeFunction()
		{
			if (Bytecode())
				delete Bytecode();
		}

		DEFPROP_RO_R(public, String, Name);
		DEFPROP_RO_P(public, unsigned char, Bytecode);
		DEFPROP_RO_C(public, size_t, BytecodeSize);
		DEFPROP_RO_C(public, size_t, LocalCount);

	private:
		BytecodeFunction(const String &name)
			: _Name(name)
		{
			BytecodeSize(0);
			Bytecode(nullptr);
			LocalCount(0);
		}

		BytecodeFunction(const String &name, unsigned char *bytecode, unsigned int bytecodeSize, int localcount)
			: _Name(name)
		{
			BytecodeSize(bytecodeSize);
			Bytecode(new unsigned char[BytecodeSize()]);
			memmove(Bytecode(), bytecode, BytecodeSize());

			LocalCount(localcount);
		}

		friend class FunctionEmitter;
		friend class Assembly;
	};

	class Class
	{
	public:
		typedef std::unordered_map<String, BytecodeFunction *, String::InsensitiveHasher, String::InsensitiveComparer> FunctionMap;

		Class(const String &name)
			: _Name(name)
		{
			_ParentsResolved = false;
		}

		virtual ~Class()
		{
			Class::FunctionMap::iterator it;
			for (it = _Functions.begin(); it != _Functions.end(); it++)
				delete it->second;
		}

		const std::list<String> &GetParents() const
		{
			return _Parents;
		}

		void AddParent(const String &name)
		{
			std::list<String>::iterator it;

			for (it = _Parents.begin(); it != _Parents.end(); it++)
				if (name.Compare(*it, StringCompare::CaseInsensitive))
					return;

			_Parents.push_back(name);
		}
		
		const FunctionMap GetFunctions() const
		{
			return _Functions;
		}

		BytecodeFunction *GetFunction(const String &name) const
		{
			FunctionMap::const_iterator it = _Functions.find(name);

			if (it == _Functions.end())
				return nullptr;

			return it->second;
		}

		void SetFunction(const String &name, BytecodeFunction *function)
		{
			_Functions[name] = function;
		}
		
		void ResolveParents();

		DEFPROP_RO_R(public, String, Name);

	private:
		bool _ParentsResolved;
		std::list<String> _Parents;
		FunctionMap _Functions;
	};

	class Assembly
	{
	public:
		typedef std::unordered_map<String, Class *, String::InsensitiveHasher, String::InsensitiveComparer> ClassMap;
		
		Assembly()
		{

		}

		virtual ~Assembly()
		{
			ClassMap::iterator it;
			for (it = _Classes.begin(); it != _Classes.end(); it++)
				delete it->second;

			Class::FunctionMap::iterator it2;
			for (it2 = _Functions.begin(); it2 != _Functions.end(); it2++)
				delete it2->second;
		}

		const Assembly::ClassMap GetClasses() const
		{
			return _Classes;
		}

		Class *GetClass(const String &name) const
		{
			ClassMap::const_iterator it = _Classes.find(name);

			if (it == _Classes.end())
				return nullptr;

			return it->second;
		}

		void SetClass(const String &name, Class *function)
		{
			_Classes[name] = function;
		}

		BytecodeFunction *GetFunction(const String &name) const
		{
			Class::FunctionMap::const_iterator it = _Functions.find(name);

			if (it == _Functions.end())
				return nullptr;

			return it->second;
		}

		void SetFunction(const String &name, BytecodeFunction *function)
		{
			_Functions[name] = function;
		}

		bool Load(void *memory, size_t size);

	private:
		ClassMap _Classes;
		Class::FunctionMap _Functions;

		friend class AssemblyFactory;
	};
}

#endif
