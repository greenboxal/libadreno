#include "AdrenoVM.h"

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
	if (stack->StackPointer - count < 0 && !canExpand)
	{
		return 0;
	}
	else if (stack->StackPointer - count < 0 && canExpand)
	{
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
