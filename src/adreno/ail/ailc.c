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

#include <adreno/ail/ailc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ailp.h"

void AilCompiler_Initialize(AilCompiler *compiler, char *data)
{
	compiler->Data = data;
	compiler->DataPosition = 0;
	compiler->DataSize = strlen(data);
}

AdrenoScript *AilCompiler_Compile(AilCompiler *compiler)
{
	/* Initialize the script object */
	compiler->Script = (AdrenoScript *)AdrenoAlloc(sizeof(AdrenoScript));
	AdrenoScript_Initialize(compiler->Script);
	compiler->Script->GCFlags = GC_FREE;

	ail_setup_buffer(compiler->Data);

	if (ail_parse(compiler) != 0)
	{
		ail_free_buffer();
		AdrenoScript_Free(compiler->Script);
		AdrenoFree(compiler->Script);
		return NULL;
	}

	ail_free_buffer();

	return compiler->Script;
}

void AilCompiler_Free(AilCompiler *compiler)
{
	
}
