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

#include <adreno/memory.h>

// This memory manager was taken from rAthena Ragnarφk Online emulator, all credits goes to the original creators
// Some modifications by libadreno team

#if defined(USE_MEMORY_MANAGER)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

////////////// Memory Libraries //////////////////

#if defined(MEMWATCH)

#	include <string.h> 
#	include "memwatch.h"
#	define MALLOC(n,file,line,func)	mwMalloc((n),(file),(line))
#	define CALLOC(m,n,file,line,func)	mwCalloc((m),(n),(file),(line))
#	define REALLOC(p,n,file,line,func)	mwRealloc((p),(n),(file),(line))
#	define STRDUP(p,file,line,func)	mwStrdup((p),(file),(line))
#	define WCSDUP(p,file,line,func)	mwWcsdup(p)
#	define FREE(p,file,line,func)		mwFree((p),(file),(line))
#	define MEMORY_USAGE()	0
#	define MEMORY_VERIFY(ptr)	mwIsSafeAddr(ptr, 1)
#	define MEMORY_CHECK() CHECK()

#elif defined(DMALLOC)

#	include <string.h>
#	include <stdlib.h>
#	include "dmalloc.h"
#	define MALLOC(n,file,line,func)	dmalloc_malloc((file),(line),(n),DMALLOC_FUNC_MALLOC,0,0)
#	define CALLOC(m,n,file,line,func)	dmalloc_malloc((file),(line),(m)*(n),DMALLOC_FUNC_CALLOC,0,0)
#	define REALLOC(p,n,file,line,func)	dmalloc_realloc((file),(line),(p),(n),DMALLOC_FUNC_REALLOC,0)
#	define STRDUP(p,file,line,func)	strdup(p)
#	define WCSDUP(p,file,line,func)	wcsdup(p)
#	define FREE(p,file,line,func)		free(p)
#	define MEMORY_USAGE()	dmalloc_memory_allocated()
#	define MEMORY_VERIFY(ptr)	(dmalloc_verify(ptr) == DMALLOC_VERIFY_NOERROR)
#	define MEMORY_CHECK()	dmalloc_log_stats(); dmalloc_log_unfreed()

#elif defined(GCOLLECT)

#	include "gc.h"
#	ifdef GC_ADD_CALLER
#		define RETURN_ADDR 0,
#	else
#		define RETURN_ADDR
#	endif
#	define MALLOC(n,file,line,func)	GC_debug_malloc((n), RETURN_ADDR (file),(line))
#	define CALLOC(m,n,file,line,func)	GC_debug_malloc((m)*(n), RETURN_ADDR (file),(line))
#	define REALLOC(p,n,file,line,func)	GC_debug_realloc((p),(n), RETURN_ADDR (file),(line))
#	define STRDUP(p,file,line,func)	GC_debugstrdup((p), RETURN_ADDR (file),(line))
#	define WCSDUP(p,file,line,func)	GC_debugwcsdup((p), RETURN_ADDR (file),(line))
#	define FREE(p,file,line,func)		GC_debug_free(p)
#	define MEMORY_USAGE()	GC_get_heap_size()
#	define MEMORY_VERIFY(ptr)	(GC_base(ptr) != NULL)
#	define MEMORY_CHECK()	GC_gcollect()

#elif defined(USE_MALLOC)

#	define MALLOC(n,file,line,func)	malloc(n)
#	define CALLOC(m,n,file,line,func)	calloc((m),(n))
#	define REALLOC(p,n,file,line,func)	realloc((p),(n))
#	define STRDUP(p,file,line,func)	strdup(p)
#	define WCSDUP(p,file,line,func)	wcsdup(p)
#	define FREE(p,file,line,func)		free(p)
#	define MEMORY_USAGE()	0
#	define MEMORY_VERIFY(ptr)	1
#	define MEMORY_CHECK()

#endif

