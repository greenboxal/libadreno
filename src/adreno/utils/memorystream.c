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
#include <adreno/memory.h>

void AdrenoMS_Open(AdrenoMS *ms)
{
	ms->buffer = NULL;
	ms->bufferPosition = 0;
	ms->bufferSize = 0;
	ms->bufferMaxSize = 0;
	ms->expansionFactor = 100;
}

int AdrenoMS_Write(AdrenoMS *ms, unsigned char *buffer, unsigned int offset, unsigned int size)
{
	if (ms->bufferPosition + size > ms->bufferSize)
	{
		ms->bufferSize = ms->bufferPosition + size;
		
		if (ms->bufferSize > ms->bufferMaxSize)
		{
			ms->bufferMaxSize = (ms->bufferSize / ms->expansionFactor + 1) * ms->expansionFactor;
			ms->buffer = (unsigned char *)AdrenoRealloc(ms->buffer, ms->bufferMaxSize);
		}
	}

	memcpy(&ms->buffer[ms->bufferPosition], &buffer[offset], size);
	ms->bufferPosition += size;

	return size;
}

int AdrenoMS_Read(AdrenoMS *ms, unsigned char *buffer, unsigned int offset, unsigned int size)
{
	int read = size;
	
	if (ms->bufferPosition + size > ms->bufferSize)
	{
		read = ms->bufferSize - ms->bufferPosition;
	}
	
	memcpy(&buffer[offset], &ms->buffer[ms->bufferPosition], read);

	return read;
}

int AdrenoMS_Seek(AdrenoMS *ms, unsigned int origin, unsigned int offset)
{
	unsigned int destOffset = 0;

	if (origin == 0)
		destOffset = offset;
	else if (origin == 1)
		destOffset = ms->bufferPosition + offset;
	else if (origin == 2)
		destOffset = ms->bufferSize + offset;

	if (destOffset < 0 || destOffset >= ms->bufferSize)
		return 0;

	ms->bufferPosition = destOffset;

	return 1;
}

unsigned char *AdrenoMS_Clone(AdrenoMS *ms)
{
	unsigned char *tmp;

	if (!ms->buffer)
		return NULL;

	tmp = (unsigned char *)AdrenoAlloc(ms->bufferSize);
	memcpy(tmp, ms->buffer, ms->bufferSize);

	return tmp;
}

void AdrenoMS_Close(AdrenoMS *ms)
{
	if (ms->buffer)
		AdrenoFree(ms->buffer);

	ms->buffer = NULL;
	ms->bufferPosition = 0;
	ms->bufferSize = 0;
	ms->bufferMaxSize = 0;
}
