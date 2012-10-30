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

#ifndef ADRENOMS_H
#define ADRENOMS_H

#include <stddef.h> // size_t

namespace Adreno
{
	class MemoryStream
	{
	public:
		MemoryStream();
		MemoryStream(const MemoryStream &ms);
		~MemoryStream();

		size_t Write(void *buffer, size_t offset, size_t size);
		size_t Read(void *buffer, size_t offset, size_t size);
		bool Seek(unsigned int origin, size_t offset);
		void Close();

		void *Clone();
		void SetExpansionFactor(size_t size);

	private:
		unsigned char *_Buffer;
		size_t _BufferPosition, _BufferSize, _BufferMaxSize;
		size_t _ExpansionFactor;
	};
}

#endif
