#include <adreno/utils/hashtable.h>
#include <adreno/memory.h>

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#if HEAP_DEBUG > 1
#define ADJUST_POINTER(x, type, delta) \
	if (x) { unsigned int oldX = (unsigned int)x; \
		x = (type *)((unsigned int)x + (unsigned int)delta); printf("(%x -> %x) ", oldX, x); }
#else
#define ADJUST_POINTER(x, type, delta) \
	if (x) \
		x = (type *)((unsigned int)x + (unsigned int)delta)
#endif

#define CREALLOC(var, type, count) \
	var = (type *)AdrenoRealloc(var, (count) * sizeof(type))

AdrenoHashtableNode *hashtable_find(AdrenoHashtable *hashtable, unsigned int keyHash)
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

int AdrenoHashtable_HeapAdjust(AdrenoHashtable *hashtable)
{
	unsigned int i, delta = 0;
	AdrenoHashtableNode *node, *oldBase = hashtable->NodeHeap;

	if (hashtable->FreeSlots <= 0)
		hashtable->FreeSlots = hashtable->ExpansionFactor;

	CREALLOC(hashtable->NodeHeap, AdrenoHashtableNode, hashtable->NodeCount + hashtable->FreeSlots);

	if (!hashtable->NodeHeap)
		return 0;

	if (oldBase != NULL)
	{
		delta = (unsigned int)((unsigned int)hashtable->NodeHeap - (unsigned int)oldBase);

#if HEAP_DEBUG > 1
		printf("Relocating node heap: %x => %x(%d bytes). root = %x\n", oldBase, hashtable->NodeHeap, delta, hashtable->RootNode);
#endif

		if (delta != 0)
		{
#if HEAP_DEBUG > 1
			printf("Root Node: ");
#endif
			ADJUST_POINTER(hashtable->RootNode, AdrenoHashtableNode, delta);
#if HEAP_DEBUG > 1
			printf("\n");
#endif

			for (i = 0; i < hashtable->NodeCount; i++)
			{
				node = (AdrenoHashtableNode *)&hashtable->NodeHeap[i];

#if HEAP_DEBUG > 1
				printf("Node %d(%x): ", i, node);
#endif
				ADJUST_POINTER(node->Left, AdrenoHashtableNode, delta);
				ADJUST_POINTER(node->Right, AdrenoHashtableNode, delta);
				ADJUST_POINTER(node->Parent, AdrenoHashtableNode, delta);
#if HEAP_DEBUG > 1
				printf("\n");
#endif
			}
		}
	}

	return delta;
}

AdrenoHashtableNode *AdrenoHashtable_HeapAlloc(AdrenoHashtable *hashtable, unsigned int *heapMove)
{
	if (hashtable->FreeSlots <= 0)
	{
		unsigned int tmp = AdrenoHashtable_HeapAdjust(hashtable);

		if (heapMove)
			*heapMove = tmp;
	}

	hashtable->FreeSlots--;

	return &hashtable->NodeHeap[hashtable->NodeCount++];
}

void AdrenoHashtable_HeapFree(AdrenoHashtable *hashtable, AdrenoHashtableNode *ptr)
{
	hashtable->NodeCount--;

	if (hashtable->NodeCount <= 0)
	{
		AdrenoHashtable_Clear(hashtable);
	}
	else
	{
		memcpy(ptr, &hashtable->NodeHeap[hashtable->NodeCount - 1], sizeof(AdrenoHashtableNode));

		if (ptr->Right)
			ptr->Right->Parent = ptr;

		if (ptr->Left)
			ptr->Left->Parent = ptr;

		if (ptr->Value.Key > ptr->Parent->Value.Key)
			ptr->Parent->Right = ptr;
		else if (ptr->Value.Key < ptr->Parent->Value.Key)
			ptr->Parent->Left = ptr;

		hashtable->FreeSlots++;
		if (hashtable->FreeSlots >= hashtable->ExpansionFactor * 2)
		{
			hashtable->FreeSlots = hashtable->ExpansionFactor;
			AdrenoHashtable_HeapAdjust(hashtable);
		}
	}
}

