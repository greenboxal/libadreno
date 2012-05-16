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
