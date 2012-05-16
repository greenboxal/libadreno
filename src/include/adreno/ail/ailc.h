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

#ifndef AILC_H
#define AILC_H

#include <adreno/vm/vm.h>
#include <stdio.h>

typedef struct
{
	char *Data;
	unsigned int DataPosition, DataSize;
	AdrenoScript *Script;
	AdrenoFunction *CurrentFunction;
	AdrenoOpcodePrefixes Prefix;
} AilCompiler;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AilCompiler_Initialize(AilCompiler *compiler, char *data);
	extern AdrenoScript *AilCompiler_Compile(AilCompiler *compiler);
	extern void AilCompiler_Free(AilCompiler *compiler);

#ifdef __cplusplus
}
#endif

#endif
