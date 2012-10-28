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

#include <adreno/utils/bitarray.h>
#include <adreno/memory.h>

#include <memory.h>

#define BAINDEX(b) (b / 32)
#define BAOFFSET(b) (b % 32)

void AdrenoBitArray_Initialize(AdrenoBitArray *ba, unsigned int initialSize)
{
	ba->DataCount = 0;
	ba->Data = NULL;
	ba->AnchorIndex = 0;
#ifdef BITARRAY_USE_ANCHOR_OFFSET
	ba->AnchorOffset = 0;
#endif

	if (initialSize > 0)
		AdrenoBitArray_Resize(ba, initialSize);
}

void AdrenoBitArray_Set(AdrenoBitArray *ba, unsigned int idx)
{
	unsigned int index = BAINDEX(idx);
	unsigned int offset = BAOFFSET(idx);

	if (index >= ba->DataCount)
		return;

	ba->Data[index] |= 1 << offset;

	if (index == ba->AnchorIndex && ba->Data[index] == 0xFFFFFFFF)
	{
		ba->AnchorIndex++;
#ifdef BITARRAY_USE_ANCHOR_OFFSET
		ba->AnchorOffset = 0;
#endif
	}
}

void AdrenoBitArray_Unset(AdrenoBitArray *ba, unsigned int idx)
{
	unsigned int index = BAINDEX(idx);
	unsigned int offset = BAOFFSET(idx);

	if (index >= ba->DataCount)
		return;

	ba->Data[index] &= ~(1 << offset);

	if (index <= ba->AnchorIndex)
	{
		ba->AnchorIndex = index;
		
#ifdef BITARRAY_USE_ANCHOR_OFFSET
		if (offset < ba->AnchorOffset)
			ba->AnchorOffset = offset;
#endif
	}
}

int AdrenoBitArray_Get(AdrenoBitArray *ba, unsigned int idx)
{
	unsigned int index = BAINDEX(idx);
	unsigned int offset = BAOFFSET(idx);

	if (index >= ba->DataCount)
		return -1;

	return ba->Data[index] & (1 << offset);
}

int AdrenoBitArray_Search(AdrenoBitArray *ba)
{
	unsigned int i, j, s;
	
#ifdef BITARRAY_USE_ANCHOR_OFFSET
	s = ba->AnchorOffset;
#else
	s = 0;
#endif

	for (i = ba->AnchorIndex; i < ba->DataCount; i++)
	{
		if (ba->Data[i] == 0xFFFFFFFF)
		{
#ifdef BITARRAY_USE_ANCHOR_OFFSET
			s = 0;
#endif
			continue;
		}

		for (j = s; j < 32; j++)
		{
			int mask = 1 << j;

			if (!(ba->Data[i] & mask))
			{
				ba->AnchorIndex = i;
#ifdef BITARRAY_USE_ANCHOR_OFFSET
				ba->AnchorOffset = j;
#endif

				return i * 32 + j;
			}
		}
		
#ifdef BITARRAY_USE_ANCHOR_OFFSET
		s = 0;
#endif
	}

	return -1;
}

void AdrenoBitArray_Clear(AdrenoBitArray *ba)
{
	memset(ba->Data, 0, ba->DataCount * sizeof(unsigned int));
}

void AdrenoBitArray_Resize(AdrenoBitArray *ba, unsigned int newSize)
{
	int oldCount = ba->DataCount;
	int newCount;

	if (newSize % 32 > 0)
		newSize += newSize - (newSize % 32);

	newCount = newSize / 32;

	ba->DataCount = newCount;
	ba->Data = (unsigned int *)AdrenoRealloc(ba->Data, ba->DataCount * sizeof(unsigned int));

	if (newCount > oldCount)
		memset(&ba->Data[oldCount], 0, (newCount - oldCount) * sizeof(unsigned int));
}

void AdrenoBitArray_Free(AdrenoBitArray *ba)
{
	if (ba->Data)
		AdrenoFree(ba->Data);
	
	ba->DataCount = 0;
	ba->Data = NULL;
	ba->AnchorIndex = 0;
#ifdef BITARRAY_USE_ANCHOR_OFFSET
	ba->AnchorOffset = 0;
#endif
}
