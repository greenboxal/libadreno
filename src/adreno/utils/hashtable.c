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

#include <adreno/utils/hashtable.h>
#include <adreno/utils/memorypool.h>
#include <adreno/config.h>
#include <adreno/memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AdrenoMemoryPool *AHT_NodePool = NULL;
static unsigned HT_UseCount = 0;

static hash_t GetHash( AdrenoHashtable *hashtable, void *key )
{
	return hashtable->Hash
		? hashtable->Hash( key, hashtable->Len( key ))
		: (hash_t) (size_t) key;
}

AdrenoHashtableNode *AdrenoHashtable_Find(AdrenoHashtable *hashtable, unsigned int keyHash)
{
	AdrenoHashtableNode *next = hashtable->RootNode;

	while (next != NULL)
	{
		if (keyHash == next->Value.Key)
			return next;
		else if (keyHash < next->Value.Key)
			next = next->Left;
		else if (keyHash > next->Value.Key)
			next = next->Right;
	}

	return NULL;
}

void AdrenoHashtable_Initialize(AdrenoHashtable *hashtable, AdrenoHashtable_HashFunction hash, AdrenoHashtable_LenFunction len)
{
	HT_UseCount++;

	if (!AHT_NodePool)
		AHT_NodePool = AdrenoMemoryPool_New(sizeof(AdrenoHashtableNode), 1);

	if (!hashtable)
		return;

	hashtable->NodeCount = 0;
	hashtable->RootNode = NULL;

	hashtable->Hash = hash;
	hashtable->Len = len;

	hashtable->FreeSlots = 0;
	hashtable->ExpansionFactor = ADRENOHT_DEFAULT_EXPANSION_FACTOR;
}

void AdrenoHashtable_Destroy(AdrenoHashtable *hashtable)
{
	AdrenoHashtable_Clear(hashtable);

	HT_UseCount--;
	if (HT_UseCount <= 0)
	{
		AdrenoMemoryPool_Destroy(AHT_NodePool);
		AHT_NodePool = NULL;
	}
}

void AdrenoHashtable_Set(AdrenoHashtable *hashtable, void *key, void *value)
{
	hash_t keyHash = GetHash( hashtable, key );

	AdrenoHashtableNode *next = hashtable->RootNode;
	AdrenoHashtableNode *ptr;

#define CREATE_NODE(node, parentNode) \
	{ \
		node = (AdrenoHashtableNode *)AdrenoMemoryPool_Alloc(AHT_NodePool); \
		if (!node) \
			return; \
		node->Value.Key = keyHash; \
		node->Value.Value = value; \
		node->Parent = parentNode; \
		node->Left = NULL; \
		node->Right = NULL; \
	}

	if (next == NULL)
	{
		CREATE_NODE(hashtable->RootNode, NULL);
	}
	else
	{
		while (next != NULL)
		{
			if (keyHash == next->Value.Key)
			{
				next->Value.Value = value;
				break;
			}
			else if (keyHash < next->Value.Key)
			{
				if (next->Left == NULL)
				{
					CREATE_NODE(ptr, next);
					next->Left = ptr;
					hashtable->NodeCount++;
					break;
				}
				else
				{
					next = next->Left;
				}
			}
			else if (keyHash > next->Value.Key)
			{
				if (next->Right == NULL)
				{
					CREATE_NODE(ptr, next);
					next->Right = ptr;
					hashtable->NodeCount++;
					break;
				}
				else
				{
					next = next->Right;
				}
			}
		}
	}

#undef CREATE_NODE
}

int AdrenoHashtable_Get(AdrenoHashtable *hashtable, void *key, void **value)
{
	hash_t keyHash = GetHash( hashtable, key );
	AdrenoHashtableNode *ptr;

	if ((ptr = AdrenoHashtable_Find(hashtable, keyHash)) != NULL)
	{
		if (value)
			*value = ptr->Value.Value;

		return 1;
	}

	return 0;
}

AdrenoHashtableNode *Hashtable_GetLeftMost(AdrenoHashtableNode *node)
{
	while (node->Left != NULL)
		node = node->Left;

	return node;
}

