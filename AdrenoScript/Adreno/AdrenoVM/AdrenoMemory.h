#ifndef ADRENOMEMORY_H
#define ADRENOMEMORY_H

#include "AdrenoConfig.h"

#ifdef USE_MALLOC

#include <malloc.h>

#define AdrenoAlloc(size) malloc((size))
#define AdrenoRealloc(object, size) realloc((object), (size))
#define AdrenoFree(object) free((object))

#else
#error Memory manager not defined.
#endif

#endif