void AdrenoHashtable_Initialize(AdrenoHashtable *hashtable, AdrenoHashtable_HashFunction hash, AdrenoHashtable_LenFunction len)
{
	if (!hashtable)
		return;

	hashtable->NodeCount = 0;
	hashtable->NodeHeap = NULL;
	hashtable->RootNode = NULL;

	hashtable->Hash = hash;
	hashtable->Len = len;

	hashtable->FreeSlots = 0;
	hashtable->ExpansionFactor = DEFAULT_EXPANSION_FACTOR;

#if HEAP_DEBUG > 0
	hashtable->ReallocCount = 0;
#endif
}

void AdrenoHashtable_Destroy(AdrenoHashtable *hashtable)
{
	AdrenoHashtable_Clear(hashtable);
}

void AdrenoHashtable_Set(AdrenoHashtable *hashtable, void *key, void *value)
{
	unsigned int keyHash = hashtable->Hash ? hashtable->Hash(key, hashtable->Len(key)) : (unsigned int)key;
	AdrenoHashtableNode *next = hashtable->RootNode;
	AdrenoHashtableNode *ptr;

#define CREATE_NODE(node, parentNode) \
	{ \
		unsigned int delta = 0; \
		node = AdrenoHashtable_HeapAlloc(hashtable, &delta); \
		if (!node) \
			return; \
		ADJUST_POINTER(next, AdrenoHashtableNode, delta); \
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
					CREATE_NODE(ptr, NULL);
					next->Left = ptr;
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
					CREATE_NODE(ptr, NULL);
					next->Right = ptr;
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
	unsigned int keyHash = hashtable->Hash ? hashtable->Hash(key, hashtable->Len(key)) : (unsigned int)key;
	AdrenoHashtableNode *ptr;

	if ((ptr = hashtable_find(hashtable, keyHash)) != NULL)
	{
		if (value)
			*value = ptr->Value.Value;

		return 1;
	}

	return 0;
}

KeyValuePair hashtable_minKeyHash(AdrenoHashtableNode *node)
{
	KeyValuePair *value;

	do
	{
		value = &node->Value;
		node = node->Left;
	}
	while (node != NULL);

	return *value;
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
			node->Value = hashtable_minKeyHash(node->Right);

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

void AdrenoHashtable_Remove(AdrenoHashtable *hashtable, void *key)
{
	unsigned int keyHash = hashtable->Hash ? hashtable->Hash(key, hashtable->Len(key)) : (unsigned int)key;

	if (hashtable->RootNode)
		return;

	if (hashtable->RootNode->Value.Key == keyHash)
	{
		AdrenoHashtableNode auxRoot;
		AdrenoHashtableNode *removedNode;

		auxRoot.Left = hashtable->RootNode;

		removedNode = AdrenoHashtable_TreeRemove(hashtable->RootNode, keyHash, &auxRoot);
		hashtable->RootNode = auxRoot.Left;

		if (removedNode != NULL)
			AdrenoHashtable_HeapFree(hashtable, removedNode);
	}
	else
	{
		AdrenoHashtableNode *removedNode = AdrenoHashtable_TreeRemove(hashtable->RootNode, keyHash, NULL);

		if (removedNode != NULL)
			AdrenoHashtable_HeapFree(hashtable, removedNode);
	}
}

int AdrenoHashtable_Count(AdrenoHashtable *hashtable)
{
	return hashtable->NodeCount;
}

void AdrenoHashtable_Clear(AdrenoHashtable *hashtable)
{
	if (hashtable->RootNode)
	{
		AdrenoFree(hashtable->NodeHeap);
		hashtable->NodeHeap = NULL;
	}

	hashtable->NodeCount = 0;
	hashtable->FreeSlots = 0;
}

unsigned int AdrenoHashtable_Hash_Fnv(void *key, unsigned int size)
{
	unsigned int hval = 0x811c9dc5;
	unsigned char *bp = (unsigned char *)key;
	unsigned char *be = bp + size;

	while (bp < be)
	{
		hval ^= (unsigned int) * bp++;
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
	}

	return (int)hval;
}

unsigned int AdrenoHashtable_Len_String(void *key)
{
	char *str = (char *)key;
	int size = 0;

	while (str[size++]) ;

	return size;
}

unsigned int AdrenoHashtable_Len_WString(void *key)
{
	wchar_t *str = (wchar_t *)key;
	int size = 0;

	while (str[size++]) ;

	return size * sizeof(wchar_t);
}
