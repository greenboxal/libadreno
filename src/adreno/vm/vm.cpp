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

#include <adreno/vm/vm.h>

using namespace Adreno;

THREAD_LOCAL VMContext *VMContext::_CurrentVM;

VMContext::VMContext()
{
	GC(new GarbageCollector());
}

VMContext::~VMContext()
{
	if (_CurrentVM == this)
		_CurrentVM = nullptr;

	delete GC();
}

void VMContext::MakeCurrent()
{
	_CurrentVM = this;
}

Class *VMContext::GetClass(const String &name) const
{
	Assembly::ClassMap::const_iterator it = _Classes.find(name);

	if (it == _Classes.end())
		return nullptr;

	return it->second;
}

void VMContext::SetClass(const String &name, Class *function)
{
	_Classes[name] = function;
}

Value VMContext::GetGlobal(const String &name)
{
	Object::FieldMap::iterator it = _Globals.find(name);

	if (it == _Globals.end())
		return Value();

	return it->second;
}

void VMContext::SetGlobal(const String &name, const Value &value)
{
	_Globals[name] = value;
}

