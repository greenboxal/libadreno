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

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef uint32_t hash_t;

	typedef hash_t (*AdrenoHashtable_HashFunction)(void *key, size_t size);
	typedef size_t (*AdrenoHashtable_LenFunction)(void *key);

	typedef struct keyvaluepair KeyValuePair;
	struct keyvaluepair
	{
		hash_t Key;
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
		/* Nodes */
		size_t NodeCount;
		AdrenoHashtableNode *RootNode;

		/* Functions */
		AdrenoHashtable_HashFunction Hash;
		AdrenoHashtable_LenFunction Len;

		/* Expansion settings */
		size_t FreeSlots;
		size_t ExpansionFactor;
	} AdrenoHashtable;

	typedef struct
	{
		AdrenoHashtable *Hashtable;
		AdrenoHashtableNode *CurrentNode;
	} AdrenoHashtableIterator;

	/* Hashtable functions */
	extern void AdrenoHashtable_Initialize(AdrenoHashtable *hashtable, AdrenoHashtable_HashFunction hash, AdrenoHashtable_LenFunction len);
	extern void AdrenoHashtable_Destroy(AdrenoHashtable *hashtable);

	extern void AdrenoHashtable_Set(AdrenoHashtable *hashtable, void *key, void *value);
	extern int AdrenoHashtable_Get(AdrenoHashtable *hashtable, void *key, void **value);
	extern void AdrenoHashtable_Remove(AdrenoHashtable *hashtable, void *key);

	extern AdrenoHashtableIterator *AdrenoHashtable_CreateIterator(AdrenoHashtable *hashtable);
	extern void AdrenoHashtableIterator_Next(AdrenoHashtableIterator *it);
	extern void AdrenoHashtableIterator_Free(AdrenoHashtableIterator *it);

	extern int AdrenoHashtable_Count(AdrenoHashtable *hashtable);
	extern void AdrenoHashtable_Clear(AdrenoHashtable *hashtable);

	/* General hash functions */
	extern hash_t AdrenoHashtable_Hash_Fnv(void *key, size_t size);

	/* General len functions */
	extern size_t AdrenoHashtable_Len_String(void *key);
	extern size_t AdrenoHashtable_Len_String(void *key);

#ifdef __cplusplus
}
#endif

#endif
