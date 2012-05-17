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

#ifdef __cplusplus
extern "C"
{
#endif

	void *AdrenoMM_Alloc	(unsigned int size, const char *file, int line, const char *func);
	void *AdrenoMM_CAlloc	(unsigned int num, unsigned int size, const char *file, int line, const char *func);
	void *AdrenoMM_Realloc	(void *p, unsigned int size, const char *file, int line, const char *func);
	char *AdrenoMM_Strdup	(const char *p, const char *file, int line, const char *func);
	wchar_t *AdrenoMM_WStrdup	(const wchar_t *p, const char *file, int line, const char *func);
	void  AdrenoMM_Free 	(void *p, const char *file, int line, const char *func);

	#define AdrenoAlloc(size) AdrenoMM_Alloc((size), ALC_MARK)
	#define AdrenoRealloc(object, size) AdrenoMM_Realloc((object), (size), ALC_MARK)
	#define AdrenoStrdup(object) AdrenoMM_Strdup((object), ALC_MARK)
	#define AdrenoWStrdup(object) AdrenoMM_WStrdup((object), ALC_MARK)
	#define AdrenoFree(object) AdrenoMM_Free((object), ALC_MARK)

	extern void AdrenoMM_MemoryCheck();
	extern int AdrenoMM_VerifyPointer(void* ptr);
	extern unsigned int AdrenoMM_Usage();
	extern void AdrenoMM_Initialize();
	extern void AdrenoMM_Final();

#ifdef __cplusplus
}
#endif

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
