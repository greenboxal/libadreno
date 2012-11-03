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
	int extra = 0;

	if ((op & Opcode::Mask) == 0)
		return;

	if (prefix == Prefix::Extra)
		prefix = 0;

	if (prefix && (prefix & Prefix::Mask) == 0)
	{
		op |= Prefix::Extra;
		extra = 1;
	}
	else if (prefix)
	{
		op |= prefix;
	}

	buff[0] = op;
	if (extra == 1)
		buff[1] = prefix;

	Stream().Write(buff, 0, 1 + extra);
}

void FunctionEmitter::EmitOp2(unsigned char op, Label *target, unsigned char prefix)
{
	char buff[6];
	int extra = 0;

	if ((op & Opcode::Mask) == 0)
		return;

	if (prefix == Prefix::Extra)
		prefix = 0;

	if (prefix && (prefix & Prefix::Mask) == 0)
	{
		op |= Prefix::Extra;
		extra = 1;
	}
	else if (prefix)
	{
		op |= prefix;
	}

	buff[0] = op;
	if (extra == 1)
		buff[1] = prefix;

	*((std::uint32_t *)&buff[1 + extra]) = 0;

	RelocationEntry re;
	re.IP = Stream().Tell() + 1 + extra;
	re.label = target;
	_Relocs.push_back(re);

	Stream().Write(buff, 0, 5 + extra);
}

void FunctionEmitter::EmitOp2(unsigned char op, std::uint32_t p1, unsigned char prefix)
{
	char buff[2 + sizeof(std::uint32_t)];
	int extra = 0;

	if ((op & Opcode::Mask) == 0)
		return;

	if (prefix == Prefix::Extra)
		prefix = 0;

	if (prefix && (prefix & Prefix::Mask) == 0)
	{
		op |= Prefix::Extra;
		extra = 1;
	}
	else if (prefix)
	{
		op |= prefix;
	}

	buff[0] = op;
	if (extra == 1)
		buff[1] = prefix;

	*((std::uint32_t *)&buff[1 + extra]) = p1;

	Stream().Write(buff, 0, 1 + sizeof(std::uint32_t) + extra);
}

void FunctionEmitter::EmitDOp2(unsigned char op, double p1, unsigned char prefix)
{
	char buff[2 + sizeof(double)];
	int extra = 0;

	if ((op & Opcode::Mask) == 0)
		return;

	if (prefix == Prefix::Extra)
		prefix = 0;

	if (prefix && (prefix & Prefix::Mask) == 0)
	{
		op |= Prefix::Extra;
		extra = 1;
	}
	else if (prefix)
	{
		op |= prefix;
	}

	buff[0] = op;
	if (extra == 1)
		buff[1] = prefix;

	*((double *)&buff[1 + extra]) = p1;

	Stream().Write(buff, 0, 1 + sizeof(double) + extra);
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

void *AssemblyBuilder::Save(size_t *)
{
	return nullptr;
}
