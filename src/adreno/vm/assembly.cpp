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

using namespace Adreno;

Function::~Function()
{
	if (Bytecode())
		delete Bytecode();
}

Function::Function(const Reference<Assembly> &owner, const String &name, unsigned char *bytecode, size_t bytecodeSize, size_t localcount, size_t stacksize)
	: _Name(name)
{
	BytecodeSize(bytecodeSize);
	Bytecode(new unsigned char[BytecodeSize()]);
	LocalCount(localcount);
	StackSize(stacksize);
	Owner(owner);

	memmove(Bytecode(), bytecode, BytecodeSize());
}

Value Function::Call(const Arguments &args)
{
	Value ret;

	Context *ctx = Context::Current();
	if (ctx == nullptr)
		return Value();

	ctx->Run(this, args, ret);

	return ret;
}

bool Assembly::Load(void *, size_t)
{
	return false;
}
