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
