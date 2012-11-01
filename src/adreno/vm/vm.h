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

#ifndef ADRENOVM_H
#define ADRENOVM_H

#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/value.h>
#include <adreno/vm/object.h>
#include <adreno/vm/assembly.h>

namespace Adreno
{
	class ExecutionContext;
	class VMContext
	{
	public:
		static VMContext *CurrentVM()
		{
			return _CurrentVM;
		}

		VMContext();
		~VMContext();

		void MakeCurrent();

		ExecutionContext *CreateExecutionContext(Assembly *assembly);
		
		const Object::FieldMap GetGlobals() const
		{
			return _Globals;
		}

		Class *GetClass(const String &name) const;
		void SetClass(const String &name, Class *function);

		const Assembly::ClassMap GetClasses() const
		{
			return _Classes;
		}

		Value GetGlobal(const String &name);
		void SetGlobal(const String &name, const Value &value);

		DEFPROP_RO_P(public, GarbageCollector, GC);

	private:
		Object::FieldMap _Globals;
		Assembly::ClassMap _Classes;

		static THREAD_LOCAL VMContext *_CurrentVM;
	};

	namespace ExecutionState
	{
		enum
		{
			Running,
			Suspended,
			Stopped
		};
	}

	class ExecutionContext
	{
	public:
		ExecutionContext();
		~ExecutionContext();

		void Run();

		DEFPROP_RO_P(public, VMContext, Owner);
		DEFPROP_RO_P(public, Assembly, Unit);
		DEFPROP_RO_P(public, int, State);
	};
}

#endif
