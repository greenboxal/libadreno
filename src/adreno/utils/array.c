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

#include <adreno/utils/array.h>
#include <adreno/memory.h>

#include <string.h>

void AdrenoArray_Initialize(AdrenoArray *arr)
{
	arr->Data = NULL;
	arr->Count = 0;
}

unsigned int AdrenoArray_Add(AdrenoArray *arr, void *e)
{
	int idx = arr->Count++;

	arr->Data = (void **)AdrenoRealloc(arr->Data, arr->Count * sizeof(void *));
	arr->Data[idx] = e;

	return idx;
}

void AdrenoArray_Remove(AdrenoArray *arr, unsigned int idx)
{
	if (idx >= arr->Count)
		return;

	memcpy(&arr->Data[idx], &arr->Data[idx + 1], (--arr->Count - idx) * sizeof(void *));
	
	arr->Data = (void **)AdrenoRealloc(arr->Data, arr->Count * sizeof(void *));
}

unsigned int AdrenoArray_IndexOf(AdrenoArray *arr, void *e)
{
	unsigned int i = 0;

	for (i = 0; i < arr->Count; i++)
	{
		if (arr->Data[i] == e)
			return i;
	}

	return (unsigned int)-1;
}

void AdrenoArray_Free(AdrenoArray *arr)
{
	if (arr->Data)
		AdrenoFree(arr->Data);

	arr->Count = 0;
}
