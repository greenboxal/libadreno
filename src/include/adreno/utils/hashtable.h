#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HEAP_DEBUG 0

#define USE_EXPANSION_FACTOR 1
#define DEFAULT_EXPANSION_FACTOR 100

#ifdef __cplusplus
extern "C"
{
#endif

	typedef unsigned int (*AdrenoHashtable_HashFunction)(void *key, unsigned int size);
	typedef unsigned int (*AdrenoHashtable_LenFunction)(void *key);

	typedef struct keyvaluepair KeyValuePair;
	struct keyvaluepair
	{
		unsigned int Key;
		void *Value;
	};

	typedef struct hashtablenode AdrenoHashtableNode;
	struct hashtablenode
	{
		KeyValuePair Value;

		AdrenoHashtableNode *Parent;
		AdrenoHashtableNode *Left;
		AdrenoHashtableNode *Right;
	};

	typedef struct
	{
		// Nodes
		unsigned int NodeCount;
		AdrenoHashtableNode *NodeHeap;
		AdrenoHashtableNode *RootNode;

		// Functions
		AdrenoHashtable_HashFunction Hash;
		AdrenoHashtable_LenFunction Len;

		// Expansion settings
		int FreeSlots;
		int ExpansionFactor;

#if HEAP_DEBUG > 0
		// Debug stuff
		int ReallocCount;
#endif
	} AdrenoHashtable;

	// Hashtable functions
	extern void AdrenoHashtable_Initialize(AdrenoHashtable *hashtable, AdrenoHashtable_HashFunction hash, AdrenoHashtable_LenFunction len);
	extern void AdrenoHashtable_Destroy(AdrenoHashtable *hashtable);

	extern void AdrenoHashtable_Set(AdrenoHashtable *hashtable, void *key, void *value);
	extern int AdrenoHashtable_Get(AdrenoHashtable *hashtable, void *key, void **value);
	extern void AdrenoHashtable_Remove(AdrenoHashtable *hashtable, void *key);

	extern int AdrenoHashtable_Count(AdrenoHashtable *hashtable);
	extern void AdrenoHashtable_Clear(AdrenoHashtable *hashtable);

	// General hash functions
	extern unsigned int AdrenoHashtable_Hash_Fnv(void *key, unsigned int size);

	// General len functions
	extern unsigned int AdrenoHashtable_Len_String(void *key);
	extern unsigned int AdrenoHashtable_Len_WString(void *key);

#ifdef __cplusplus
}
#endif

#endif
