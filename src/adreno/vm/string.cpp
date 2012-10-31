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

#include <adreno/vm/string.h>
#include <string.h>

using namespace Adreno;

String::SharedImpl::SharedImpl(char *str)
{
	Size(strlen(str));
	Data(new char[Size() + 1]);
	memmove(Data(), str, Size());
	Data()[Size()] = 0;
	_ReferenceCount = 0;

	MakeHashes();
}

String::SharedImpl::SharedImpl(char *str, size_t size)
{
	Size(size);
	Data(new char[Size() + 1]);
	memmove(Data(), str, Size());
	Data()[Size()] = 0;
	_ReferenceCount = 0;

	MakeHashes();
}

String::SharedImpl::SharedImpl(const std::string &str)
{
	Size(str.size());
	Data(new char[Size() + 1]);
	memmove(Data(), str.c_str(), Size());
	Data()[Size()] = 0;
	_ReferenceCount = 0;

	MakeHashes();
}

String::SharedImpl::SharedImpl(SharedImpl *s1, SharedImpl *s2)
{
	Size(s1->Size() + s2->Size());
	Data(new char[Size() + 1]);
	memmove(Data(), s1->Data(), s1->Size());
	memmove(Data() + s1->Size(), s2->Data(), s2->Size());
	Data()[Size()] = 0;
	_ReferenceCount = 0;

	MakeHashes();
}

String::SharedImpl::~SharedImpl()
{
	delete[] Data();
}

void String::SharedImpl::IncRef()
{
	_ReferenceCount++;
}

void String::SharedImpl::DecRef()
{
	_ReferenceCount--;

	if (_ReferenceCount <= 0)
		delete this;
}

bool String::SharedImpl::Compare(String::SharedImpl *other, StringCompare flags) const
{
	if (flags == StringCompare::CaseSensitive)
		return Hash() == other->Hash();
	else
		return InsensitiveHash() == other->InsensitiveHash();
}

String String::Static(char *str)
{
	return SharedImpl::NewStatic(str, strlen(str));
}

String String::Static(char *str, size_t size)
{
	return SharedImpl::NewStatic(str, size);
}

String::SharedImpl *String::SharedImpl::NewStatic(char *str, size_t size)
{
	SharedImpl *impl = new SharedImpl();

	impl->Data(str);
	impl->Size(size);
	impl->MakeHashes();

	return impl;
}

void String::SharedImpl::MakeHashes()
{
	{
		size_t hval = 0x811c9dc5;
		unsigned char *bp = (unsigned char *)Data();
		unsigned char *be = bp + Size();

		while (bp < be)
		{
			hval ^= (size_t)*bp++;
			hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
		}

		Hash(hval);
	}

	{
		size_t hval = 0x811c9dc5;
		unsigned char *bp = (unsigned char *)Data();
		unsigned char *be = bp + Size();

		while (bp < be)
		{
			hval ^= (size_t)tolower(*bp++);
			hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
		}

		InsensitiveHash(hval);
	}

	_HasHash = true;
}
