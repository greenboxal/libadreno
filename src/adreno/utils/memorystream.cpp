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

#include <adreno/utils/memorystream.h>

#include <memory.h>
#include <malloc.h>

using namespace Adreno;

MemoryStream::MemoryStream()
{
	_Buffer = NULL;
	_BufferPosition = 0;
	_BufferSize = 0;
	_BufferMaxSize = 0;
	_ExpansionFactor = 100;
}

MemoryStream::MemoryStream(const MemoryStream &ms)
{
	_Buffer = NULL;
	_BufferPosition = ms._BufferPosition;
	_BufferSize = ms._BufferSize;
	_BufferMaxSize = ms._BufferMaxSize;
	_ExpansionFactor = ms._ExpansionFactor;

	if (_Buffer)
	{
		_Buffer = (unsigned char *)malloc(_BufferMaxSize);
		memcpy(_Buffer, ms._Buffer, _BufferMaxSize);
	}
}

MemoryStream::~MemoryStream()
{
	if (_Buffer != NULL)
		Close();
}

size_t MemoryStream::Write(void *buffer, size_t offset, size_t size)
{
	if (_BufferPosition + size > _BufferSize)
	{
		_BufferSize = _BufferPosition + size;
		
		if (_BufferSize > _BufferMaxSize)
		{
			_BufferMaxSize = (_BufferSize / _ExpansionFactor + 1) * _ExpansionFactor;
			_Buffer = (unsigned char *)realloc(_Buffer, _BufferMaxSize);
		}
	}

	memcpy(&_Buffer[_BufferPosition], (unsigned char *)buffer + offset, size);
	_BufferPosition += size;

	return size;
}

size_t MemoryStream::Read(void *buffer, size_t offset, size_t size)
{
	int read = size;
	
	if (_BufferPosition + size > _BufferSize)
	{
		read = _BufferSize - _BufferPosition;
	}
	
	memcpy((unsigned char *)buffer + offset, &_Buffer[_BufferPosition], read);

	return read;
}

bool MemoryStream::Seek(unsigned int origin, size_t offset)
{
	size_t destOffset = 0;

	if (origin == 0)
		destOffset = offset;
	else if (origin == 1)
		destOffset = _BufferPosition + offset;
	else if (origin == 2)
		destOffset = _BufferSize + offset;

	if (destOffset >= _BufferSize)
		return false;

	_BufferPosition = destOffset;

	return true;
}

void MemoryStream::SetExpansionFactor(size_t size)
{
	_BufferMaxSize = size;
}

void *MemoryStream::Clone()
{
	void *tmp;

	if (_Buffer)
		return NULL;

	tmp = malloc(_BufferSize);
	memcpy(tmp, _Buffer, _BufferSize);

	return tmp;
}

void MemoryStream::Close()
{
	if (_Buffer != NULL)
	{
		free(_Buffer);
		
		_Buffer = NULL;
		_BufferPosition = 0;
		_BufferSize = 0;
		_BufferMaxSize = 0;
	}
}
