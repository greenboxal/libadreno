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