void* aMalloc_(unsigned int size, const char *file, int line, const char *func)
{
	void *ret = MALLOC(size, file, line, func);
	// ShowMessage("%s:%d: in func %s: aMalloc %d\n",file,line,func,size);
	if (ret == NULL){
		printf("%s:%d: in func %s: aMalloc error out of memory!\n",file,line,func);
		exit(EXIT_FAILURE);
	}

	return ret;
}
void* aCalloc_(unsigned int num, unsigned int size, const char *file, int line, const char *func)
{
	void *ret = CALLOC(num, size, file, line, func);
	// ShowMessage("%s:%d: in func %s: aCalloc %d %d\n",file,line,func,num,size);
	if (ret == NULL){
		printf("%s:%d: in func %s: aCalloc error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
void* aRealloc_(void *p, unsigned int size, const char *file, int line, const char *func)
{
	void *ret = REALLOC(p, size, file, line, func);
	// ShowMessage("%s:%d: in func %s: aRealloc %p %d\n",file,line,func,p,size);
	if (ret == NULL){
		printf("%s:%d: in func %s: aRealloc error out of memory!\n",file,line,func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
char* aStrdup_(const char *p, const char *file, int line, const char *func)
{
	char *ret = STRDUP(p, file, line, func);
	// ShowMessage("%s:%d: in func %s: aStrdup %p\n",file,line,func,p);
	if (ret == NULL){
		printf("%s:%d: in func %s: aStrdup error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
wchar_t* aWStrdup_(const wchar_t *p, const char *file, int line, const char *func)
{
	wchar_t *ret = WCSDUP(p, file, line, func);
	// ShowMessage("%s:%d: in func %s: aStrdup %p\n",file,line,func,p);
	if (ret == NULL){
		printf("%s:%d: in func %s: aStrdup error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
void aFree_(void *p, const char *file, int line, const char *func)
{
	// ShowMessage("%s:%d: in func %s: aFree %p\n",file,line,func,p);
	if (p)
		FREE(p, file, line, func);

	p = NULL;
}

#ifdef USE_MEMORY_MANAGER

/* USE_MEMORY_MANAGER */

/*
 * }l[W
 *     malloc , free Μπψ¦IΙoιζ€Ι΅½ΰΜB
 *     ‘GΘπsΑΔ’ιΜΕAα±d­Θι©ΰ΅κάΉρB
 *
 * f[^\’ΘΗiΰΎΊθΕ·’άΉρ^^; j
 *     Eπ‘ΜuubNvΙͺ―ΔA³ηΙubNπ‘Μujbgv
 *       Ιͺ―Δ’ά·BjbgΜTCYΝAPubNΜeΚπ‘ΒΙΟzͺ
 *       ΅½ΰΜΕ·B½Ζ¦ΞAPjbg32KBΜκAubNPΒΝ32ByteΜ
 *       jbgͺA1024ΒWάΑΔoΔ’½θA64ByteΜjbgͺ 512ΒWάΑΔ
 *       oΔ’½θ΅ά·Bipadding,unit_head π­j
 *
 *     EubN―mΝNXg(block_prev,block_next) ΕΒΘͺθA―ΆTC
 *       YπΒubN―mΰNXg(hash_prev,hash_nect) ΕΒΘ
 *       ͺΑΔ’ά·B»κΙζθAsvΖΘΑ½ΜΔpͺψ¦IΙs¦ά·B
 */

/* ubNΜACg */
#define BLOCK_ALIGNMENT1	16
#define BLOCK_ALIGNMENT2	64

/* ubNΙόιf[^Κ */
#define BLOCK_DATA_COUNT1	128
#define BLOCK_DATA_COUNT2	608

/* ubNΜε«³: 16*128 + 64*576 = 40KB */
#define BLOCK_DATA_SIZE1	( BLOCK_ALIGNMENT1 * BLOCK_DATA_COUNT1 )
#define BLOCK_DATA_SIZE2	( BLOCK_ALIGNMENT2 * BLOCK_DATA_COUNT2 )
#define BLOCK_DATA_SIZE		( BLOCK_DATA_SIZE1 + BLOCK_DATA_SIZE2 )

/* κxΙmΫ·ιubNΜB */
#define BLOCK_ALLOC		104

/* ubN */
struct block {
	struct block* block_next;		/* ΙmΫ΅½Μζ */
	struct block* unfill_prev;		/* ΜάΑΔ’Θ’Μζ */
	struct block* unfill_next;		/* ΜάΑΔ’Θ’Μζ */
	unsigned short unit_size;		/* jbgΜε«³ */
	unsigned short unit_hash;		/* jbgΜnbV */
	unsigned short unit_count;		/* jbgΜΒ */
	unsigned short unit_used;		/* gpjbg */
	unsigned short unit_unfill;		/* ’gpjbgΜκ */
	unsigned short unit_maxused;	/* gpjbgΜΕεl */
	char   data[ BLOCK_DATA_SIZE ];
};

struct unit_head {
	struct block   *block;
	const  char*   file;
	unsigned short line;
	unsigned short size;
	long           checksum;
};

static struct block* hash_unfill[BLOCK_DATA_COUNT1 + BLOCK_DATA_COUNT2 + 1];
static struct block* block_first, *block_last, block_head;

/* πg’ρΉΘ’ΜζpΜf[^ */
struct unit_head_large {
	unsigned int                  size;
	struct unit_head_large* prev;
	struct unit_head_large* next;
	struct unit_head        unit_head;
};

static struct unit_head_large *unit_head_large_first = NULL;

static struct block* block_malloc(unsigned short hash);
static void          block_free(struct block* p);
static unsigned int        memmgr_usage_bytes;
static unsigned int		   memmgr_max_used_bytes;

#define block2unit(p, n) ((struct unit_head*)(&(p)->data[ p->unit_size * (n) ]))
#define memmgr_assert(v) do { if(!(v)) { printf("Memory manager: assertion '" #v "' failed!\n"); } } while(0)

static unsigned short size2hash( unsigned int size )
{
	if( size <= BLOCK_DATA_SIZE1 ) {
		return (unsigned short)(size + BLOCK_ALIGNMENT1 - 1) / BLOCK_ALIGNMENT1;
	} else if( size <= BLOCK_DATA_SIZE ){
		return (unsigned short)(size - BLOCK_DATA_SIZE1 + BLOCK_ALIGNMENT2 - 1) / BLOCK_ALIGNMENT2
				+ BLOCK_DATA_COUNT1;
	} else {
		return 0xffff;	// ubN·π΄¦ικΝ hash Ι΅Θ’
	}
}

static unsigned int hash2size( unsigned short hash )
{
	if( hash <= BLOCK_DATA_COUNT1) {
		return hash * BLOCK_ALIGNMENT1;
	} else {
		return (hash - BLOCK_DATA_COUNT1) * BLOCK_ALIGNMENT2 + BLOCK_DATA_SIZE1;
	}
}

void* AdrenoMM_Alloc(unsigned int size, const char *file, int line, const char *func )
{
	struct block *block;
	short size_hash = size2hash( size );
	struct unit_head *head;

	if(size == 0) {
		return NULL;
	}
	memmgr_usage_bytes += size;
	if (memmgr_usage_bytes > memmgr_max_used_bytes)
		memmgr_max_used_bytes = memmgr_usage_bytes;

	/* ubN·π΄¦ιΜζΜmΫΙΝAmalloc() πp’ι */
	/* »ΜΫAunit_head.block Ι NULL πγό΅ΔζΚ·ι */
	if(hash2size(size_hash) > BLOCK_DATA_SIZE - sizeof(struct unit_head)) {
		struct unit_head_large* p = (struct unit_head_large*)MALLOC(sizeof(struct unit_head_large)+size,file,line,func);
		if(p != NULL) {
			p->size            = size;
			p->unit_head.block = NULL;
			p->unit_head.size  = 0;
			p->unit_head.file  = file;
			p->unit_head.line  = line;
			p->prev = NULL;
			if (unit_head_large_first == NULL)
				p->next = NULL;
			else {
				unit_head_large_first->prev = p;
				p->next = unit_head_large_first;
			}
			unit_head_large_first = p;
			*(long*)((char*)p + sizeof(struct unit_head_large) - sizeof(long) + size) = 0xdeadbeaf;
			return (char *)p + sizeof(struct unit_head_large) - sizeof(long);
		} else {
			printf("Memory manager::memmgr_alloc failed (allocating %ld+%d bytes at %s:%d).\n", sizeof(struct unit_head_large), size, file, line);
			exit(EXIT_FAILURE);
		}
	}

	/* ―κTCYΜubNͺmΫ³κΔ’Θ’AV½ΙmΫ·ι */
	if(hash_unfill[size_hash]) {
		block = hash_unfill[size_hash];
	} else {
		block = block_malloc(size_hash);
	}

	if( block->unit_unfill == 0xFFFF ) {
		// freeΟέΜζͺcΑΔ’Θ’
		memmgr_assert(block->unit_used <  block->unit_count);
		memmgr_assert(block->unit_used == block->unit_maxused);
		head = block2unit(block, block->unit_maxused);
		block->unit_used++;
		block->unit_maxused++;
	} else {
		head = block2unit(block, block->unit_unfill);
		block->unit_unfill = head->size;
		block->unit_used++;
	}

	if( block->unit_unfill == 0xFFFF && block->unit_maxused >= block->unit_count) {
		// jbgπg’Κ½΅½ΜΕAunfillXg©ην
		if( block->unfill_prev == &block_head) {
			hash_unfill[ size_hash ] = block->unfill_next;
		} else {
			block->unfill_prev->unfill_next = block->unfill_next;
		}
		if( block->unfill_next ) {
			block->unfill_next->unfill_prev = block->unfill_prev;
		}
		block->unfill_prev = NULL;
	}

#ifdef DEBUG_MEMMGR
	{
		unsigned int i, sz = hash2size( size_hash );
		for( i=0; i<sz; i++ )
		{
			if( ((unsigned char*)head)[ sizeof(struct unit_head) - sizeof(long) + i] != 0xfd )
			{
				if( head->line != 0xfdfd )
				{
					printf("Memory manager: freed-data is changed. (freed in %s line %d)\n", head->file,head->line);
				}
				else
				{
					printf("Memory manager: not-allocated-data is changed.\n");
				}
				break;
			}
		}
		memset( (char *)head + sizeof(struct unit_head) - sizeof(long), 0xcd, sz );
	}
#endif

	head->block = block;
	head->file  = file;
	head->line  = line;
	head->size  = (unsigned short)size;
	*(long*)((char*)head + sizeof(struct unit_head) - sizeof(long) + size) = 0xdeadbeaf;
	return (char *)head + sizeof(struct unit_head) - sizeof(long);
};

void* AdrenoMM_CAlloc(unsigned int num, unsigned int size, const char *file, int line, const char *func )
{
	void *p = AdrenoMM_Alloc(num * size,file,line,func);
	memset(p,0,num * size);
	return p;
}

void* AdrenoMM_Realloc(void *memblock, unsigned int size, const char *file, int line, const char *func )
{
	unsigned int old_size;
	if(memblock == NULL) {
		return AdrenoMM_Alloc(size,file,line,func);
	}

	old_size = ((struct unit_head *)((char *)memblock - sizeof(struct unit_head) + sizeof(long)))->size;
	if( old_size == 0 ) {
		old_size = ((struct unit_head_large *)((char *)memblock - sizeof(struct unit_head_large) + sizeof(long)))->size;
	}
	if(old_size > size) {
		// TCYk¬ -> »ΜάάΤ·iθ²«j
		return memblock;
	}  else {
		// TCYgε
		void *p = AdrenoMM_Alloc(size,file,line,func);
		if(p != NULL) {
			memcpy(p,memblock,old_size);
		}
		AdrenoMM_Free(memblock,file,line,func);
		return p;
	}
}

char* AdrenoMM_Strdup(const char *p, const char *file, int line, const char *func )
{
	if(p == NULL) {
		return NULL;
	} else {
		unsigned int len = strlen(p);
		char *string  = (char *)AdrenoMM_Alloc(len + 1,file,line,func);
		memcpy(string,p,len+1);
		return string;
	}
}

wchar_t* AdrenoMM_WStrdup(const wchar_t *p, const char *file, int line, const char *func )
{
	if(p == NULL) {
		return NULL;
	} else {
		unsigned int len = wcslen(p);
		wchar_t *string  = (wchar_t *)AdrenoMM_Alloc((len+1),file,line,func);
		memcpy(string,p,((len+1)*sizeof(wchar_t)));
		return string;
	}
}

void AdrenoMM_Free(void *ptr, const char *file, int line, const char *func )
{
	struct unit_head *head;

	if (ptr == NULL)
		return; 

	head = (struct unit_head *)((char *)ptr - sizeof(struct unit_head) + sizeof(long));
	if(head->size == 0) {
		/* malloc() ΕΌΙmΫ³κ½Μζ */
		struct unit_head_large *head_large = (struct unit_head_large *)((char *)ptr - sizeof(struct unit_head_large) + sizeof(long));
		if(
			*(long*)((char*)head_large + sizeof(struct unit_head_large) - sizeof(long) + head_large->size)
			!= 0xdeadbeaf)
		{
			printf("Memory manager: args of aFree 0x%p is overflowed pointer %s line %d\n", ptr, file, line);
		} else {
			head->size = 0xFFFF;
			if(head_large->prev) {
				head_large->prev->next = head_large->next;
			} else {
				unit_head_large_first  = head_large->next;
			}
			if(head_large->next) {
				head_large->next->prev = head_large->prev;
			}
			memmgr_usage_bytes -= head_large->size;
#ifdef DEBUG_MEMMGR
			// set freed memory to 0xfd
			memset(ptr, 0xfd, head_large->size);
#endif
			FREE(head_large,file,line,func);
		}
	} else {
		/* jbgπϊ */
		struct block *block = head->block;
		if( (char*)head - (char*)block > (long)sizeof(struct block) ) {
			printf("Memory manager: args of aFree 0x%p is invalid pointer %s line %d\n", ptr, file, line);
		} else if(head->block == NULL) {
			printf("Memory manager: args of aFree 0x%p is freed pointer %s:%d@%s\n", ptr, file, line, func);
		} else if(*(long*)((char*)head + sizeof(struct unit_head) - sizeof(long) + head->size) != 0xdeadbeaf) {
			printf("Memory manager: args of aFree 0x%p is overflowed pointer %s line %d\n", ptr, file, line);
		} else {
			memmgr_usage_bytes -= head->size;
			head->block         = NULL;
#ifdef DEBUG_MEMMGR
			memset(ptr, 0xfd, block->unit_size - sizeof(struct unit_head) + sizeof(long) );
			head->file = file;
			head->line = line;
#endif
			memmgr_assert( block->unit_used > 0 );
			if(--block->unit_used == 0) {
				/* ubNΜπϊ */
				block_free(block);
			} else {
				if( block->unfill_prev == NULL) {
					// unfill XgΙΗΑ
					if( hash_unfill[ block->unit_hash ] ) {
						hash_unfill[ block->unit_hash ]->unfill_prev = block;
					}
					block->unfill_prev = &block_head;
					block->unfill_next = hash_unfill[ block->unit_hash ];
					hash_unfill[ block->unit_hash ] = block;
				}
				head->size     = block->unit_unfill;
				block->unit_unfill = (unsigned short)(((uintptr_t)head - (uintptr_t)block->data) / block->unit_size);
			}
		}
	}
}

/* ubNπmΫ·ι */
static struct block* block_malloc(unsigned short hash)
{
	int i;
	struct block *p;
	if(hash_unfill[0] != NULL) {
		/* ubNpΜΜζΝmΫΟέ */
		p = hash_unfill[0];
		hash_unfill[0] = hash_unfill[0]->unfill_next;
	} else {
		/* ubNpΜΜζπV½ΙmΫ·ι */
		p = (struct block*)MALLOC(sizeof(struct block) * (BLOCK_ALLOC), __FILE__, __LINE__, __func__ );
		if(p == NULL) {
			printf("Memory manager::block_alloc failed.\n");
			exit(EXIT_FAILURE);
		}

		if(block_first == NULL) {
			/* ρmΫ */
			block_first = p;
		} else {
			block_last->block_next = p;
		}
		block_last = &p[BLOCK_ALLOC - 1];
		block_last->block_next = NULL;
		/* ubNπA³Ήι */
		for(i=0;i<BLOCK_ALLOC;i++) {
			if(i != 0) {
				// p[0] Ν±κ©ηg€ΜΕNΙΝΑ¦Θ’
				p[i].unfill_next = hash_unfill[0];
				hash_unfill[0]   = &p[i];
				p[i].unfill_prev = NULL;
				p[i].unit_used = 0;
			}
			if(i != BLOCK_ALLOC -1) {
				p[i].block_next = &p[i+1];
			}
		}
	}

	// unfill ΙΗΑ
	memmgr_assert(hash_unfill[ hash ] == NULL);
	hash_unfill[ hash ] = p;
	p->unfill_prev  = &block_head;
	p->unfill_next  = NULL;
	p->unit_size    = (unsigned short)(hash2size( hash ) + sizeof(struct unit_head));
	p->unit_hash    = hash;
	p->unit_count   = BLOCK_DATA_SIZE / p->unit_size;
	p->unit_used    = 0;
	p->unit_unfill  = 0xFFFF;
	p->unit_maxused = 0;
#ifdef DEBUG_MEMMGR
	memset( p->data, 0xfd, sizeof(p->data) );
#endif
	return p;
}

static void block_free(struct block* p)
{
	if( p->unfill_prev ) {
		if( p->unfill_prev == &block_head) {
			hash_unfill[ p->unit_hash ] = p->unfill_next;
		} else {
			p->unfill_prev->unfill_next = p->unfill_next;
		}
		if( p->unfill_next ) {
			p->unfill_next->unfill_prev = p->unfill_prev;
		}
		p->unfill_prev = NULL;
	}

	p->unfill_next = hash_unfill[0];
	hash_unfill[0] = p;
}

unsigned int memmgr_usage (void)
{
	return memmgr_usage_bytes / 1024;
}

#ifdef MEMORYMANAGER_LOG
static char memmer_logfile[128];
static FILE *log_fp;

static void memmgr_log (char *buf)
{
	if( !log_fp )
	{
		time_t raw;
		struct tm* t;

		log_fp = fopen(memmer_logfile,"at");
		if (!log_fp) log_fp = stdout;

		time(&raw);
		t = localtime(&raw);
		fprintf(log_fp, "\nMemory manager: Memory leaks found at %d/%02d/%02d %02dh%02dm%02ds.\n",
			(t->tm_year+1900), (t->tm_mon+1), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	}
	fprintf(log_fp, "%s", buf);
	return;
}
#endif /* MEMORYMANAGER_LOG */

/// Returns 1 if the memory location is active.
/// Active means it is allocated and points to a usable part.
///
/// @param ptr Pointer to the memory
/// @return 1 if the memory is active
int memmgr_verify(void* ptr)
{
	struct block* block = block_first;
	struct unit_head_large* large = unit_head_large_first;

	if( ptr == NULL )
		return 0;// never valid

	// search small blocks
	while( block )
	{
		if( (char*)ptr >= (char*)block && (char*)ptr < ((char*)block) + sizeof(struct block) )
		{// found memory block
			if( block->unit_used && (char*)ptr >= block->data )
			{// memory block is being used and ptr points to a sub-unit
				unsigned int i = (unsigned int)((char*)ptr - block->data)/block->unit_size;
				struct unit_head* head = block2unit(block, i);
				if( i < block->unit_maxused && head->block != NULL )
				{// memory unit is allocated, check if ptr points to the usable part
					return ( (char*)ptr >= ((char*)head) + sizeof(struct unit_head) - sizeof(long)
						&& (char*)ptr < ((char*)head) + sizeof(struct unit_head) - sizeof(long) + head->size );
				}
			}
			return 0;
		}
		block = block->block_next;
	}

	// search large blocks
	while( large )
	{
		if( (char*)ptr >= (char*)large && (char*)ptr < ((char*)large) + large->size )
		{// found memory block, check if ptr points to the usable part
			return ( (char*)ptr >= ((char*)large) + sizeof(struct unit_head_large) - sizeof(long)
				&& (char*)ptr < ((char*)large) + sizeof(struct unit_head_large) - sizeof(long) + large->size );
		}
		large = large->next;
	}
	return 0;
}

static void memmgr_final (void)
{
	struct block *block = block_first;
	struct unit_head_large *large = unit_head_large_first;

#ifdef MEMORYMANAGER_LOG
	int count = 0;
#endif /* MEMORYMANAGER_LOG */

	while (block) {
		if (block->unit_used) {
			int i;
			for (i = 0; i < block->unit_maxused; i++) {
				struct unit_head *head = block2unit(block, i);
				if(head->block != NULL) {
					char* ptr = (char *)head + sizeof(struct unit_head) - sizeof(long);
#ifdef MEMORYMANAGER_LOG
					char buf[1024];
					sprintf (buf,
						"%04d : %s line %d size %lu address 0x%p\n", ++count,
						head->file, head->line, (unsigned long)head->size, ptr);
					memmgr_log (buf);
#endif /* MEMORYMANAGER_LOG */
					// get block pointer and free it [celest]
					AdrenoMM_Free(ptr, ALC_MARK);
				}
			}
		}
		block = block->block_next;
	}

	while(large) {
		struct unit_head_large *large2;
#ifdef MEMORYMANAGER_LOG
		char buf[1024];
		sprintf (buf,
			"%04d : %s line %d size %lu address 0x%p\n", ++count,
			large->unit_head.file, large->unit_head.line, (unsigned long)large->size, &large->unit_head.checksum);
		memmgr_log (buf);
#endif /* MEMORYMANAGER_LOG */
		large2 = large->next;
		FREE(large,file,line,func);
		large = large2;
	}
#ifdef MEMORYMANAGER_LOG
	printf("Memory manager: %d max bytes used.\n", memmgr_max_used_bytes);
	if(count == 0) {
		printf("Memory manager: No memory leaks found.\n");
	} else {
		printf("Memory manager: Memory leaks found and fixed.\n");
		fclose(log_fp);
	}
#endif /* MEMORYMANAGER_LOG */
}

static void memmgr_init (void)
{
#ifdef MEMORYMANAGER_LOG
	sprintf(memmer_logfile, "log/%s.leaks", "AdrenoVM");
	printf("Memory manager initialised: %s\n", memmer_logfile);
	memset(hash_unfill, 0, sizeof(hash_unfill));
#endif /* MEMORYMANAGER_LOG */
}
#endif /* USE_MEMORY_MANAGER */


/*======================================
 * Initialise
 *--------------------------------------
 */


/// Tests the memory for errors and memory leaks.
void AdrenoMM_MemoryCheck()
{
	MEMORY_CHECK();
}


/// Returns 1 if a pointer is valid.
/// The check is best-effort, 0 positives are possible.
int AdrenoMM_VerifyPointer(void* ptr)
{
#ifdef USE_MEMORY_MANAGER
	return memmgr_verify(ptr) && MEMORY_VERIFY(ptr);
#else
	return MEMORY_VERIFY(ptr);
#endif
}


unsigned int AdrenoMM_Usage()
{
#ifdef USE_MEMORY_MANAGER
	return memmgr_usage ();
#else
	return MEMORY_USAGE();
#endif
}

void AdrenoMM_Final()
{
#ifdef USE_MEMORY_MANAGER
	memmgr_final();
#endif
	MEMORY_CHECK();
}

void AdrenoMM_Initialize()
{
#if defined(DMALLOC) && defined(CYGWIN)
	// http://dmalloc.com/docs/latest/online/dmalloc_19.html
	dmalloc_debug_setup(getenv("DMALLOC_OPTIONS"));
#endif
#ifdef GCOLLECT
	// don't garbage collect, only report inaccessible memory that was not deallocated
	GC_find_leak = 1;
	GC_INIT();
#endif
#ifdef USE_MEMORY_MANAGER
	memmgr_init();
#endif
}


#endif
