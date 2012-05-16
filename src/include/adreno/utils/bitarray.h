#ifndef ADRENOBITARRAY_H
#define ADRENOBITARRAY_H

#define USE_ANCHOR_OFFSET

typedef struct
{
	unsigned int *Data;
	unsigned int DataCount;
	unsigned int AnchorIndex;
#ifdef USE_ANCHOR_OFFSET
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
