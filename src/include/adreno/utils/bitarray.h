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

#ifndef ADRENOBITARRAY_H
#define ADRENOBITARRAY_H

#include <adreno/config.h>

typedef struct
{
	unsigned int *Data;
	unsigned int DataCount;
	unsigned int AnchorIndex;
#ifdef BITARRAY_USE_ANCHOR_OFFSET
	unsigned int AnchorOffset;
#endif
} AdrenoBitArray;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoBitArray_Initialize(AdrenoBitArray *ba, unsigned int initialSize);
	extern void AdrenoBitArray_Set(AdrenoBitArray *ba, unsigned int idx);
	extern void AdrenoBitArray_Unset(AdrenoBitArray *ba, unsigned int idx);
	extern int AdrenoBitArray_Get(AdrenoBitArray *ba, unsigned int idx);
	extern int AdrenoBitArray_Search(AdrenoBitArray *ba);
	extern void AdrenoBitArray_Clear(AdrenoBitArray *ba);
	extern void AdrenoBitArray_Resize(AdrenoBitArray *ba, unsigned int newSize);
	extern void AdrenoBitArray_Free(AdrenoBitArray *ba);

#ifdef __cplusplus
}
#endif

#endif
