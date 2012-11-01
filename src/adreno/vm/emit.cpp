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

#include <adreno/config.h>
#include <adreno/vm/emit.h>
#include <assert.h>

using namespace Adreno;

Label *FunctionEmitter::CreateLabel()
{
	_Labels.push_back(this);
	return &_Labels.back();
}

void FunctionEmitter::EmitOp(unsigned char op, unsigned char prefix)
{
	char buff[2];

	buff[0] = prefix;
	buff[1] = op;

	Stream().Write(buff, 0, sizeof(buff));
}

void FunctionEmitter::EmitOp(unsigned char op, Label *target, int prefix)
{
	char buff[6];

	buff[0] = prefix;
	buff[1] = op;
	*((std::uint32_t *)&buff[2]) = 0;

	RelocationEntry re;
	re.IP = Stream().Tell() + 2;
	re.label = target;
	_Relocs.push_back(re);

	Stream().Write(buff, 0, sizeof(buff));
}

void FunctionEmitter::EmitOp(unsigned char op, std::uint32_t p1, int prefix)
{
	char buff[6];

	buff[0] = prefix;
	buff[1] = op;
	*((std::uint32_t *)&buff[2]) = p1;

	Stream().Write(buff, 0, sizeof(buff));
}

void FunctionEmitter::EmitOp(unsigned char op, std::uint32_t p1, std::uint32_t p2, int prefix)
{
	char buff[10];

	buff[0] = prefix;
	buff[1] = op;
	*((std::uint32_t *)&buff[2]) = p1;
	*((std::uint32_t *)&buff[6]) = p2;

	Stream().Write(buff, 0, sizeof(buff));
}

void FunctionEmitter::Finish()
{
	std::list<RelocationEntry>::iterator it;

	BytecodeSize(Stream().Tell());
	Bytecode((unsigned char *)Stream().Clone());
	Stream().Close();

	for (it = _Relocs.begin(); it != _Relocs.end(); it++)
	{
		assert(it->label);
		assert(it->label->_Owner == this);
		assert(it->label->_IP != 0xFFFFFFFF);
		
		*((std::uint32_t *)(Bytecode() + it->IP)) = it->label->_IP;
	}

	_Relocs.clear();
	_Labels.clear();
}

void *AssemblyBuilder::Save(size_t *size)
{
	return nullptr;
}
