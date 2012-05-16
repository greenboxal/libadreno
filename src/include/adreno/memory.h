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

#ifndef ADRENOMEMORY_H
#define ADRENOMEMORY_H

#include <adreno/config.h>

#include <wchar.h>

#if defined(USE_MEMORY_MANAGER)

#define ALC_MARK __FILE__, __LINE__, __FUNCTION__

void *_mmalloc	(unsigned int size, const char *file, int line, const char *func);
void *_mcalloc	(unsigned int num, unsigned int size, const char *file, int line, const char *func);
void *_mrealloc	(void *p, unsigned int size, const char *file, int line, const char *func);
char *_mstrdup	(const char *p, const char *file, int line, const char *func);
wchar_t *_mwstrdup	(const wchar_t *p, const char *file, int line, const char *func);
void  _mfree	(void *p, const char *file, int line, const char *func);

#define AdrenoAlloc(size) _mmalloc((size), ALC_MARK)
#define AdrenoRealloc(object, size) _mrealloc((object), (size), ALC_MARK)
#define AdrenoStrdup(object) _mstrdup((object), ALC_MARK)
#define AdrenoWStrdup(object) _mwstrdup((object), ALC_MARK)
#define AdrenoFree(object) _mfree((object), ALC_MARK)

void malloc_memory_check(void);
int malloc_verify_ptr(void* ptr);
unsigned int malloc_usage (void);
void malloc_init (void);
void malloc_final (void);

#elif defined(USE_MALLOC)

#include <malloc.h>

#define AdrenoAlloc(size) malloc((size))
#define AdrenoRealloc(object, size) realloc((object), (size))
#define AdrenoStrdup(object) strdup((object))
#define AdrenoWStrdup(object) wcsdup((object))
#define AdrenoFree(object) free((object))

#else
#error Memory manager not defined.
#endif

#endif