AdrenoHashtableNode *AdrenoHashtable_TreeRemove(AdrenoHashtableNode *node, unsigned int keyHash, AdrenoHashtableNode *parent)
{
	if (keyHash < node->Value.Key)
	{
		if (node->Left != NULL)
			return AdrenoHashtable_TreeRemove(node->Left, keyHash, node);
		else
			return NULL;
	}
	else if (keyHash > node->Value.Key)
	{
		if (node->Right != NULL)
			return AdrenoHashtable_TreeRemove(node->Right, keyHash, node);
		else
			return NULL;
	}
	else
	{
		if (node->Left != NULL && node->Right != NULL)
		{
			node->Value = Hashtable_GetLeftMost(node->Right)->Value;

			return AdrenoHashtable_TreeRemove(node->Right, node->Value.Key, node);
		}
		else if (parent->Left == node)
		{
			parent->Left = (node->Left != NULL) ? node->Left : node->Right;

			return node;
		}
		else if (parent->Right == node)
		{
			parent->Right = (node->Left != NULL) ? node->Left : node->Right;

			return node;
		}
	}

	return NULL;
}

void AdrenoHashtable_RemoveHashed(AdrenoHashtable *hashtable, unsigned int keyHash)
{
	if (!hashtable->RootNode)
		return;

	if (hashtable->RootNode->Value.Key == keyHash)
	{
		AdrenoHashtableNode auxRoot;
		AdrenoHashtableNode *removedNode;

		auxRoot.Left = hashtable->RootNode;

		removedNode = AdrenoHashtable_TreeRemove(hashtable->RootNode, keyHash, &auxRoot);
		hashtable->RootNode = auxRoot.Left;

		if (hashtable->RootNode)
			hashtable->RootNode->Parent = NULL;
		
		hashtable->NodeCount--;
		if (removedNode != NULL)
			AdrenoMemoryPool_Free(AHT_NodePool, removedNode);
	}
	else
	{
		AdrenoHashtableNode *removedNode = AdrenoHashtable_TreeRemove(hashtable->RootNode, keyHash, NULL);
		
		hashtable->NodeCount--;
		if (removedNode != NULL)
			AdrenoMemoryPool_Free(AHT_NodePool, removedNode);
	}
}

void AdrenoHashtable_Remove(AdrenoHashtable *hashtable, void *key)
{
	hash_t keyHash = GetHash( hashtable, key );

	AdrenoHashtable_RemoveHashed(hashtable, keyHash);
}

int AdrenoHashtable_Count(AdrenoHashtable *hashtable)
{
	return hashtable->NodeCount;
}

void AdrenoHashtable_Clear(AdrenoHashtable *hashtable)
{
	while (hashtable->RootNode)
		AdrenoHashtable_RemoveHashed(hashtable, hashtable->RootNode->Value.Key);

	hashtable->NodeCount = 0;
	hashtable->FreeSlots = 0;
}

AdrenoHashtableIterator *AdrenoHashtable_CreateIterator(AdrenoHashtable *hashtable)
{
	AdrenoHashtableIterator *it = (AdrenoHashtableIterator *)AdrenoAlloc(sizeof(AdrenoHashtableIterator));

	it->Hashtable = hashtable;
	it->CurrentNode = Hashtable_GetLeftMost(hashtable->RootNode);

	return it;
}

void AdrenoHashtableIterator_Next(AdrenoHashtableIterator *it)
{
	if (it->CurrentNode->Right != NULL) 
	{
		it->CurrentNode = Hashtable_GetLeftMost(it->CurrentNode->Right);
    } 
	else 
	{
        AdrenoHashtableNode *n = it->CurrentNode;

        while (n->Parent != NULL && n == n->Parent->Right) 
		{
            n = n->Parent;
        }

        it->CurrentNode = n->Parent;
    }
}

void AdrenoHashtableIterator_Free(AdrenoHashtableIterator *it)
{
	AdrenoFree(it);
}

hash_t AdrenoHashtable_Hash_Fnv(void *key, size_t size)
{
	hash_t hval = 0x811c9dc5;
	unsigned char *bp = (unsigned char *)key;
	unsigned char *be = bp + size;

	while (bp < be)
	{
		hval ^= (hash_t) * bp++;
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
	}

	return hval;
}

size_t AdrenoHashtable_Len_String(void *key)
{
	// strlen() is better than counting as it will use SSE instructions on Core 2+
	return strlen( (const char*) key );
}

unsigned int AdrenoHashtable_Len_WString(void *key)
{
	wchar_t *str = (wchar_t *)key;
	int size = 0;

	while (str[size++]) ;

	return size * sizeof(wchar_t);
}
