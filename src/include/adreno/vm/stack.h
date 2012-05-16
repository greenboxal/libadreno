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

#ifndef ADRENOSTACK_H
#define ADRENOSTACK_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

struct adrenostack
{
	AdrenoValue *Stack;
	unsigned int StackPointer, StackSize;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoStack_Initialize(AdrenoStack *stack, int initialSize);
	extern void AdrenoStack_Free(AdrenoStack *stack);
	extern int AdrenoStack_Push(AdrenoStack *stack, AdrenoValue *value, int canExpand);
	extern int AdrenoStack_Take(AdrenoStack *stack, AdrenoValue **value, int count, int canExpand);
	extern int AdrenoStack_Pop(AdrenoStack *stack, AdrenoValue *value);
	extern void AdrenoStack_Clear(AdrenoStack *stack);

#ifdef __cplusplus
}
#endif

#endif
