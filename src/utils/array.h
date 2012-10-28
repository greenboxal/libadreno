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

#ifndef ADRENOARRAY_H
#define ADRENOARRAY_H

typedef struct
{
	void **Data;
	unsigned int Count;
} AdrenoArray;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoArray_Initialize(AdrenoArray *arr);
	extern unsigned int AdrenoArray_Add(AdrenoArray *arr, void *e);
	extern void AdrenoArray_Remove(AdrenoArray *arr, unsigned int idx);
	extern unsigned int AdrenoArray_IndexOf(AdrenoArray *arr, void *e);
	extern void AdrenoArray_Free(AdrenoArray *arr);

#ifdef __cplusplus
}
#endif

#endif
