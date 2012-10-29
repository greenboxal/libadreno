#ifndef _ADRENO_FILESYSTEM_H
#define _ADRENO_FILESYSTEM_H

#include <adreno/config.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Stores the length of the open file `f` in `result`.
 * Returns 0 on success, -1 on failure
 */
extern int
AdrenoFS_GetFileLength( FILE* f, size_t* result );

/**
 * Returns the entire contents of file `f` in a buffer. The caller must free the
 * buffer. Returns NULL on failure.
 */
extern char *
AdrenoFS_ReadWholeFile( FILE* f, size_t* len );

/**
 * Writes the buffer at 'buf' to file `f`. If the file is not empty, it is
 * truncated & replaced.
 * Returns 0 on success, -1 on failure.
 */
extern int
AdrenoFS_WriteWholeFile( FILE* f, const char* buf, size_t len );

/**
 * Returns the entire contents of `filename` in a buffer. The caller must free the
 * buffer. Returns NULL on failure.
 */
extern char *
AdrenoFS_LoadFile( const char* filename, size_t* len );

/**
 * Writes the string at 'content' to `filename`. If the file exists, it is
 * overwritten.
 * Returns 0 on success, -1 on failure.
 */
extern int
AdrenoFS_SaveFile( const char* filename, const char* buf, size_t len );


#ifdef __cplusplus
}
#endif


#endif

