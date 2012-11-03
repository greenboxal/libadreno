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
#include <adreno/vm/assembly.h>

#include <assert.h>

using namespace Adreno;

bool Assembly::Load(void *, size_t)
{
	return false;
}

void Class::ResolveParents()
{
	std::list<String>::iterator it;

	if (_ParentsResolved)
		return;

	for (it = _Parents.begin(); it != _Parents.end(); it++)
	{
		Class *parent = VMContext::CurrentVM()->GetClass(*it);

		assert(parent);
		parent->ResolveParents();

		FunctionMap &fmap = parent->_Functions;
		FunctionMap::iterator it2;
		for (it2 = fmap.begin(); it2 != fmap.end(); it2++)
		{
			if (GetFunction(it2->first) == nullptr)
				SetFunction(it2->second);
		}
	}

	_ParentsResolved = true;
}
