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