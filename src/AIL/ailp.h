#ifndef AILP_H
#define AILP_H

#include <adreno/vm/vm.h>
#include <adreno/vm/emit.h>
#include <adreno/ail/ailc.h>

#ifdef __cplusplus
extern "C"
{
#endif
	
	extern void ail_setup_buffer(char *s);
	extern void ail_free_buffer();
	extern int ail_lex(); 
	extern int ail_parse(void *context); 
	extern void ail_error(const char* s);
	extern char *ail_parse_string(char *s);

#ifdef __cplusplus
}
#endif

#endif