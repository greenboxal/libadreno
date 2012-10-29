#include <adreno/utils/filesystem.h>
#include <adreno/memory.h>

/* TODO: config.h check for this would be nice */
#ifndef _MSC_VER
#include <unistd.h>
#endif

int
AdrenoFS_GetFileLength( FILE* f, size_t* result )
{
	size_t orig = ftell( f );
	if( -1 == fseek( f, 0, SEEK_END ) ) {
		return -1;
	}

	*result = ftell( f );
	fseek( f, orig, SEEK_SET );

	return 0;
}

char *
AdrenoFS_ReadWholeFile( FILE* f, size_t* len )
{
	size_t l, orig;
	char* buf;
	int ret;

	*len = 0;

	AdrenoFS_GetFileLength( f, &l );

	orig = ftell( f );
	if( orig ) {
		fseek( f, 0, SEEK_SET );
	}

	buf = AdrenoAlloc( l + 1 );
	ret = fread( buf, 1, l, f );
	fseek( f, orig, SEEK_SET );

	if( -1 == ret ) {
		AdrenoFree( buf );
		return NULL;
	}

	*len = l;
	buf[ l ] = '\0';
	return buf;
}

int
AdrenoFS_WriteWholeFile( FILE* f, const char* buf, size_t len )
{
	fseek( f, 0, SEEK_SET );
	ftruncate( fileno( f ), 0 );

	if( len != fwrite( buf, 1, len, f ) ) {
		return -1;
	}

	return 0;
}

char *
AdrenoFS_LoadFile( const char* filename, size_t* len )
{
	FILE* f;
	char* buf;
	
	f = fopen( filename, "rb" );
	if( !f ) {
		return NULL;
	}

	buf = AdrenoFS_ReadWholeFile( f, len );
	
	fclose( f );
	return buf;
}

int
AdrenoFS_SaveFile( const char* filename, const char* buf, size_t len )
{
	FILE* f;
	int ret;
	
	f = fopen( filename, "wb" );
	if( !f ) {
		return -1;
	}

	ret = AdrenoFS_WriteWholeFile( f, buf, len );
	
	fclose( f );
	return ret;
}

