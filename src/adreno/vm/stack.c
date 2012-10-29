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

#include <adreno/vm/vm.h>

#include <memory.h>

void AdrenoStack_Initialize(AdrenoStack *stack, int initialSize)
{
	stack->Stack = (AdrenoValue *)AdrenoAlloc(sizeof(AdrenoValue) * initialSize);
	stack->StackPointer = initialSize;
	stack->StackSize = initialSize;

	memset(stack->Stack, 0, sizeof(AdrenoValue) * initialSize);
}

void AdrenoStack_Free(AdrenoStack *stack)
{
	if (stack->Stack)
	{
		AdrenoStack_Clear(stack);
		AdrenoFree(stack->Stack);
	}

	stack->Stack = NULL;
	stack->StackPointer = 0;
	stack->StackSize = 0;
}

int AdrenoStack_Push(AdrenoStack *stack, AdrenoValue *value, int canExpand)
{
	if (stack->StackPointer <= 0 && !canExpand)
	{
		return 0;
	}
	else if (stack->StackPointer <= 0 && canExpand)
	{
		int oldSize = stack->StackSize;
		
		stack->StackSize += ADRENOSTACK_EXPANSION_FACTOR;
		stack->Stack = (AdrenoValue *)AdrenoRealloc(stack->Stack, sizeof(AdrenoValue) * stack->StackSize);
		
		memset(&stack->Stack[oldSize], 0, sizeof(AdrenoValue) * stack->StackSize - oldSize);
	}

	stack->Stack[--stack->StackPointer] = *value;

	return 1;
}

int AdrenoStack_Take(AdrenoStack *stack, AdrenoValue **value, int count, int canExpand)
{
	if( (size_t) count > stack->StackPointer )
	{
		if( !canExpand )
		{
			return 0;
		}

		int oldSize = stack->StackSize;
		
		stack->StackSize += ((count + ADRENOSTACK_EXPANSION_FACTOR - 1) / ADRENOSTACK_EXPANSION_FACTOR) * ADRENOSTACK_EXPANSION_FACTOR;
		stack->Stack = (AdrenoValue *)AdrenoRealloc(stack->Stack, sizeof(AdrenoValue) * stack->StackSize);
		
		memset(&stack->Stack[oldSize], 0, sizeof(AdrenoValue) * stack->StackSize - oldSize);
	}

	stack->StackPointer -= count;
	*value = &stack->Stack[stack->StackPointer];

	return 1;
}

int AdrenoStack_Pop(AdrenoStack *stack, AdrenoValue *value)
{
	if (stack->StackPointer >= stack->StackSize)
		return 0;

	*value = stack->Stack[stack->StackPointer++];

	return 1;
}

void AdrenoStack_Clear(AdrenoStack *stack)
{
	AdrenoValue value;

	while (AdrenoStack_Pop(stack, &value))
		AdrenoValue_Dereference(&value);
}
