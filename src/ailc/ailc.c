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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <adreno/ail/ailc.h>
#include <adreno/memory.h>
#include <adreno/utils/filesystem.h>
#include <adreno/vm/vm.h>

void ShowUsage()
{
	printf("usage: ailc [options] [input file]\n");
	printf("If no input is specified stdin is used.\n");
	printf("Options:\n");
	printf("\t-o <output file>: Save the output to output file [default = ail.bin]\n");
	printf("\t-h: Show this text\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	char *inFile;
	char *outFile = "ail.bin";
	int i = 0;
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

	if (argc < 2)
	{
		ShowUsage();
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			ShowUsage();
			return 1;
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			i++;

			if (i >= argc)
			{
				ShowUsage();
				return 1;
			}

			outFile = argv[i];
		}
		else if (i == argc - 1)
		{
			inFile = argv[i];
		}
		else
		{
			ShowUsage();
			return 1;
		}
	}

	{
		AilCompiler c;
		AdrenoScript *s;
		size_t size;
		char *data = AdrenoFS_LoadFile( inFile, &size );
		if( !data ) {
			return 1;
		}

		AdrenoVM_StaticInit();

		AilCompiler_Initialize(&c, data);
		s = AilCompiler_Compile(&c);
		AilCompiler_Free(&c);
		AdrenoFree(c.Data);

		data = AdrenoScript_Save(s, &size);

		AdrenoFS_SaveFile( outFile, data, size );
		AdrenoScript_Free(s);
		AdrenoFree(data);

		AdrenoVM_StaticDestroy();
	}
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif

	return 0;
}
